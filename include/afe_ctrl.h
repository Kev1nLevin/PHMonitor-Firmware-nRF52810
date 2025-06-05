#pragma once
#include <stdint.h>
void afe_ctrl_init(void);
void afe_power_on(void);
void afe_power_off(void);
float afe_sample_mv(void);
