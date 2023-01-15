#ifndef LIBOPENCM3_USART_H
#define LIBOPENCM3_USART_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/sam/d/memorymap.h>

#define USART_CTRLA(sercom) 		MMIO32(sercom + 0x00)
#define USART_CTRLB(sercom) 		MMIO32(sercom + 0x04)
#define USART_BAUD(sercom) 			MMIO16(sercom + 0x0C)
#define USART_RXPL(sercom) 			MMIO8(sercom + 0x0E)
#define USART_INTENCLR(sercom) 	MMIO8(sercom + 0x14)
#define USART_INTENSET(sercom) 	MMIO8(sercom + 0x16)
#define USART_INTFLAG(sercom) 	MMIO8(sercom + 0x18)
#define USART_STATUS(sercom) 		MMIO16(sercom + 0x1A)
#define USART_SYNCBUSY(sercom) 	MMIO32(sercom + 0x28)
#define USART_DATA(sercom) 			MMIO16(sercom + 0x28)

#define USART_CTRLA_ENABLE      (1 << 1)
#define USART_CTRLA_MODE_USART  (1 << 2)
#define USART_CTRLA_TXPO(tx_pad) (tx_pad << 16)
#define USART_CTRLA_RXPO(rx_pad) (rx_pad << 20)
#define USART_CTRLA_PADS_MASK   (USART_CTRLA_TXPO(3) | USART_CTRLA_RXPO(3))
#define USART_CTRLA_HAS_PARITY 	(1 << 24)


#define USART_CTRLB_CHSIZE(chsize) 		(chsize << 0)
#define USART_CTRLB_CHSIZE_MASK       USART_CTRLB_CHSIZE(3)
#define USART_CTRLB_SBMODE(stopbits) 	(stopbits << 6)
#define USART_CTRLB_SBMODE_MASK       USART_CTRLB_CHSIZE(1)
#define USART_CTRLB_PARITY_ODD     		(1 << 13)
#define USART_CTRLB_PARITY_MASK       USART_CTRLB_CHSIZE(1)
#define USART_CTRLB_TXEN					 		(1 << 16)
#define USART_CTRLB_RXEN					 		(1 << 17)

enum usart_stopbits {
	USART_STOPBITS_1,
	USART_STOPBITS_2,
	USART_STOPBITS_MASK = 1
};

enum usart_parity {
	USART_PARITY_EVEN,
	USART_PARITY_ODD,
	USART_PARITY_NONE,
};

enum usart_mode {
	USART_MODE_DISABLED,
	USART_MODE_RX,
	USART_MODE_TX,
	USART_MODE_TX_RX,
	USART_MODE_MASK = 3
};

enum usart_tx_pad {
	USART_TX_PAD0,
	USART_TX_PAD2,
	USART_TX_PAD0_RTS_CTS,
	USART_TX_PAD_MASK = 3
};

enum usart_rx_pad {
	USART_RX_PAD0,
	USART_RX_PAD1,
	USART_RX_PAD2,
	USART_RX_PAD3,
	USART_RX_PAD_MASK = 3
};

enum usart_chsize {
	USART_CHSIZE_8BIT = 0,
	USART_CHSIZE_9BIT = 1,
	USART_CHSIZE_5BIT = 5,
	USART_CHSIZE_6BIT = 6,
	USART_CHSIZE_7BIT = 7,
	USART_CHSIZE_MASK = 7
};

/* Note: Flow control is enabled by using usart_tx_pad with RTS/CTS. */

void usart_set_baudrate(uint32_t sercom, uint32_t baud);
void usart_set_databits(uint32_t sercom, enum usart_chsize);
void usart_set_stopbits(uint32_t sercom, enum usart_stopbits);
void usart_set_parity(uint32_t sercom, enum usart_parity);
void usart_set_mode(uint32_t sercom, enum usart_mode);
void usart_set_pads(uint32_t sercom, enum usart_tx_pad, enum usart_rx_pad);
void usart_enable(uint32_t sercom);
void usart_disable(uint32_t sercom);
void usart_send(uint32_t sercom, uint16_t data);
uint16_t usart_recv(uint32_t sercom);
void usart_wait_send_ready(uint32_t sercom);
void usart_wait_recv_ready(uint32_t sercom);
void usart_send_blocking(uint32_t sercom, uint16_t data);
uint16_t usart_recv_blocking(uint32_t sercom);
void usart_enable_rx_interrupt(uint32_t sercom);
void usart_disable_rx_interrupt(uint32_t sercom);
void usart_wp_disable(uint32_t sercom);
void usart_wp_enable(uint32_t sercom);
void usart_select_clock(uint32_t sercom, enum usart_clock clk);

#endif