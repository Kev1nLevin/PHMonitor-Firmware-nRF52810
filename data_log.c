#include "data_log.h"
#include "mem_fram.h"

static uint16_t ptr, samples;

#define PTR_ADDR 0x0000
#define CNT_ADDR 0x0002

void data_log_init(void)
{
    fram_read(PTR_ADDR, (uint8_t*)&ptr, 2);
    fram_read(CNT_ADDR, (uint8_t*)&samples, 2);
    if(ptr < DATA_START || ptr > DATA_END){ ptr = DATA_START; samples = 0; }
}

void data_log_store(float fval)
{
    fram_write(ptr, (uint8_t*)&fval, 4);
    ptr += 4; if(ptr > DATA_END) ptr = DATA_START;
    samples++;
    fram_write(PTR_ADDR, (uint8_t*)&ptr, 2);
    fram_write(CNT_ADDR, (uint8_t*)&samples, 2);
}

uint16_t data_log_samples(void){ return samples; }

void data_log_read_chunk(uint16_t addr, uint8_t *p, uint16_t len)
{ fram_read(addr, p, len); }

void data_log_erase(void)
{
    ptr = DATA_START; samples = 0;
    fram_write(PTR_ADDR, (uint8_t*)&ptr, 2);
    fram_write(CNT_ADDR, (uint8_t*)&samples, 2);
}
