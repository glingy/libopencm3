#ifndef LIBOPENCM3_SERNO_H
#define LIBOPENCM3_SERNO_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/sam/d/memorymap.h>

#define SERNO_WORD0 MMIO32(0x0080A00CU)
#define SERNO_WORD1 MMIO32(0x0080A040U)
#define SERNO_WORD2 MMIO32(0x0080A044U)
#define SERNO_WORD3 MMIO32(0x0080A048U)

#endif