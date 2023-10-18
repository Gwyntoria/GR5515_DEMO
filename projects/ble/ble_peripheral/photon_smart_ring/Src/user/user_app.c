/**
 *****************************************************************************************
 *
 * @file user_app.c
 *
 * @brief User function Implementation.
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "user_app.h"
#include "app_error.h"
#include "app_log.h"
#include "dfu_port.h"
#include "gh3x2x_demo.h"
#include "gh3x2x_demo_config.h"
#include "gr55xx_sys.h"
#include "health.h"
#include "hrs.h"
#include "otas.h"
#include "utility.h"

/*
 * DEFINES
 *****************************************************************************************
 */
/**@brief Gapm config data. */
#define DEVICE_NAME               "Health-3220" /**< Device Name which will be set in GAP. */
#define APP_ADV_FAST_MIN_INTERVAL 32            /**< The fast advertising min interval (in units of 0.625 ms). */
#define APP_ADV_FAST_MAX_INTERVAL 48            /**< The fast advertising max interval (in units of 0.625 ms). */
#define APP_ADV_SLOW_MIN_INTERVAL 160           /**< The slow advertising min interval (in units of 0.625 ms). */
#define APP_ADV_SLOW_MAX_INTERVAL 400           /**< The slow advertising max interval (in units of 0.625 ms). */
#define MAX_MTU_DEFUALT           247           /**< Defualt length of maximal MTU acceptable for device. */
#define MAX_MPS_DEFUALT           247           /**< Defualt length of maximal packet size acceptable for device. */
#define MAX_NB_LECB_DEFUALT       10            /**< Defualt length of maximal number of LE Credit based connection. */
#define MAX_TX_OCTET_DEFUALT      251           /**< Default maximum transmitted number of payload octets. */
#define MAX_TX_TIME_DEFUALT       2120          /**< Defualt maximum packet transmission time. */

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static gap_adv_param_t      s_gap_adv_param;      /**< Advertising parameters for legay advertising. */
static gap_adv_time_param_t s_gap_adv_time_param; /**< Advertising time parameter. */

static const uint8_t s_adv_data_set[] = /**< Advertising data. */
    {
        0x11, // Length of this data
        BLE_GAP_AD_TYPE_COMPLETE_LIST_128_BIT_UUID,
        HEALTH_SERVICE_UUID,
        // Device appearance
        0x03,
        BLE_GAP_AD_TYPE_APPEARANCE,
        LO_U16(BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR),
        HI_U16(BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR),

        // Device Services uuid
        0x07,
        BLE_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID,
        LO_U16(BLE_ATT_SVC_HEART_RATE),
        HI_U16(BLE_ATT_SVC_HEART_RATE),
        LO_U16(BLE_ATT_SVC_DEVICE_INFO),
        HI_U16(BLE_ATT_SVC_DEVICE_INFO),
        LO_U16(BLE_ATT_SVC_BATTERY_SERVICE),
        HI_U16(BLE_ATT_SVC_BATTERY_SERVICE),

};

static const uint8_t s_adv_rsp_data_set[] = /**< Scan responce data. */
    {
        0x0c,                          // Length of this data
        BLE_GAP_AD_TYPE_COMPLETE_NAME, // Health
        'H',
        'e',
        'a',
        'l',
        't',
        'h',
        '_',
        '3',
        '2',
        '2',
        '0',
        // Manufacturer specific adv data type
        0x05,
        BLE_GAP_AD_TYPE_MANU_SPECIFIC_DATA,
        // Goodix SIG Company Identifier: 0x04F7
        0xF7,
        0x04,
        // Goodix specific adv data
        0x02,
        0x03,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Initialize gap parameters.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile)parameters
 *          of the device including the device name, appearance, and the preferred connection parameters.
 *****************************************************************************************
 */
static void gap_params_init(void)
{
    sdk_err_t error_code;

    error_code = ble_gap_device_name_set(BLE_GAP_WRITE_PERM_DISABLE, (uint8_t*)DEVICE_NAME, strlen(DEVICE_NAME));

    s_gap_adv_param.adv_intv_max = APP_ADV_SLOW_MAX_INTERVAL;
    s_gap_adv_param.adv_intv_min = APP_ADV_FAST_MIN_INTERVAL;
    s_gap_adv_param.adv_mode     = GAP_ADV_TYPE_ADV_IND;
    s_gap_adv_param.chnl_map     = GAP_ADV_CHANNEL_37_38_39;
    s_gap_adv_param.disc_mode    = GAP_DISC_MODE_NON_DISCOVERABLE;
    s_gap_adv_param.filter_pol   = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;

    error_code = ble_gap_adv_param_set(0, BLE_GAP_OWN_ADDR_STATIC, &s_gap_adv_param);
    APP_ERROR_CHECK(error_code);

    error_code =
        ble_gap_adv_data_set(0, BLE_GAP_ADV_DATA_TYPE_SCAN_RSP, s_adv_rsp_data_set, sizeof(s_adv_rsp_data_set));
    APP_ERROR_CHECK(error_code);

    error_code = ble_gap_adv_data_set(0, BLE_GAP_ADV_DATA_TYPE_DATA, s_adv_data_set, sizeof(s_adv_data_set));
    APP_ERROR_CHECK(error_code);

    s_gap_adv_time_param.duration    = 0;
    s_gap_adv_time_param.max_adv_evt = 0;
}

/**
 *****************************************************************************************
 * @brief Function for process gus service event
 *
 * @param[in] p_evt: Pointer to gus event stucture.
 *****************************************************************************************
 */
static void health_service_process_event(health_evt_t* p_evt)
{
    switch (p_evt->evt_type) {
        case HEALTH_EVT_TX_PORT_OPENED:

            break;

        case HEALTH_EVT_TX_PORT_CLOSED:

            break;

        case HEALTH_EVT_RX_DATA_RECEIVED:
#if (__SUPPORT_PROTOCOL_ANALYZE__)
            Gh3x2xDemoProtocolProcess(p_evt->p_data, p_evt->length);
#endif
            //            printf("recv: ");
            //			for(uint16_t i = 0;i < p_evt->length; i ++)
            //            {
            //                printf("%c",p_evt->p_data[i]);
            //            }
            //            printf("\r\n");
            delay_ms(10);
            break;

        case HEALTH_EVT_TX_DATA_SENT:

            break;
        default:
            break;
    }
}
GU8         hrs_notify_flag = 0;
static void heartrate_service_process_event(hrs_evt_t* p_hrs_evt)
{
    //    sdk_err_t error_code;

    switch (p_hrs_evt->evt_type) {
        case HRS_EVT_NOTIFICATION_ENABLED:
            hrs_notify_flag = 1;
            //            error_code = app_timer_start(s_heart_rate_meas_timer_id, HEART_RATE_MEAS_INTERVAL, NULL);
            //            APP_ERROR_CHECK(error_code);

            //            error_code = app_timer_start(s_rr_interval_meas_timer_id, RR_INTERVAL_INTERVAL, NULL);
            //            APP_ERROR_CHECK(error_code);
            APP_LOG_DEBUG("Heart Rate Notification Enabled.");
            break;

        case HRS_EVT_NOTIFICATION_DISABLED:
            hrs_notify_flag = 0;
            //            app_timer_stop(s_heart_rate_meas_timer_id);
            //            app_timer_stop(s_rr_interval_meas_timer_id);
            APP_LOG_DEBUG("Heart Rate Notification Disabled.");
            break;

        case HRS_EVT_RESET_ENERGY_EXPENDED:
            //            s_energy_expended = 0;
            //            s_energy_cnt      = 10;    // trigger sending m_energy_expended=0
            //            hrs_energy_update(0);
            APP_LOG_DEBUG("Heart Energy Expended Reset.");
            break;

        case HRS_EVT_READ_BODY_SEN_LOCATION:
            // Output log for PTS Automation.
            // The log must be same with the HRS/SEN/CR/BV-01-C's condition defined in hrs_config.xml.
            APP_LOG_DEBUG("Body Sensor Location: 0x%02x.", HRS_SENS_LOC_FINGER);
            break;

        default:
            break;
    }
}

static void dfu_program_start_callback(void);
static void dfu_programing_callback(uint8_t pro);
static void dfu_program_end_callback(uint8_t status);

static dfu_pro_callback_t dfu_pro_call = {
    .dfu_program_start_callback = dfu_program_start_callback,
    .dfu_programing_callback    = dfu_programing_callback,
    .dfu_program_end_callback   = dfu_program_end_callback,
};

static void dfu_program_start_callback(void)
{
    APP_LOG_DEBUG("Start DFU OTA.");
}

static void dfu_programing_callback(uint8_t pro)
{
    APP_LOG_DEBUG("DFU OTA.... %d%%", pro);
}

static void dfu_program_end_callback(uint8_t status)
{
    APP_LOG_DEBUG("DFU OTA complete.");
}
/**
 *****************************************************************************************
 * @brief Function for initializing services
 *****************************************************************************************
 */
static void services_init(void)
{
    sdk_err_t     error_code;
    health_init_t health_init;
    hrs_init_t    hrs_init;

    health_init.evt_handler = health_service_process_event;

    error_code = health_service_init(&health_init);
    APP_ERROR_CHECK(error_code);

    hrs_init.sensor_loc                  = HRS_SENS_LOC_FINGER;
    hrs_init.char_mask                   = HRS_CHAR_MANDATORY | HRS_CHAR_BODY_SENSOR_LOC_SUP | HRS_CHAR_ENGY_EXP_SUP;
    hrs_init.evt_handler                 = heartrate_service_process_event;
    hrs_init.is_sensor_contact_supported = true;

    error_code = hrs_service_init(&hrs_init);
    APP_ERROR_CHECK(error_code);

    dfu_port_init(NULL, &dfu_pro_call);
    dfu_service_init(NULL);
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
uint8_t bleConnectState = 0;

void app_disconnected_handler(uint8_t conn_idx, uint8_t reason)
{
    sdk_err_t error_code;

    error_code = ble_gap_adv_start(0, &s_gap_adv_time_param);
    APP_ERROR_CHECK(error_code);
    bleConnectState = 0;
}

void app_connected_handler(uint8_t conn_idx, const gap_conn_cmp_t* p_param)
{
    APP_LOG_INFO("con_interval: %d us", p_param->con_interval * 1250);
    APP_LOG_INFO("con_latency : 0x%04X", p_param->con_latency);
    APP_LOG_INFO("sup_to      : %d ms", p_param->sup_to * 10);
    bleConnectState = 1;
}

void ble_init_cmp_callback(void)
{
    gap_bdaddr_t  bd_addr;
    sdk_version_t version;
    sdk_err_t     error_code;

    sys_sdk_verison_get(&version);
    APP_LOG_INFO("Goodix GR551x SDK V%d.%d.%02d (commit %u)", version.major, version.minor, version.build,
                 version.commit_id);

    error_code = ble_gap_addr_get(&bd_addr);

    APP_ERROR_CHECK(error_code);
    APP_LOG_INFO("Local Board %02X:%02X:%02X:%02X:%02X:%02X.", bd_addr.gap_addr.addr[5], bd_addr.gap_addr.addr[4],
                 bd_addr.gap_addr.addr[3], bd_addr.gap_addr.addr[2], bd_addr.gap_addr.addr[1],
                 bd_addr.gap_addr.addr[0]);
    APP_LOG_INFO("Template application example started.");

    services_init();
    gap_params_init();

    error_code = ble_gap_adv_start(0, &s_gap_adv_time_param);
    APP_ERROR_CHECK(error_code);
    APP_LOG_INFO("ble gap adv started.");
}
