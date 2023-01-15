#ifndef LIBOPENCM3_NVMCTRL_H
#define LIBOPENCM3_NVMCTRL_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/sam/d/memorymap.h>

#define NVMCTRL_CTRLB MMIO32(NVMCTRL_BASE + 0x04)
#define NVMCTRL_CTRLB_RWS_DUAL (2 << 1)

void nvmctrl_enable_rws(void);

#endif