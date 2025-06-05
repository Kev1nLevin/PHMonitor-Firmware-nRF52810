/*
 * Stage 4 – Ultra‑low‑power data logger for nRF52810
 * Author: Sai Chaitanya
 */
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_clock.h"
#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "data_log.h"
#include "dls_ctrl.h"
#include "afe_ctrl.h"   /* GPIO power control + SAADC helpers */

#define PERIOD_MS   300000UL  /* 5 min */
#define ON_MS       20000UL   /* 20 s */

APP_TIMER_DEF(periodic_timer);
APP_TIMER_DEF(off_timer);

/* ---------- prototypes ---------- */
static void periodic_handler(void *p);
static void off_handler(void *p);

int main(void)
{
    NRF_POWER->DCDCEN = 1;                    /* buck mode */
    nrf_drv_clock_init();
    nrf_drv_clock_lfclk_request(NULL);
    nrf_pwr_mgmt_init();
    app_timer_init();

    afe_ctrl_init();      /* GPIO + SAADC setup, powered‑off */
    twi_bus_init();       /* shared helper inside mem_fram */
    fram_init();          /* external FRAM */
    data_log_init();      /* restore ptr / samples */

    ble_stack_init();     /* SoftDevice + GAP/GATT */
    dls_ctrl_init();      /* Data‑Log Service */

    app_timer_create(&periodic_timer, APP_TIMER_MODE_REPEATED, periodic_handler);
    app_timer_create(&off_timer,      APP_TIMER_MODE_SINGLE_SHOT, off_handler);
    app_timer_start(periodic_timer, APP_TIMER_TICKS(PERIOD_MS), NULL);

    while (true)
    {
        dls_ctrl_process();   /* handle dump TX if in progress */
        nrf_pwr_mgmt_run();   /* sleep */
    }
}

/* ---------- timers ---------- */
static void periodic_handler(void *p)
{
    (void)p;
    afe_power_on();           /* enable GPIO rails & ADC */
    nrf_delay_ms(1000);       /* settle */

    float mv = afe_sample_mv();
    data_log_store(mv);

    if (dls_connected()) dls_ctrl_send_sample_cnt();

    app_timer_start(off_timer, APP_TIMER_TICKS(ON_MS), NULL);
}

static void off_handler(void *p)
{
    (void)p;
    afe_power_off();          /* shut everything down */
    NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}
