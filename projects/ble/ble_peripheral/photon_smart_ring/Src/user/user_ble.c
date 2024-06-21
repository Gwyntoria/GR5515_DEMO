#include "user_ble.h"

#include <stdint.h>

#include "app_log.h"

#include "health.h"
#include "user_func_ctrl.h"

static ble_notify_status s_notify_status_health_cmd  = BLE_NOTIFY_CLOSED;
static ble_notify_status s_notify_status_health_hr   = BLE_NOTIFY_CLOSED;
static ble_notify_status s_notify_status_health_hrv  = BLE_NOTIFY_CLOSED;
static ble_notify_status s_notify_status_health_spo2 = BLE_NOTIFY_CLOSED;
static ble_notify_status s_notify_status_gbc_cmd     = BLE_NOTIFY_CLOSED;
static ble_notify_status s_notify_status_gbc_data    = BLE_NOTIFY_CLOSED;

bool is_ble_notify_opened(ble_notify_type type) {
    switch (type) {
        case BLE_NOTIFY_HEALTH_CMD:
            return s_notify_status_health_cmd == BLE_NOTIFY_OPENED;

        case BLE_NOTIFY_HEALTH_HR:
            return s_notify_status_health_hr == BLE_NOTIFY_OPENED;

        case BLE_NOTIFY_HEALTH_HRV:
            return s_notify_status_health_hrv == BLE_NOTIFY_OPENED;

        case BLE_NOTIFY_HEALTH_SPO2:
            return s_notify_status_health_spo2 == BLE_NOTIFY_OPENED;

        case BLE_NOTIFY_GBC_CMD:
            return s_notify_status_gbc_cmd == BLE_NOTIFY_OPENED;

        case BLE_NOTIFY_GBC_DATA:
            return s_notify_status_gbc_data == BLE_NOTIFY_OPENED;

        default:
            return false;
    }
}

void _gh_cmd_handler(uint8_t* p_data, uint16_t length) {
    if (length < 1) {
        return;
    }

    data_stream_hex(p_data, length);

    switch (p_data[0]) {
        case HEALTH_REQ_OPEN_HR:
            func_ctrl_set_switch_3x2x(kFuncSwitchOn);
            func_ctrl_set_switch_hr(kFuncSwitchOn);
            break;

        case HEALTH_REQ_CLOSE_HR:
            func_ctrl_set_switch_3x2x(kFuncSwitchOff);
            func_ctrl_set_switch_hr(kFuncSwitchOff);
            break;

        case HEALTH_REQ_OPEN_HRV:
            func_ctrl_set_switch_3x2x(kFuncSwitchOn);
            func_ctrl_set_switch_hrv(kFuncSwitchOn);
            break;

        case HEALTH_REQ_CLOSE_HRV:
            func_ctrl_set_switch_3x2x(kFuncSwitchOff);
            func_ctrl_set_switch_hrv(kFuncSwitchOff);
            break;

        case HEALTH_REQ_OPEN_SPO2:
            func_ctrl_set_switch_3x2x(kFuncSwitchOn);
            func_ctrl_set_switch_spo2(kFuncSwitchOn);
            break;

        case HEALTH_REQ_CLOSE_SPO2:
            func_ctrl_set_switch_3x2x(kFuncSwitchOff);
            func_ctrl_set_switch_spo2(kFuncSwitchOff);
            break;

        default:
            uint8_t response = HEALTH_RSP_UNKNOWN_CMD;
            health_notify_cmd(0, &response, 1);
            break;
    }
}

/**
 * @brief Process the events related to the health service.
 *
 * This function is responsible for handling different event types related to the health service.
 * It performs specific actions based on the event type received.
 *
 * @param p_evt Pointer to the health_evt_t structure containing the event type and data.
 */
void health_service_process_event(health_evt_t* p_evt) {
    switch (p_evt->evt_type) {
        case HEALTH_EVT_CMD_PORT_OPENED:
            // APP_LOG_INFO("HEALTH_EVT_CMD_PORT_OPENED");
            s_notify_status_health_cmd = BLE_NOTIFY_OPENED;
            break;

        case HEALTH_EVT_CMD_PORT_CLOSED:
            // APP_LOG_INFO("HEALTH_EVT_CMD_PORT_CLOSED");
            s_notify_status_health_cmd = BLE_NOTIFY_CLOSED;
            break;

        case HEALTH_EVT_CMD_RECEIVED:
            _gh_cmd_handler(p_evt->p_data, p_evt->length);
            break;

        case HEALTH_EVT_CMD_NOTIFIED:
            break;

        case HEALTH_EVT_HR_PORT_OPENED:
            // APP_LOG_INFO("HEALTH_EVT_HR_PORT_OPENED");
            s_notify_status_health_hr = BLE_NOTIFY_OPENED;
            break;

        case HEALTH_EVT_HR_PORT_CLOSED:
            // APP_LOG_INFO("HEALTH_EVT_HR_PORT_CLOSED");
            s_notify_status_health_hr = BLE_NOTIFY_CLOSED;
            break;

        case HEALTH_EVT_HR_NOTIFIED:
            // APP_LOG_INFO("HEALTH_EVT_HR_NOTIFIED");
            break;

        case HEALTH_EVT_HRV_PORT_OPENED:
            // APP_LOG_INFO("HEALTH_EVT_HRV_PORT_OPENED");
            s_notify_status_health_hr = BLE_NOTIFY_OPENED;
            break;

        case HEALTH_EVT_HRV_PORT_CLOSED:
            // APP_LOG_INFO("HEALTH_EVT_HRV_PORT_CLOSED");
            s_notify_status_health_hrv = BLE_NOTIFY_CLOSED;
            break;

        case HEALTH_EVT_HRV_NOTIFIED:
            // APP_LOG_INFO("HEALTH_EVT_HRV_NOTIFIED");
            break;

        case HEALTH_EVT_SPO2_PORT_OPENED:
            // APP_LOG_INFO("HEALTH_EVT_SPO2_PORT_OPENED");
            s_notify_status_health_spo2 = BLE_NOTIFY_OPENED;
            break;

        case HEALTH_EVT_SPO2_PORT_CLOSED:
            // APP_LOG_INFO("HEALTH_EVT_SPO2_PORT_CLOSED");
            s_notify_status_health_spo2 = BLE_NOTIFY_CLOSED;
            break;

        case HEALTH_EVT_SPO2_NOTIFIED:
            // APP_LOG_INFO("HEALTH_EVT_SPO2_NOTIFIED");
            break;

        default:
            break;
    }
}

void _gbc_cmd_handler(uint8_t* p_data, uint16_t length) {
    if (length < 1) {
        return;
    }

    data_stream_hex(p_data, length);

    switch (p_data[0]) {
        case GBC_REQ_ERASE_FLASH:
            func_ctrl_set_switch_fla(kFuncSwitchOn);
            break;

        case GBC_REQ_SEND_DATA:
            func_ctrl_set_switch_ble(kFuncSwitchOn);
            break;
        default:
            uint8_t response = GBC_RSP_UNKNOWN_CMD;
            gbc_notify_cmd(0, &response, 1);
            break;
    }
}

void gbc_service_process_event(gbc_evt_t* p_evt) {
    switch (p_evt->evt_type) {
        case GBC_EVT_CMD_PORT_OPENED:
            // APP_LOG_INFO("GBC_EVT_CMD_PORT_OPENED");
            s_notify_status_gbc_cmd = BLE_NOTIFY_OPENED;
            break;
        
        case GBC_EVT_CMD_PORT_CLOSED:
            // APP_LOG_INFO("GBC_EVT_CMD_PORT_CLOSED");
            s_notify_status_gbc_cmd = BLE_NOTIFY_CLOSED;
            break;

        case GBC_EVT_CMD_RECEIVED:
            // APP_LOG_INFO("GBC_EVT_CMD_RECEIVED");

            _gbc_cmd_handler(p_evt->p_data, p_evt->length);
            break;

        case GBC_EVT_CMD_NOTIFIED:
            // APP_LOG_INFO("GBC_EVT_CMD_NOTIFIED");
            break;

        case GBC_EVT_DATA_PORT_OPENED:
            // APP_LOG_INFO("GBC_EVT_DATA_PORT_OPENED");
            s_notify_status_gbc_data = BLE_NOTIFY_OPENED;
            break;

        case GBC_EVT_DATA_PORT_CLOSED:
            // APP_LOG_INFO("GBC_EVT_DATA_PORT_CLOSED");
            s_notify_status_gbc_data = BLE_NOTIFY_CLOSED;
            break;

        case GBC_EVT_DATA_NOTIFIED:
            // APP_LOG_INFO("GBC_EVT_DATA_INDICATION_COMPLETE");
            break;

        default:
            break;
    }
}