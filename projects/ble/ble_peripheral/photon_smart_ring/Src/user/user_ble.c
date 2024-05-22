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
            APP_LOG_INFO("HEALTH_EVT_HR_PORT_OPENED\n");
            func_ctrl_set_switch_func(kFuncSwitchOn);
            func_ctrl_set_switch_hr(kFuncSwitchOn);
            break;

        case HEALTH_EVT_HR_PORT_CLOSED:
            APP_LOG_INFO("HEALTH_EVT_HR_PORT_CLOSED\n");
            func_ctrl_set_switch_func(kFuncSwitchOff);
            func_ctrl_set_switch_hr(kFuncSwitchOff);
            break;

        case HEALTH_EVT_HRV_PORT_OPENED:
            APP_LOG_INFO("HEALTH_EVT_HRV_PORT_OPENED\n");
            func_ctrl_set_switch_func(kFuncSwitchOn);
            func_ctrl_set_switch_hrv(kFuncSwitchOn);
            break;

        case HEALTH_EVT_HRV_PORT_CLOSED:
            APP_LOG_INFO("HEALTH_EVT_HRV_PORT_CLOSED\n");
            func_ctrl_set_switch_func(kFuncSwitchOff);
            func_ctrl_set_switch_hrv(kFuncSwitchOff);
            break;

        case HEALTH_EVT_SPO2_PORT_OPENED:
            APP_LOG_INFO("HEALTH_EVT_SPO2_PORT_OPENED\n");
            func_ctrl_set_switch_func(kFuncSwitchOn);
            func_ctrl_set_switch_spo2(kFuncSwitchOn);
            break;

        case HEALTH_EVT_SPO2_PORT_CLOSED:
            APP_LOG_INFO("HEALTH_EVT_SPO2_PORT_CLOSED\n");
            func_ctrl_set_switch_func(kFuncSwitchOff);
            func_ctrl_set_switch_spo2(kFuncSwitchOff);
            break;

        case HEALTH_EVT_RR_PORT_OPENED:
            APP_LOG_INFO("HEALTH_EVT_RR_PORT_OPENED\n");
            func_ctrl_set_switch_func(kFuncSwitchOn);
            func_ctrl_set_switch_rr(kFuncSwitchOn);
            break;

        case HEALTH_EVT_RR_PORT_CLOSED:
            APP_LOG_INFO("HEALTH_EVT_RR_PORT_CLOSED\n");
            func_ctrl_set_switch_func(kFuncSwitchOff);
            func_ctrl_set_switch_rr(kFuncSwitchOff);
            break;

        case HEALTH_EVT_HR_DATA_SENT:
            APP_LOG_INFO("HEALTH_EVT_HR_DATA_SENT\n");
            break;

        case HEALTH_EVT_HRV_DATA_SENT:
            APP_LOG_INFO("HEALTH_EVT_HRV_DATA_SENT\n");
            break;

        case HEALTH_EVT_SPO2_DATA_SENT:
            APP_LOG_INFO("HEALTH_EVT_SPO2_DATA_SENT\n");
            break;

        case HEALTH_EVT_RR_DATA_SENT:
            APP_LOG_INFO("HEALTH_EVT_RR_DATA_SENT\n");
            break;

        case HEALTH_EVT_RX_DATA_RECEIVED:
// #if (__SUPPORT_PROTOCOL_ANALYZE__)
//             Gh3x2xDemoProtocolProcess(p_evt->p_data, p_evt->length);
// #endif
            APP_LOG_INFO("HEALTH_EVT_RX_DATA_RECEIVED:");
            for (uint16_t i = 0; i < p_evt->length; i++) {
                printf("    %c", p_evt->p_data[i]);
            }
            printf("\r\n");
            delay_ms(10);
            break;

        default:
            break;
    }
}
