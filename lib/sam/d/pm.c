#include <libopencm3/sam/d/pm.h>

void pm_enable(uint32_t periph) {
  PM_APBCMASK |= periph;
}
