#pragma once
#include <stdint.h>
void data_log_init(void);
void data_log_store(float fval);
uint16_t data_log_samples(void);
void data_log_read_chunk(uint16_t addr, uint8_t *p, uint16_t len);
void data_log_erase(void);
#define DATA_START 0x0004
#define DATA_END   0xFFFC
