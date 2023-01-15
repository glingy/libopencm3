#include <libopencm3/sam/d/gclk.h>

void gclk_enable_gen(enum gclk_gen_id id, enum gclk_gen_source source, enum gclk_gen_divsel divsel, uint_fast16_t div)
{
	GCLK_GENCTRL = GCLK_GENCTRL_ID(id) | GCLK_GENCTRL_SRC(source) | GCLK_GENCTRL_DIVSEL(divsel) | GCLK_GENCTRL_GENEN;
  GCLK_GENDIV = GCLK_GENDIV_ID(id) | GCLK_GENDIV_DIV(div);
}

void gclk_enable_clk(enum gclk_clk_id id, enum gclk_gen_id gen) {
  GCLK_CLKCTRL = GCLK_CLKCTRL_ID(id) | GCLK_CLKCTRL_GEN(gen) | GCLK_CLKCTRL_CLKEN;
}

void sysctrl_enable_dfll48m() {
  // Workaround errata 1.2.1 (Must disable ON_DEMAND before configuring)
  // Enable before configuring DFLLVAL
  SYSCTRL_DFLLCTRL = SYSCTRL_DFLLCTRL_ENABLE;

  SYSCTRL_DFLLVAL = SYSCTRL_DFLLVAL_CALIB;
}
