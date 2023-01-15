#include <libopencm3/sam/d/nvmctrl.h>

void nvmctrl_enable_rws() {
  NVMCTRL_CTRLB |= NVMCTRL_CTRLB_RWS_DUAL;
}