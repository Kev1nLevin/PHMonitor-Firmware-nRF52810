#include "mem_fram.h"
#include "nrf_drv_twi.h"
#include "app_error.h"
#include <string.h>

#define TWISCL_PIN 12
#define TWISDA_PIN 15
#define FRAM_ADDR  0x50
#define PAGE_SIZE  16

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(0);

void twi_bus_init(void)
{
    nrf_drv_twi_config_t cfg = {
        .scl = TWISCL_PIN,
        .sda = TWISDA_PIN,
        .frequency = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_LOW,
        .clear_bus_init = false
    };
    nrf_drv_twi_init(&m_twi, &cfg, NULL, NULL);
    nrf_drv_twi_enable(&m_twi);
}

void fram_init(void){}

static void twi_disable(void){ NRF_TWI0->ENABLE = 0; }

void fram_write(uint16_t addr, uint8_t const *p, uint16_t len)
{
    while(len)
    {
        uint8_t chunk = PAGE_SIZE - (addr & (PAGE_SIZE-1));
        if(chunk > len) chunk = len;
        uint8_t buf[PAGE_SIZE+2];
        buf[0] = addr >> 8;
        buf[1] = addr & 0xFF;
        memcpy(&buf[2], p, chunk);
        nrf_drv_twi_tx(&m_twi, FRAM_ADDR, buf, chunk+2, false);
        addr += chunk; p += chunk; len -= chunk;
    }
    twi_disable();
}

void fram_read(uint16_t addr, uint8_t *p, uint16_t len)
{
    uint8_t a[2] = { addr>>8, addr & 0xFF };
    nrf_drv_twi_tx(&m_twi, FRAM_ADDR, a, 2, true);
    nrf_drv_twi_rx(&m_twi, FRAM_ADDR, p, len);
    twi_disable();
}
