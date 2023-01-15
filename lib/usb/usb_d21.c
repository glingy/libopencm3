/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2011 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * NOTE: Max total packet length is 256 bytes.
 */
#include <string.h>
#include <libopencm3/cm3/common.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/sam/gpio.h>
#include <libopencm3/sam/d/gclk.h>
#include <libopencm3/sam/d/usb.h>
#include <libopencm3/cm3/nvic.h>
#include "usb_private.h"
#include "usb_dwc_common.h"



/* Receive FIFO size in 32-bit words. */
#define NUM_ENDPOINTS 3
#define FIFO_SIZE 256

static char ep_fifo[NUM_ENDPOINTS][2][FIFO_SIZE];

static struct samd21_usbd_endpoint_ram_registers {
	struct __packed {
		uint32_t ADDR;
		uint32_t PCKSIZE;
		uint16_t EXTREG;
		uint8_t  STATUS_BK;
		/* Padding */
		uint32_t : 8;
		uint32_t : 32;
	} BANK[2];
} endpoints[NUM_ENDPOINTS];

static usbd_device *samd21_usbd_init(void);
static void samd21_usbd_set_address(usbd_device * dev, uint8_t addr);
static void samd21_usbd_ep_setup(usbd_device *dev, uint8_t addr, uint8_t type, uint16_t max_size,
	void (*callback)(usbd_device *usbd_dev, uint8_t ep));
static void samd21_usbd_ep_reset(usbd_device *dev);
static void samd21_usbd_ep_stall_set(usbd_device *dev, uint8_t addr, uint8_t stall);
static uint8_t samd21_usbd_ep_stall_get(usbd_device *dev, uint8_t addr);
static void samd21_usbd_ep_nak_set(usbd_device *dev, uint8_t addr, uint8_t nak);
static uint16_t samd21_usbd_ep_write_packet(usbd_device *dev, uint8_t addr, const void *buf, uint16_t len);
static uint16_t samd21_usbd_ep_read_packet(usbd_device *dev, uint8_t addr, void *buf, uint16_t len);
static void samd21_usbd_poll(usbd_device *dev);
static void samd21_usbd_disconnect(usbd_device *usbd_dev, bool disconnected);

static struct _usbd_device usbd_dev;

#define EP_READY(addr) (1 << addr)
static uint8_t ep_read_ready = 0b00000000;

const struct _usbd_driver samd21_usb_driver = {
	.init 						= samd21_usbd_init,
	.set_address 			= samd21_usbd_set_address,
	.ep_setup 				= samd21_usbd_ep_setup,
	.ep_reset 				= samd21_usbd_ep_reset,
	.ep_stall_set 		= samd21_usbd_ep_stall_set,
	.ep_stall_get 		= samd21_usbd_ep_stall_get,
	.ep_nak_set 			= samd21_usbd_ep_nak_set,
	.ep_write_packet 	= samd21_usbd_ep_write_packet,
	.ep_read_packet 	= samd21_usbd_ep_read_packet,
	.poll 						= samd21_usbd_poll,
	.disconnect 			= samd21_usbd_disconnect,
	.base_address 		= USB_BASE,
	.set_address_before_status = 0,
	.supports_multi_packets = 1,
	.rx_fifo_size = FIFO_SIZE,
};

/**
 * USB will run off of GCLK0 is running at 48MHz. 
 * GCLK0 _must_ be configured before calling samd21_usbd_init. 
 * 
 * USB pin modes must also be configured before calling samd21_usbd_init
 **/

/** Initialize the USB device controller hardware of the SAMD21. */
static usbd_device *samd21_usbd_init(void)
{
	gpio_set_af(PORTA, PORT_PMUX_FUN_G, GPIO24 | GPIO25);
	gpio_mode_setup(PORTA, GPIO_MODE_OUTPUT, GPIO_CNF_AF, GPIO24 | GPIO25);

	gclk_enable_clk(GCLK_CLK_USB, GCLK_GEN_ID0);

	// Reset USB peripheral		
	USB_CTRLA = USB_CTRLA_SWRST;
	while (USB_SYNCBUSY & USB_SYNCBUSY_SWRST) continue;

	// Set endpoint address?

	/* Set endpoints address */
	USB_DESCADD = (uint32_t)&endpoints;

	/* Write USB calibration values */
	USB_PADCAL = USB_PADCAL_CALIB;

	/* Set USB full speed mode and attach */
	USB_CTRLB = USB_CTRLB_FULL_SPEED;

	/* Enable USB as Device */
	USB_CTRLA = USB_CTRLA_ENABLE;


	// Already attached on line 84?
	/* Attach USB */

	USB_INTENSET = USB_INTFLAG_EORST;//| USB_INTFLAG_SUSPEND | USB_INTFLAG_WAKEUP;

	usbd_dev.fifo_mem_top = samd21_usb_driver.rx_fifo_size;

	return &usbd_dev;
}

static void samd21_usbd_set_address(usbd_device * dev, uint8_t addr)
{
	(void) dev;
	USB_DADD = USB_DADD_ADDEN | USB_DADD_ADDR(addr);
}

static void samd21_usbd_ep_setup(usbd_device *dev, uint8_t addr, uint8_t type, uint16_t max_size,
	void (*callback)(usbd_device *usbd_dev, uint8_t ep))
{
	/* Translate USB standard type codes to STM32. */
	const uint16_t typelookup[] = {
		[USB_ENDPOINT_ATTR_CONTROL] = USB_EPCFG_EPTYPE_CONTROL,
		[USB_ENDPOINT_ATTR_ISOCHRONOUS] = USB_EPCFG_EPTYPE_ISO,
		[USB_ENDPOINT_ATTR_BULK] = USB_EPCFG_EPTYPE_BULK,
		[USB_ENDPOINT_ATTR_INTERRUPT] = USB_EPCFG_EPTYPE_INTERRUPT,
	};

	uint8_t dir = !!(addr & 0x80);
	addr &= 0x7f;

	/* Set packet size */
	if (max_size >= 64) {
		endpoints[addr].BANK[dir].PCKSIZE = USBEP_PCKSIZE_AUTO_ZLP | USBEP_PCKSIZE_SIZE_64;
	} else if (max_size >= 32) {
		endpoints[addr].BANK[dir].PCKSIZE = USBEP_PCKSIZE_AUTO_ZLP | USBEP_PCKSIZE_SIZE_32;
	} else if (max_size >= 16) {
		endpoints[addr].BANK[dir].PCKSIZE = USBEP_PCKSIZE_AUTO_ZLP | USBEP_PCKSIZE_SIZE_16;
	} else {
		endpoints[addr].BANK[dir].PCKSIZE = USBEP_PCKSIZE_AUTO_ZLP | USBEP_PCKSIZE_SIZE_8;
	}

	if (addr == 0) {
		/* Set packet size */
		if (max_size >= 64) {
			endpoints[addr].BANK[1].PCKSIZE = USBEP_PCKSIZE_AUTO_ZLP | USBEP_PCKSIZE_SIZE_64;
		} else if (max_size >= 32) {
			endpoints[addr].BANK[1].PCKSIZE = USBEP_PCKSIZE_AUTO_ZLP | USBEP_PCKSIZE_SIZE_32;
		} else if (max_size >= 16) {
			endpoints[addr].BANK[1].PCKSIZE = USBEP_PCKSIZE_AUTO_ZLP | USBEP_PCKSIZE_SIZE_16;
		} else {
			endpoints[addr].BANK[1].PCKSIZE = USBEP_PCKSIZE_AUTO_ZLP | USBEP_PCKSIZE_SIZE_8;
		}
	}

	/* Set data address to the top of the fifo buffer space */
	endpoints[addr].BANK[dir].ADDR = (uint32_t)ep_fifo[addr][dir];


	if (dir || (addr == 0)) {
		/* Set EP Bank Type */
		USB_EPCFGn(addr) |= USB_EPCFG_EPTYPE1(typelookup[type]);

		if (callback) {
			dev->user_callback_ctr[addr][USB_TRANSACTION_IN] = callback;
		}

		USB_EPINTENSETn(addr) = USB_EPINTFLAG_TRCPT1;
		/* IN endpoint is not ready */
		/* No valid data to send at the moment. */
		USB_EPSTATUSCLRn(addr) = USB_EPSTATUS_BK1RDY;
	}
	
	if (!dir) {
		USB_EPCFGn(addr) |= USB_EPCFG_EPTYPE0(typelookup[type]);

		if (callback) {
			dev->user_callback_ctr[addr][USB_TRANSACTION_OUT] = callback;
		}

		USB_EPINTENSETn(addr) = USB_EPINTFLAG_TRCPT0;
		/* OUT endpoint is not ready */
		/* Waiting for valid data to be received */
		USB_EPSTATUSCLRn(addr) = USB_EPSTATUS_BK0RDY;
	}

	USB_EPINTENSETn(addr) = USB_EPINTFLAG_RXSTP;
}

static void samd21_usbd_ep_reset(usbd_device *dev)
{
	(void) dev;

	/* Reset all endpoints. */
	for (int i = 1; i < NUM_ENDPOINTS; i++) {
		USB_EPCFGn(i) = 0;
	}
}

static void samd21_usbd_ep_stall_set(usbd_device *dev, uint8_t addr, uint8_t stall)
{
	(void)dev;

	uint8_t dir = !!(addr & 0x80);
	addr &= 0x7f;

	if (dir || (addr == 0)) {
		if (stall) 
			USB_EPSTATUSSETn(addr) = USB_EPSTATUS_STALLRQ1;
		else
			USB_EPSTATUSCLRn(addr) = USB_EPSTATUS_STALLRQ1;
	} else {
		if (stall)
			USB_EPSTATUSSETn(addr) = USB_EPSTATUS_STALLRQ0;
		else
			USB_EPSTATUSCLRn(addr) = USB_EPSTATUS_STALLRQ0;
	}
}

static uint8_t samd21_usbd_ep_stall_get(usbd_device *dev, uint8_t addr)
{
	(void)dev;

	uint8_t dir = !!(addr & 0x80);
	addr &= 0x7f;
	
	if (dir || (addr == 0)) {
		return !!(USB_EPSTATUSn(addr) & USB_EPSTATUS_STALLRQ1);
	} else {
		return !!(USB_EPSTATUSn(addr) & USB_EPSTATUS_STALLRQ0);
	}
}

// Where or what is this even used for?
static void samd21_usbd_ep_nak_set(usbd_device *dev, uint8_t addr, uint8_t nak)
{
	(void)dev;

	/* It does not make sense to force NAK on IN endpoints. */
	if (addr & 0x80) {
		return;
	}

	if (nak)
		USB_EPSTATUSSETn(addr) = USB_EPSTATUS_BK0RDY;
	else
		USB_EPSTATUSCLRn(addr) = USB_EPSTATUS_BK0RDY;
}

static uint16_t samd21_usbd_ep_write_packet(usbd_device *dev, uint8_t addr, const void *buf, uint16_t len)
{
	(void)dev;
	addr &= 0x7F;

	if (USB_EPSTATUSn(addr) & USB_EPSTATUS_BK1RDY) {
		return 0;
	}

	// Copy to the fifo:
	memcpy(ep_fifo[addr][1], buf, len);

	endpoints[addr].BANK[1].ADDR = (uint32_t) ep_fifo[addr][1];
	endpoints[addr].BANK[1].PCKSIZE =
		(endpoints[addr].BANK[1].PCKSIZE & ~(USBEP_PCKSIZE_BYTE_COUNT(0x3FFF) | USBEP_PCKSIZE_MULTI_PKT_SIZE(0x3FFF)))
		| USBEP_PCKSIZE_BYTE_COUNT(len); // Must clear MULTI_PKT_SIZE

	//USB_EPINTFLAGn(addr) = USB_EPINTFLAG_TRCPT1;
	USB_EPSTATUSSETn(addr) = USB_EPSTATUS_BK1RDY;

	return len;
}

static uint16_t samd21_usbd_ep_read_packet(usbd_device *dev, uint8_t addr, void *buf, uint16_t len)
{
	(void)dev;
	addr &= 0x7F;

	// Ensure we have received something
	if (!(USB_EPSTATUSn(addr) & USB_EPSTATUS_BK0RDY)) {
		return 0;
	}

	len = MIN(endpoints[addr].BANK[0].PCKSIZE & 0x3FFF, len);
	memcpy(buf, ep_fifo[addr][0], len);

	// Do I need to force nak? What does this actually mean?
	//if (!st_usbfs_force_nak[addr]) {
	//	USB_SET_EP_RX_STAT(addr, USB_EP_RX_STAT_VALID);
	//}

	USB_EPSTATUSCLRn(addr) = USB_EPSTATUS_BK0RDY;

	return len;
}

static void samd21_usbd_poll(usbd_device *dev)
{	
	uint16_t intflag = USB_INTFLAG;

	if (intflag & USB_INTFLAG_EORST) {
		USB_INTFLAG = USB_INTFLAG_EORST;
		_usbd_reset(dev);
		return;
	}

	uint_fast8_t epints = USB_EPINTSMRY;
	if (epints) {
		uint8_t type = 3;
		for (int i = 0; i < NUM_ENDPOINTS; i++) {
			// Only check if the summary says this endpoint has an interrupt flag set.
			if (!(epints & (1 << i))) continue;
			uint_fast8_t epint = USB_EPINTFLAGn(i);
			if (epint & USB_EPINTFLAG_RXSTP) {
				USB_EPINTFLAGn(i) = USB_EPINTFLAG_RXSTP;
				type = USB_TRANSACTION_SETUP;
				samd21_usbd_ep_read_packet(dev, i, &dev->control_state.req, 8);
			}
			else if (epint & USB_EPINTFLAG_TRCPT0) {
				USB_EPINTFLAGn(i) = USB_EPINTFLAG_TRCPT0;
				type = USB_TRANSACTION_OUT;
			}
			else if (epint & USB_EPINTFLAG_TRCPT1) {
				USB_EPINTFLAGn(i) = USB_EPINTFLAG_TRCPT1;
				type = USB_TRANSACTION_IN;
				ep_read_ready |= EP_READY(i);
			}
			else {
				USB_EPINTFLAGn(i) = 0xFF;
			}

			if (type == 3) continue;

			if (dev->user_callback_ctr[i][type]) {
				dev->user_callback_ctr[i][type](dev, i);
			} else if (type == USB_TRANSACTION_SETUP) {
				USB_EPSTATUSCLRn(i) = USB_EPSTATUS_BK0RDY;
			}
		}
	}

	//if (intflag & USB_INTFLAG_SUSPEND) {
	//	USB_INTFLAG = USB_INTFLAG_SUSPEND;

	//	if (dev->user_callback_suspend) {
	//	dev->user_callback_suspend();
	//	}
	//}

	//if (intflag & USB_INTFLAG_WAKEUP) {
	//	USB_INTFLAG = USB_INTFLAG_WAKEUP;

	//	if (dev->user_callback_resume) {
	//	dev->user_callback_resume();
	//	}
	//}

	//if (intflag & USB_INTFLAG_SOF) {
	//	USB_INTFLAG = USB_INTFLAG_SOF;

	//	if (dev->user_callback_sof) {
	//	dev->user_callback_sof();
	//	}
	//}
}

static void samd21_usbd_disconnect(usbd_device *dev, bool disconnected)
{
	(void) dev;
	if (disconnected) {
		USB_CTRLB |= USB_CTRLB_DETACH;
	} else {
		USB_CTRLB &= ~USB_CTRLB_DETACH;
	}
}