#include "user_ble.h"

#include <stdint.h>

#include "app_log.h"

#include "health.h"
#include "user_func_ctrl.h"

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
        case HEALTH_EVT_HR_PORT_OPENED:
            APP_LOG_INFO("HEALTH_EVT_HR_PORT_OPENED");
            func_ctrl_set_switch_3x2x(kFuncSwitchOn);
            func_ctrl_set_switch_hr(kFuncSwitchOn);
            break;

        case HEALTH_EVT_HR_PORT_CLOSED:
            APP_LOG_INFO("HEALTH_EVT_HR_PORT_CLOSED");
            func_ctrl_set_switch_3x2x(kFuncSwitchOff);
            func_ctrl_set_switch_hr(kFuncSwitchOff);
            break;

        case HEALTH_EVT_HRV_PORT_OPENED:
            APP_LOG_INFO("HEALTH_EVT_HRV_PORT_OPENED");
            func_ctrl_set_switch_3x2x(kFuncSwitchOn);
            func_ctrl_set_switch_hrv(kFuncSwitchOn);
            break;

        case HEALTH_EVT_HRV_PORT_CLOSED:
            APP_LOG_INFO("HEALTH_EVT_HRV_PORT_CLOSED");
            func_ctrl_set_switch_3x2x(kFuncSwitchOff);
            func_ctrl_set_switch_hrv(kFuncSwitchOff);
            break;

        case HEALTH_EVT_SPO2_PORT_OPENED:
            APP_LOG_INFO("HEALTH_EVT_SPO2_PORT_OPENED");
            func_ctrl_set_switch_3x2x(kFuncSwitchOn);
            func_ctrl_set_switch_spo2(kFuncSwitchOn);
            break;

        case HEALTH_EVT_SPO2_PORT_CLOSED:
            APP_LOG_INFO("HEALTH_EVT_SPO2_PORT_CLOSED");
            func_ctrl_set_switch_3x2x(kFuncSwitchOff);
            func_ctrl_set_switch_spo2(kFuncSwitchOff);
            break;

        case HEALTH_EVT_HR_DATA_SENT:
            APP_LOG_INFO("HEALTH_EVT_HR_DATA_SENT");
            break;

        case HEALTH_EVT_HRV_DATA_SENT:
            APP_LOG_INFO("HEALTH_EVT_HRV_DATA_SENT");
            break;

        case HEALTH_EVT_SPO2_DATA_SENT:
            APP_LOG_INFO("HEALTH_EVT_SPO2_DATA_SENT");
            break;

        case HEALTH_EVT_RX_DATA_RECEIVED:
            APP_LOG_INFO("HEALTH_EVT_RX_DATA_RECEIVED");
            data_stream_hex(p_evt->p_data, p_evt->length);
            delay_ms(10);
            break;

        default:
            break;
    }
}

void gbc_service_process_event(gbc_evt_t* p_evt) {
    switch (p_evt->evt_type) {
        case GBC_EVT_TX_NOTIFICATION_ENABLED:
            APP_LOG_INFO("GBC_EVT_TX_NOTIFICATION_ENABLED");
            break;

        case GBC_EVT_TX_NOTIFICATION_DISABLED:
            APP_LOG_INFO("GBC_EVT_TX_NOTIFICATION_DISABLED");
            break;

        case GBC_EVT_TX_NOTIFY_COMPLETE:
            APP_LOG_INFO("GBC_EVT_TX_NOTIFY_COMPLETE");
            break;

        case GBC_EVT_RX_RECEIVE_DATA:
            APP_LOG_INFO("GBC_EVT_RX_RECEIVE_DATA");

            data_stream_hex(p_evt->p_data, p_evt->length);
            break;

        case THS_EVT_SETTINGS_CHANGED:
            APP_LOG_INFO("THS_EVT_SETTINGS_CHANGED");

            data_stream_hex(p_evt->p_data, p_evt->length);
            break;

        case GBC_EVT_DATA_INDICATION_ENABLED:
            APP_LOG_INFO("GBC_EVT_DATA_INDICATION_ENABLED");

            func_ctrl_set_switch_ble(kFuncSwitchOn);
            break;

        case GBC_EVT_DATA_INDICATION_DISABLED:
            APP_LOG_INFO("GBC_EVT_DATA_INDICATION_DISABLED");

            func_ctrl_set_switch_ble(kFuncSwitchOff);
            break;

        case GBC_EVT_DATA_INDICATION_COMPLETE:
            APP_LOG_INFO("GBC_EVT_DATA_INDICATION_COMPLETE");
            break;

        default:
            break;
    }
}