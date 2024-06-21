/**
 *****************************************************************************************
 *
 * @file gus.c
 *
 * @brief Goodix UART Service Implementation.
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
 ******************************************************************************************
 */
#include "health.h"

#include "ble_prf_types.h"
#include "ble_prf_utils.h"
#include "utility.h"

#define HEALTH_SERVICE_UUID \
    { 0xFB, 0x34, 0x9B, 0x5f, 0x40, 0x90, 0x11, 0x10, 0x20, 0x24, 0x01, 0x10, 0x0E, 0xA0, 0x00, 0x01 }
#define HEALTH_SERVER_CMD_UUID \
    { 0xFB, 0x34, 0x9B, 0x5f, 0x40, 0x90, 0x11, 0x10, 0x20, 0x24, 0x01, 0x10, 0x0E, 0xA0, 0x00, 0x02 }
#define HEALTH_SERVER_HR_UUID \
    { 0xFB, 0x34, 0x9B, 0x5f, 0x40, 0x90, 0x11, 0x10, 0x20, 0x24, 0x01, 0x10, 0x0E, 0xA0, 0x00, 0x03 }
#define HEALTH_SERVER_HRV_UUID \
    { 0xFB, 0x34, 0x9B, 0x5f, 0x40, 0x90, 0x11, 0x10, 0x20, 0x24, 0x01, 0x10, 0x0E, 0xA0, 0x00, 0x04 }
#define HEALTH_SERVER_SPO2_UUID \
    { 0xFB, 0x34, 0x9B, 0x5f, 0x40, 0x90, 0x11, 0x10, 0x20, 0x24, 0x01, 0x10, 0x0E, 0xA0, 0x00, 0x05 }

#define ATT_128_PRIMARY_SERVICE BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_PRIMARY_SERVICE)
#define ATT_128_CHARACTERISTIC  BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_CHARACTERISTIC)
#define ATT_128_CLIENT_CHAR_CFG BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DESC_CLIENT_CHAR_CFG)

/*
 * ENUMERATIONS
 *****************************************************************************************
 */
/**@brief Goodix UART Service Attributes Indexes. */
enum health_attr_idx_t {
    HEALTH_IDX_SVC,

    HEALTH_IDX_CMD_CHAR,
    HEALTH_IDX_CMD_VAL,
    HEALTH_IDX_CMD_CFG,

    HEALTH_IDX_HR_CHAR,
    HEALTH_IDX_HR_VAL,
    HEALTH_IDX_HR_CFG,

    HEALTH_IDX_HRV_CHAR,
    HEALTH_IDX_HRV_VAL,
    HEALTH_IDX_HRV_CFG,

    HEALTH_IDX_SPO2_CHAR,
    HEALTH_IDX_SPO2_VAL,
    HEALTH_IDX_SPO2_CFG,

    HEALTH_IDX_NB,
};

struct health_env_t {
    health_init_t health_init;
    uint16_t      start_hdl;
    uint16_t      cmd_ntf_cfg[HEALTH_CONNECTION_MAX];
    uint16_t      hr_ntf_cfg[HEALTH_CONNECTION_MAX];
    uint16_t      hrv_ntf_cfg[HEALTH_CONNECTION_MAX];
    uint16_t      spo2_ntf_cfg[HEALTH_CONNECTION_MAX];
};

static sdk_err_t health_init(void);
static void      health_write_att_cb(uint8_t conn_idx, const gatts_write_req_cb_t* p_param);
static void      health_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t* p_param);
static void      health_cccd_set_cb(uint8_t conn_idx, uint16_t handle, uint16_t cccd_value);
static void      health_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t* p_ntf_ind);

static struct health_env_t s_health_env;
static const uint16_t      s_char_mask = 0xFFFF;
static health_evt_type_t s_now_notify_cmp_type = HEALTH_EVT_INVALID;

/**
 * @brief Full Health Database Description which is used to add attributes into the ATT database
 * 
 */
static const attm_desc_128_t health_att_db[HEALTH_IDX_NB] = {
    // health service
    [HEALTH_IDX_SVC] = {
        ATT_128_PRIMARY_SERVICE,
        READ_PERM_UNSEC,
        0,
        0
    },

    //* Command Characteristic Declaration
    [HEALTH_IDX_CMD_CHAR] = {
        ATT_128_CHARACTERISTIC,
        READ_PERM_UNSEC,
        0,
        0
    },
    [HEALTH_IDX_CMD_VAL]  = {
        HEALTH_SERVER_CMD_UUID,
        WRITE_CMD_PERM_UNSEC | NOTIFY_PERM_UNSEC,
        (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
        HEALTH_MAX_DATA_LEN
    },
    [HEALTH_IDX_CMD_CFG]  = {
        ATT_128_CLIENT_CHAR_CFG,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        0,
        0
    },

    //* HR Characteristic Declaration
    [HEALTH_IDX_HR_CHAR] = {
        ATT_128_CHARACTERISTIC,
        READ_PERM_UNSEC,
        0,
        0
    },
    [HEALTH_IDX_HR_VAL]  = {
        HEALTH_SERVER_HR_UUID,
        NOTIFY_PERM_UNSEC,
        (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
        HEALTH_MAX_DATA_LEN
    },
    [HEALTH_IDX_HR_CFG]  = {
        ATT_128_CLIENT_CHAR_CFG,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        0,
        0
    },

    //* HRV Characteristic Declaration
    [HEALTH_IDX_HRV_CHAR] = {
        ATT_128_CHARACTERISTIC,
        READ_PERM_UNSEC,
        0,
        0
    },
    [HEALTH_IDX_HRV_VAL]  = {
        HEALTH_SERVER_HRV_UUID,
        NOTIFY_PERM_UNSEC,
        (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
        HEALTH_MAX_DATA_LEN
    },
    [HEALTH_IDX_HRV_CFG]  = {
        ATT_128_CLIENT_CHAR_CFG,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        0,
        0
    },

    //* SPO2 Characteristic Declaration
    [HEALTH_IDX_SPO2_CHAR] = {
        ATT_128_CHARACTERISTIC,
        READ_PERM_UNSEC,
        0,
        0
    },
    [HEALTH_IDX_SPO2_VAL]  = {
        HEALTH_SERVER_SPO2_UUID,
        NOTIFY_PERM_UNSEC,
        (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
        HEALTH_MAX_DATA_LEN
    },
    [HEALTH_IDX_SPO2_CFG]  = {
        ATT_128_CLIENT_CHAR_CFG,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        0,
        0
    },
};

/**@brief Service interface required by profile manager. */
static ble_prf_manager_cbs_t health_mgr_cbs = {
    (prf_init_func_t)health_init,
    NULL,
    NULL,
};

/**@brief GATT Server Callbacks. */
static gatts_prf_cbs_t health_gatts_cbs = {
    health_read_att_cb,
    health_write_att_cb,
    NULL,
    health_ntf_ind_cb,
    health_cccd_set_cb,
};

/**@brief Server Information. */
static const prf_server_info_t health_prf_info = {
    .max_connection_nb = HEALTH_CONNECTION_MAX,
    .manager_cbs       = &health_mgr_cbs,
    .gatts_prf_cbs     = &health_gatts_cbs,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Initialize GUS and create DB in ATT.
 *
 * @return Error code to know if service initialization succeed or not.
 *****************************************************************************************
 */
static sdk_err_t health_init(void) {
    const uint8_t     health_svc_uuid[] = HEALTH_SERVICE_UUID;
    uint16_t          start_hdl         = PRF_INVALID_HANDLE;
    sdk_err_t         error_code;
    gatts_create_db_t gatts_db;

    memset(&gatts_db, 0, sizeof(gatts_db));

    gatts_db.shdl                  = &start_hdl;
    gatts_db.uuid                  = health_svc_uuid;
    gatts_db.attr_tab_cfg          = (uint8_t*)&s_char_mask;
    gatts_db.max_nb_attr           = HEALTH_IDX_NB;
    gatts_db.srvc_perm             = SRVC_UUID_TYPE_SET(UUID_TYPE_128);
    gatts_db.attr_tab_type         = SERVICE_TABLE_TYPE_128;
    gatts_db.attr_tab.attr_tab_128 = health_att_db;

    error_code = ble_gatts_srvc_db_create(&gatts_db);

    if (SDK_SUCCESS == error_code) {
        s_health_env.start_hdl = *gatts_db.shdl;
    }

    return error_code;
}

/**
 *****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] conn_idx: Index of the connection.
 * @param[in] p_param:  Pointer to the parameters of the read request.
 *****************************************************************************************
 */
static void health_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t* p_param) {
    gatts_read_cfm_t cfm;
    uint16_t         handle    = p_param->handle;
    uint8_t          tab_index = 0;

    tab_index  = prf_find_idx_by_handle(handle, s_health_env.start_hdl, HEALTH_IDX_NB, (uint8_t*)&s_char_mask);
    cfm.handle = handle;
    cfm.status = BLE_SUCCESS;

    switch (tab_index) {
        case HEALTH_IDX_CMD_CFG:
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t*)&s_health_env.cmd_ntf_cfg[conn_idx];
            cfm.status = BLE_SUCCESS;
            break;

        case HEALTH_IDX_HR_CFG:
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t*)&s_health_env.hr_ntf_cfg[conn_idx];
            cfm.status = BLE_SUCCESS;
            break;

        case HEALTH_IDX_HRV_CFG:
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t*)&s_health_env.hrv_ntf_cfg[conn_idx];
            cfm.status = BLE_SUCCESS;
            break;

        case HEALTH_IDX_SPO2_CFG:
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t*)&s_health_env.spo2_ntf_cfg[conn_idx];
            cfm.status = BLE_SUCCESS;
            break;

        default:
            cfm.length = 0;
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }

    ble_gatts_read_cfm(conn_idx, &cfm);
}

/**
 *****************************************************************************************
 * @brief Handles reception of the write request.
 *
 * @param[in] conn_idx: Index of the connection.
 * @param[in] p_param:  Point to the parameters of the write request.
 *****************************************************************************************
 */
static void health_write_att_cb(uint8_t conn_idx, const gatts_write_req_cb_t* p_param) {
    uint8_t           handle    = p_param->handle;
    uint8_t           tab_index = 0;
    //    uint16_t          flow_ctrl_state;
    uint16_t          cccd_value;
    health_evt_t      event;
    gatts_write_cfm_t cfm;

    tab_index      = prf_find_idx_by_handle(handle, s_health_env.start_hdl, HEALTH_IDX_NB, (uint8_t*)&s_char_mask);
    event.conn_idx = conn_idx;
    cfm.handle     = handle;
    cfm.status     = BLE_SUCCESS;

    switch (tab_index) {
        case HEALTH_IDX_CMD_VAL:
            event.evt_type = HEALTH_EVT_CMD_RECEIVED;
            event.p_data   = (uint8_t*)p_param->value;
            event.length   = p_param->length;
            break;

        case HEALTH_IDX_CMD_CFG:
            cccd_value = le16toh(&p_param->value[0]);
            // event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? HEALTH_EVT_CMD_PORT_OPENED : HEALTH_EVT_CMD_PORT_CLOSED;
            s_health_env.cmd_ntf_cfg[conn_idx] = cccd_value;
            break;

        case HEALTH_IDX_HR_CFG:
            cccd_value = le16toh(&p_param->value[0]);
            // event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? HEALTH_EVT_HR_PORT_OPENED : HEALTH_EVT_HR_PORT_CLOSED;
            s_health_env.hr_ntf_cfg[conn_idx] = cccd_value;
            break;

        case HEALTH_IDX_HRV_CFG:
            cccd_value = le16toh(&p_param->value[0]);
            // event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? HEALTH_EVT_HRV_PORT_OPENED : HEALTH_EVT_HRV_PORT_CLOSED;
            s_health_env.hrv_ntf_cfg[conn_idx] = cccd_value;
            break;

        case HEALTH_IDX_SPO2_CFG:
            cccd_value = le16toh(&p_param->value[0]);
            // event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? HEALTH_EVT_SPO2_PORT_OPENED : HEALTH_EVT_SPO2_PORT_CLOSED;
            s_health_env.spo2_ntf_cfg[conn_idx] = cccd_value;
            break;

        default:
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }

    if (BLE_ATT_ERR_INVALID_HANDLE != cfm.status && HEALTH_EVT_INVALID != event.evt_type && s_health_env.health_init.evt_handler) {
        s_health_env.health_init.evt_handler(&event);
    }

    ble_gatts_write_cfm(conn_idx, &cfm);
}

/**
 *****************************************************************************************
 * @brief Handles reception of the cccd recover request.
 *
 * @param[in]: conn_idx:   Connection index
 * @param[in]: handle:     The handle of cccd attribute.
 * @param[in]: cccd_value: The value of cccd attribute.
 *****************************************************************************************
 */
static void health_cccd_set_cb(uint8_t conn_idx, uint16_t handle, uint16_t cccd_value) {
    uint8_t      tab_index = 0;
    health_evt_t event;

    if (!prf_is_cccd_value_valid(cccd_value)) {
        return;
    }

    tab_index      = prf_find_idx_by_handle(handle, s_health_env.start_hdl, HEALTH_IDX_NB, (uint8_t*)&s_char_mask);
    event.conn_idx = conn_idx;
    event.evt_type = HEALTH_EVT_INVALID;

    switch (tab_index) {
        case HEALTH_IDX_CMD_CFG:
            // event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? HEALTH_EVT_CMD_PORT_OPENED : HEALTH_EVT_CMD_PORT_CLOSED;
            s_health_env.cmd_ntf_cfg[conn_idx] = cccd_value;
            break;

        case HEALTH_IDX_HR_CFG:
            // event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? HEALTH_EVT_HR_PORT_OPENED : HEALTH_EVT_HR_PORT_CLOSED;
            s_health_env.hr_ntf_cfg[conn_idx] = cccd_value;
            break;

        case HEALTH_IDX_HRV_CFG:
            // event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? HEALTH_EVT_HRV_PORT_OPENED : HEALTH_EVT_HRV_PORT_CLOSED;
            s_health_env.hrv_ntf_cfg[conn_idx] = cccd_value;
            break;

        case HEALTH_IDX_SPO2_CFG:
            // event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? HEALTH_EVT_SPO2_PORT_OPENED : HEALTH_EVT_SPO2_PORT_CLOSED;
            s_health_env.spo2_ntf_cfg[conn_idx] = cccd_value;
            break;

        default:
            break;
    }

    if (HEALTH_EVT_INVALID != event.evt_type && s_health_env.health_init.evt_handler) {
        s_health_env.health_init.evt_handler(&event);
    }
}

/**
 *****************************************************************************************
 * @brief Handles reception of the complete event.
 *
 * @param[in] conn_idx:   Connection index.
 * @param[in] status:     The status of GATTC operation.
 * @param[in] p_ntf_ind:  Pointer to the parameters of the complete event.
 *****************************************************************************************
 */
static void health_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t* p_ntf_ind) {
    if (NULL != s_health_env.health_init.evt_handler) {
        health_evt_t event;
        event.conn_idx = conn_idx;

        if (BLE_SUCCESS == status && BLE_GATT_NOTIFICATION == p_ntf_ind->type) {
            // event.evt_type = HEALTH_EVT_TX_DATA_SENT;
            event.evt_type = s_now_notify_cmp_type;
            s_health_env.health_init.evt_handler(&event);
        }
    }
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

sdk_err_t health_notify_cmd(uint8_t conn_idx, uint8_t* p_data, uint16_t length) {
    sdk_err_t        error_code = SDK_ERR_NTF_DISABLED;
    gatts_noti_ind_t send_cmd;

    if (PRF_CLI_START_NTF == s_health_env.cmd_ntf_cfg[conn_idx]) {
        // Fill in the parameter structure
        send_cmd.type   = BLE_GATT_NOTIFICATION;
        send_cmd.handle = prf_find_handle_by_idx(HEALTH_IDX_CMD_VAL, s_health_env.start_hdl, (uint8_t*)&s_char_mask);

        // Pack measured value in database
        send_cmd.length = length;
        send_cmd.value  = p_data;
        s_now_notify_cmp_type = HEALTH_EVT_CMD_NOTIFIED;

        // Send notification to peer device
        error_code = ble_gatts_noti_ind(conn_idx, &send_cmd);
    }

    return error_code;
}

sdk_err_t health_notify_hr(uint8_t conn_idx, uint8_t* p_data, uint16_t length) {
    sdk_err_t        error_code = SDK_ERR_NTF_DISABLED;
    gatts_noti_ind_t send_cmd;

    if (PRF_CLI_START_NTF == s_health_env.hr_ntf_cfg[conn_idx]) {
        // Fill in the parameter structure
        send_cmd.type   = BLE_GATT_NOTIFICATION;
        send_cmd.handle = prf_find_handle_by_idx(HEALTH_IDX_HR_VAL, s_health_env.start_hdl, (uint8_t*)&s_char_mask);

        // Pack measured value in database
        send_cmd.length = length;
        send_cmd.value  = p_data;
        s_now_notify_cmp_type = HEALTH_EVT_HR_NOTIFIED;

        // Send notification to peer device
        error_code = ble_gatts_noti_ind(conn_idx, &send_cmd);
    }

    return error_code;
}

sdk_err_t health_notify_hrv(uint8_t conn_idx, uint8_t* p_data, uint16_t length) {
    sdk_err_t        error_code = SDK_ERR_NTF_DISABLED;
    gatts_noti_ind_t send_cmd;

    if (PRF_CLI_START_NTF == s_health_env.hrv_ntf_cfg[conn_idx]) {
        // Fill in the parameter structure
        send_cmd.type   = BLE_GATT_NOTIFICATION;
        send_cmd.handle = prf_find_handle_by_idx(HEALTH_IDX_HRV_VAL, s_health_env.start_hdl, (uint8_t*)&s_char_mask);

        // Pack measured value in database
        send_cmd.length = length;
        send_cmd.value  = p_data;
        s_now_notify_cmp_type = HEALTH_EVT_HRV_NOTIFIED;

        // Send notification to peer device
        error_code = ble_gatts_noti_ind(conn_idx, &send_cmd);
    }

    return error_code;
}

sdk_err_t health_notify_spo2(uint8_t conn_idx, uint8_t* p_data, uint16_t length) {
    sdk_err_t        error_code = SDK_ERR_NTF_DISABLED;
    gatts_noti_ind_t send_cmd;

    if (PRF_CLI_START_NTF == s_health_env.spo2_ntf_cfg[conn_idx]) {
        // Fill in the parameter structure
        send_cmd.type   = BLE_GATT_NOTIFICATION;
        send_cmd.handle = prf_find_handle_by_idx(HEALTH_IDX_SPO2_VAL, s_health_env.start_hdl, (uint8_t*)&s_char_mask);

        // Pack measured value in database
        send_cmd.length = length;
        send_cmd.value  = p_data;
        s_now_notify_cmp_type = HEALTH_EVT_SPO2_NOTIFIED;

        // Send notification to peer device
        error_code = ble_gatts_noti_ind(conn_idx, &send_cmd);
    }

    return error_code;
}

sdk_err_t health_service_init(health_init_t* p_health_init) {
    if (NULL == p_health_init) {
        return SDK_ERR_POINTER_NULL;
    }

    memcpy(&s_health_env.health_init, p_health_init, sizeof(health_init_t));

    return ble_server_prf_add(&health_prf_info);
}
