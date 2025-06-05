#include "ble_dls.h"
#include "ble_types.h"
#include "ble_advdata.h"
#include "app_error.h"
#include <string.h>

static ble_dls_t *m_p;

static void on_write(ble_evt_t const *evt)
{
    ble_gatts_evt_write_t const *w = &evt->evt.gatts_evt.params.write;
    if(w->handle == m_p->ctrl_handles.value_handle && w->len==1)
        if(m_p->cmd_handler) m_p->cmd_handler((dls_cmd_t)w->data[0]);
}

static void ble_dls_on_ble_evt(ble_evt_t const *evt, void *ctx)
{
    if(evt->header.evt_id == BLE_GATTS_EVT_WRITE) on_write(evt);
    else if(evt->header.evt_id == BLE_GAP_EVT_CONNECTED)
        m_p->conn_handle = evt->evt.gap_evt.conn_handle;
    else if(evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED)
        m_p->conn_handle = BLE_CONN_HANDLE_INVALID;
}

ret_code_t ble_dls_init(ble_dls_t *p, void (*handler)(dls_cmd_t))
{
    m_p = p;
    p->conn_handle = BLE_CONN_HANDLE_INVALID;
    p->cmd_handler = handler;

    uint8_t base[] = DLS_BASE_UUID;
    ble_uuid128_t base_uuid; memcpy(base_uuid.uuid128, base, 16);
    uint8_t type;
    sd_ble_uuid_vs_add(&base_uuid, &type);

    ble_uuid_t service_uuid = { .uuid = DLS_UUID_SERVICE, .type = type };
    sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &p->service_handle);

    /** sample count char */
    ble_add_char_params_t cp = {0};
    cp.uuid = DLS_UUID_SAMPLE;
    cp.uuid_type = type;
    cp.init_len = 2; cp.max_len = 2; cp.p_init_value = NULL;
    cp.char_props.read = 1; cp.char_props.notify = 1;
    cp.read_access = SEC_OPEN; cp.cccd_write_access = SEC_OPEN;
    characteristic_add(p->service_handle, &cp, &p->samp_handles);

    /** control char */
    memset(&cp, 0, sizeof(cp));
    cp.uuid = DLS_UUID_CTRL; cp.uuid_type = type;
    cp.init_len = 1; cp.max_len = 1;
    cp.char_props.write = 1; cp.write_access = SEC_OPEN;
    characteristic_add(p->service_handle, &cp, &p->ctrl_handles);

    /** data char */
    memset(&cp, 0, sizeof(cp));
    cp.uuid = DLS_UUID_DATA; cp.uuid_type = type;
    cp.init_len = 1; cp.max_len = 182; cp.char_props.notify = 1;
    cp.cccd_write_access = SEC_OPEN;
    characteristic_add(p->service_handle, &cp, &p->data_handles);

    NRF_SDH_BLE_OBSERVER(dls_obs, 3, ble_dls_on_ble_evt, NULL);
    return NRF_SUCCESS;
}

ret_code_t ble_dls_sample_notify(ble_dls_t *p, uint16_t cnt)
{
    if(p->conn_handle == BLE_CONN_HANDLE_INVALID) return NRF_ERROR_INVALID_STATE;
    uint16_t len = 2; uint8_t buf[2] = { cnt & 0xFF, cnt >> 8 };
    return sd_ble_gatts_hvx(p->conn_handle,
        &(ble_gatts_hvx_params_t){ .handle = p->samp_handles.value_handle,
                                   .type = BLE_GATT_HVX_NOTIFICATION,
                                   .p_data = buf, .p_len = &len });
}

ret_code_t ble_dls_data_notify(ble_dls_t *p, uint8_t const *d, uint16_t len)
{
    if(p->conn_handle == BLE_CONN_HANDLE_INVALID) return NRF_ERROR_INVALID_STATE;
    ble_gatts_hvx_params_t hvx = {0};
    hvx.handle = p->data_handles.value_handle;
    hvx.type   = BLE_GATT_HVX_NOTIFICATION;
    hvx.p_data = (uint8_t*)d;
    hvx.p_len  = &len;
    return sd_ble_gatts_hvx(p->conn_handle, &hvx);
}

uint16_t dls_connected(void){ return (m_p && m_p->conn_handle != BLE_CONN_HANDLE_INVALID); }
