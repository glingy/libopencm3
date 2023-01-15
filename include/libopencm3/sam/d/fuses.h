#ifndef LIBOPENCM3_FUSES_H
#define LIBOPENCM4_FUSES_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/sam/d/memorymap.h>
#define NVMCTRL_OTP4            (0x00806020) /* NVMCTRL OTP4 Base Address */

#define FUSE_DFLL48M_COARSE_CAL ((MMIO32(NVMCTRL_OTP4 + 4) >> 26) & 0x3F)
#define FUSE_DFLL48M_FINE_CAL   ((MMIO32(NVMCTRL_OTP4 + 8) >> 0) & 0x3FF)

#define FUSE_USB_TRANSN_CAL     ((MMIO32(NVMCTRL_OTP4 + 4) >> 13) & 0x1F)
#define FUSE_USB_TRANSP_CAL     ((MMIO32(NVMCTRL_OTP4 + 4) >> 18) & 0x1F)
#define FUSE_USB_TRIM_CAL       ((MMIO32(NVMCTRL_OTP4 + 4) >> 23) & 0x7)

#endif