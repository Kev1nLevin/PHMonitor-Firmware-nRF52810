#pragma once
#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

#define DLS_BASE_UUID {0x01,0x86,0xE7,0xF8,0x22,0xB3,0x15,0xA2,0xEE,0x11,0x78,0xEB,0x00,0xB3,0x77,0x71}
#define DLS_UUID_SERVICE 0xB300
#define DLS_UUID_SAMPLE  0xB301
#define DLS_UUID_CTRL    0xB302
#define DLS_UUID_DATA    0xB303

typedef enum { DLS_CMD_DUMP = 1, DLS_CMD_ERASE = 2 } dls_cmd_t;

typedef struct {
    uint16_t service_handle;
    ble_gatts_char_handles_t samp_handles;
    ble_gatts_char_handles_t ctrl_handles;
    ble_gatts_char_handles_t data_handles;
    uint16_t conn_handle;
    void (*cmd_handler)(dls_cmd_t);
} ble_dls_t;

ret_code_t ble_dls_init(ble_dls_t *p, void (*handler)(dls_cmd_t));
ret_code_t ble_dls_sample_notify(ble_dls_t *p, uint16_t cnt);
ret_code_t ble_dls_data_notify(ble_dls_t *p, uint8_t const *d, uint16_t len);
uint16_t dls_connected(void);
