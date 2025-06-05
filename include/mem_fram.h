#pragma once
#include <stdint.h>
void twi_bus_init(void);
void fram_init(void);
void fram_write(uint16_t addr, uint8_t const *p, uint16_t len);
void fram_read(uint16_t addr, uint8_t *p, uint16_t len);
