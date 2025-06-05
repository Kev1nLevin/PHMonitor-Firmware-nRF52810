#include "dls_ctrl.h"
#include "ble_dls.h"
#include "data_log.h"
#include "mem_fram.h"
#include "app_util.h"
#include "nrf_delay.h"

static ble_dls_t m_dls;
static bool dump_pending = false;

static void cmd_handler(dls_cmd_t cmd)
{
    if(cmd == DLS_CMD_DUMP)  dump_pending = true;
    else if(cmd == DLS_CMD_ERASE) data_log_erase();
}

void dls_ctrl_init(void)
{
    ble_dls_init(&m_dls, cmd_handler);
}

void dls_ctrl_send_sample_cnt(void)
{
    ble_dls_sample_notify(&m_dls, data_log_samples());
}

void dls_ctrl_process(void)
{
    if(!dump_pending || m_dls.conn_handle == BLE_CONN_HANDLE_INVALID) return;

    uint8_t payload[184];
    uint16_t addr = DATA_START;
    uint16_t left = data_log_samples()*4;
    uint16_t pkt_idx = 0, total_pkts = (left+179)/180;

    while(left && m_dls.conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t chunk = (left >= 180) ? 180 : left;
        payload[0] = pkt_idx & 0xFF;
        payload[1] = pkt_idx >> 8;
        payload[2] = total_pkts & 0xFF;
        payload[3] = total_pkts >> 8;
        data_log_read_chunk(addr, &payload[4], chunk);
        if(ble_dls_data_notify(&m_dls, payload, chunk+4)==NRF_SUCCESS)
        {
            addr += chunk; left -= chunk; pkt_idx++;
        }
        else break; /* buffer full, retry next loop */
        nrf_delay_ms(5);
    }
    if(!left) dump_pending=false;
}
