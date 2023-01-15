#ifndef LIBOPENCM3_USB_H
#define LIBOPENCM3_USB_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/sam/d/memorymap.h>

#include <libopencm3/sam/d/fuses.h>

#define USB_CTRLA     MMIO8(USB_BASE + 0x00)
#define USB_SYNCBUSY  MMIO8(USB_BASE + 0x02)
#define USB_CTRLB     MMIO16(USB_BASE + 0x08)
#define USB_DADD      MMIO8(USB_BASE + 0x0A)
#define USB_INTENSET  MMIO16(USB_BASE + 0x18)
#define USB_INTFLAG   MMIO16(USB_BASE + 0x1C)
#define USB_EPINTSMRY MMIO16(USB_BASE + 0x20)
#define USB_DESCADD   MMIO32(USB_BASE + 0x24)
#define USB_PADCAL    MMIO16(USB_BASE + 0x28)

#define USB_EPCFGn(n)       MMIO8(USB_BASE + 0x100 + (0x20 * n))
#define USB_EPSTATUSCLRn(n) MMIO8(USB_BASE + 0x104 + (0x20 * n))
#define USB_EPSTATUSSETn(n) MMIO8(USB_BASE + 0x105 + (0x20 * n))
#define USB_EPSTATUSn(n)    MMIO8(USB_BASE + 0x106 + (0x20 * n))
#define USB_EPINTFLAGn(n)   MMIO8(USB_BASE + 0x107 + (0x20 * n))
#define USB_EPINTENCLRn(n)  MMIO8(USB_BASE + 0x108 + (0x20 * n))
#define USB_EPINTENSETn(n)  MMIO8(USB_BASE + 0x109 + (0x20 * n))

#define USB_CTRLA_SWRST  (1 << 0)
#define USB_CTRLA_ENABLE (1 << 1)
#define USB_CTRLA_MODE_HOST (1 << 7)

#define USB_PADCAL_CALIB (FUSE_USB_TRANSP_CAL << 0) | (FUSE_USB_TRANSN_CAL << 6) | (FUSE_USB_TRIM_CAL << 12)

#define USB_CTRLB_DETACH (1 << 0)
#define USB_CTRLB_FULL_SPEED (0)

#define USB_INTFLAG_SUSPEND (1 << 0)
#define USB_INTFLAG_MSOF    (1 << 1)
#define USB_INTFLAG_SOF     (1 << 2)
#define USB_INTFLAG_EORST   (1 << 3)
#define USB_INTFLAG_WAKEUP  (1 << 4)
#define USB_INTFLAG_EORSM   (1 << 5)
#define USB_INTFLAG_UPRSM   (1 << 6)
#define USB_INTFLAG_RAMACER (1 << 7)
#define USB_INTFLAG_LPMNYET (1 << 8)
#define USB_INTFLAG_LPMSUSP (1 << 9)

#define USB_SYNCBUSY_SWRST  (1 << 0)

#define USB_DADD_ADDEN (1 << 7)
#define USB_DADD_ADDR(x) (x)

#define USBEP_PCKSIZE_AUTO_ZLP          (1U << 31)
#define USBEP_PCKSIZE_SIZE_8            (0 << 28)
#define USBEP_PCKSIZE_SIZE_16           (1 << 28)
#define USBEP_PCKSIZE_SIZE_32           (2 << 28)
#define USBEP_PCKSIZE_SIZE_64           (3 << 28)
#define USBEP_PCKSIZE_MULTI_PKT_SIZE(x) (x << 14)
#define USBEP_PCKSIZE_BYTE_COUNT(x)     (x << 0)


#define USB_EPCFG_EPTYPE_DISABLED     0
#define USB_EPCFG_EPTYPE_CONTROL      1
#define USB_EPCFG_EPTYPE_ISO          2
#define USB_EPCFG_EPTYPE_BULK         3
#define USB_EPCFG_EPTYPE_INTERRUPT    4
#define USB_EPCFG_EPTYPE_DUAL_BANK    5
#define USB_EPCFG_EPTYPE0(x)          (x << 0)
#define USB_EPCFG_EPTYPE1(x)          (x << 4)
#define USB_EPCFG_NYETDIS             (1 << 7)

#define USB_EPSTATUS_STALLRQ0         (1 << 4)
#define USB_EPSTATUS_STALLRQ1         (1 << 5)
#define USB_EPSTATUS_BK0RDY           (1 << 6)
#define USB_EPSTATUS_BK1RDY           (1 << 7)

#define USB_EPINTFLAG_TRCPT0          (1 << 0)
#define USB_EPINTFLAG_TRCPT1          (1 << 1)
#define USB_EPINTFLAG_RXSTP           (1 << 4)

#endif