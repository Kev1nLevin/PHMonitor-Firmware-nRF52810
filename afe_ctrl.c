#include "afe_ctrl.h"
#include "nrf_gpio.h"
#include "nrf_drv_saadc.h"
#include "app_error.h"

#define LMPVDD_PIN  10
#define LMPVMCHI_PIN 7
#define LMPCS_IN_PIN 6
#define ADCCS_PIN    8
#define ADC_CHANNEL  NRF_SAADC_INPUT_AIN2  /* P0.14 */

static void saadc_cb(nrf_drv_saadc_evt_t const *p_evt) {}

void afe_ctrl_init(void)
{
    nrf_gpio_cfg_output(LMPVDD_PIN);
    nrf_gpio_cfg_output(LMPVMCHI_PIN);
    nrf_gpio_cfg_output(LMPCS_IN_PIN);
    nrf_gpio_cfg_output(ADCCS_PIN);

    afe_power_off();

    nrf_drv_saadc_init(NULL, saadc_cb);
    nrf_saadc_channel_config_t ch = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ADC_CHANNEL);
    ch.gain = NRF_SAADC_GAIN1_6;
    ch.reference = NRF_SAADC_REFERENCE_INTERNAL;
    nrf_drv_saadc_channel_init(0, &ch);
}

void afe_power_on(void)
{
    nrf_gpio_pin_set(LMPVDD_PIN);
    nrf_gpio_pin_set(LMPVMCHI_PIN);
    nrf_gpio_pin_set(LMPCS_IN_PIN);
    nrf_gpio_pin_set(ADCCS_PIN);
}

void afe_power_off(void)
{
    nrf_gpio_pin_clear(ADCCS_PIN);
    nrf_gpio_pin_clear(LMPVDD_PIN);
    nrf_gpio_pin_clear(LMPVMCHI_PIN);
    nrf_gpio_pin_clear(LMPCS_IN_PIN);
}

float afe_sample_mv(void)
{
    nrf_saadc_value_t raw;
    nrf_drv_saadc_sample_convert(0, &raw);
    float mv = (raw * 3600.0f) / 4095.0f;
    nrf_drv_saadc_uninit();
    NRF_SAADC->ENABLE = 0;
    return mv;
}
