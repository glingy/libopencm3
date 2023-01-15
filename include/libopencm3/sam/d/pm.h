#ifndef LIBOPENCM3_PM_H
#define LIBOPENCM3_PM_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/sam/d/memorymap.h>

#define PM_AHBMASK  MMIO32(PM_BASE + 0x14)
#define PM_APBAMASK MMIO32(PM_BASE + 0x18)
#define PM_APBBMASK MMIO32(PM_BASE + 0x1C)
#define PM_APBCMASK MMIO32(PM_BASE + 0x20)

#define PM_PAC2			(1 << 0)
#define PM_EVSYS		(1 << 1)
#define PM_SERCOM0	(1 << 4)
#define PM_SERCOM1	(1 << 3)
#define PM_SERCOM2	(1 << 4)
#define PM_SERCOM3	(1 << 5)
#define PM_SERCOM4	(1 << 6)
#define PM_SERCOM5	(1 << 7)
#define PM_TCC0			(1 << 8)
#define PM_TCC1			(1 << 9)
#define PM_TCC2			(1 << 10)
#define PM_TC3			(1 << 11)
#define PM_TC4			(1 << 12)
#define PM_TC5			(1 << 13)
#define PM_TC6			(1 << 14)
#define PM_TC7			(1 << 15)
#define PM_ADC			(1 << 16)
#define PM_AC				(1 << 17)
#define PM_DAC			(1 << 18)
#define PM_PTC			(1 << 19)
#define PM_I2S			(1 << 20)
#define PM_AC1			(1 << 21)
#define PM_TCC3			(1 << 24)


void pm_enable(uint32_t periph);


#endif