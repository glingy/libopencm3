#ifndef LIBOPENCM3_GCLK_H
#define LIBOPENCM3_GCLK_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/sam/d/memorymap.h>

#include <libopencm3/sam/d/fuses.h>



#define GCLK_CLKCTRL MMIO16(GCLK_BASE + 0x02)
#define GCLK_GENCTRL MMIO32(GCLK_BASE + 0x04)
#define GCLK_GENDIV  MMIO32(GCLK_BASE + 0x08)

#define GCLK_CLKCTRL_ID(x)     ((x & 0x3F) << 0)
#define GCLK_CLKCTRL_GEN(x)    ((x & 0x0F) << 8)
#define GCLK_CLKCTRL_CLKEN     (1 << 14)

#define GCLK_GENCTRL_ID(x)     ((x & 0x0F) << 0)
#define GCLK_GENCTRL_SRC(x)    ((x & 0x1F) << 8)
#define GCLK_GENCTRL_GENEN     (1 << 16)
#define GCLK_GENCTRL_DIVSEL(x) ((x & 0x01) << 20)

#define GCLK_GENDIV_ID(x)     ((x & 0x0F) << 0)
#define GCLK_GENDIV_DIV(x)     ((x & 0xFF) << 8)



#define SYSCTRL_DFLLCTRL MMIO16(SYSCTRL_BASE + 0x24)
#define SYSCTRL_DFLLVAL  MMIO32(SYSCTRL_BASE + 0x28)

#define SYSCTRL_DFLLCTRL_ENABLE (1 << 1)

#define SYSCTRL_DFLLVAL_CALIB ((FUSE_DFLL48M_COARSE_CAL << 10) | (FUSE_DFLL48M_FINE_CAL << 0))




enum gclk_gen_divsel {
  GCLK_GEN_DIVSEL_DIRECT, // div = DIV
  GCLK_GEN_DIVSEL_POWER   // div = 2^(DIV + 1)
};

enum gclk_gen_source {
  GCLK_GEN_SRC_XOSC,
  GCLK_GEN_SRC_GCLKIN,
  GCLK_GEN_SRC_GCLKGEN1,
  GCLK_GEN_SRC_OSCULP32K,
  GCLK_GEN_SRC_OSC32K,
  GCLK_GEN_SRC_XOSC32K,
  GCLK_GEN_SRC_OSC8M,
  GCLK_GEN_SRC_DFLL48M,
  GCLK_GEN_SRC_FDPLL96M
};

enum gclk_gen_id {
  GCLK_GEN_ID0,
  GCLK_GEN_ID1,
  GCLK_GEN_ID2,
  GCLK_GEN_ID3,
  GCLK_GEN_ID4,
  GCLK_GEN_ID5,
  GCLK_GEN_ID6,
  GCLK_GEN_ID7,
  GCLK_GEN_ID8,
};

enum gclk_clk_id {
  GCLK_CLK_DFLL48M_REF, 
  GCLK_CLK_DPLL, 
  GCLK_CLK_DPLL_32K, 
  GCLK_CLK_WDT, 
  GCLK_CLK_RTC, 
  GCLK_CLK_EIC, 
  GCLK_CLK_USB, 
  GCLK_CLK_EVSYS_CHANNEL_0, 
  GCLK_CLK_EVSYS_CHANNEL_1,
  GCLK_CLK_EVSYS_CHANNEL_2, 
  GCLK_CLK_EVSYS_CHANNEL_3, 
  GCLK_CLK_EVSYS_CHANNEL_4, 
  GCLK_CLK_EVSYS_CHANNEL_5, 
  GCLK_CLK_EVSYS_CHANNEL_6,
  GCLK_CLK_EVSYS_CHANNEL_7, 
  GCLK_CLK_EVSYS_CHANNEL_8, 
  GCLK_CLK_EVSYS_CHANNEL_9, 
  GCLK_CLK_EVSYS_CHANNEL_10, 
  GCLK_CLK_EVSYS_CHANNEL_11,
  GCLK_CLK_SERCOMx_SLOW, 
  GCLK_CLK_SERCOM0_CORE, 
  GCLK_CLK_SERCOM1_CORE, 
  GCLK_CLK_SERCOM2_CORE, 
  GCLK_CLK_SERCOM3_CORE,
  GCLK_CLK_SERCOM4_CORE, 
  GCLK_CLK_SERCOM5_CORE, 
  GCLK_CLK_TCC0_TCC1,
  GCLK_CLK_TCC2_TC3,
  GCLK_CLK_TC4_TC5,
  GCLK_CLK_TC6_TC7,
  GCLK_CLK_ADC, 
  GCLK_CLK_AC_AC1_DIG,
  GCLK_CLK_AC_AC1_ANA,
  GCLK_CLK_DAC, 
  GCLK_CLK_PTC, 
  GCLK_CLK_I2S_0, 
  GCLK_CLK_I2S_1, 
  GCLK_CLK_TCC3,
};

void gclk_enable_gen(enum gclk_gen_id id, enum gclk_gen_source source, enum gclk_gen_divsel divsel, uint_fast16_t div);
void gclk_enable_clk(enum gclk_clk_id id, enum gclk_gen_id gen);
void sysctrl_enable_dfll48m(void);

#endif