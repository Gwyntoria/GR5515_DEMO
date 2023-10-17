/**
 *****************************************************************************************
 *
 * @file ghealth.c
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
#include "ghealth.h"

#include "ble_prf_types.h"
#include "ble_prf_utils.h"
#include "utility.h"

/*
 * DEFINES
 *****************************************************************************************
 */
/**@brief The UUIDs of GHEALTH characteristics. */
#define GHEALTH_SERVER_TX_UUID {0xFB, 0x34, 0x9B, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}
#define GHEALTH_SERVER_RX_UUID {0xFB, 0x34, 0x9B, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00}
#define GHEALTH_FLOW_CTRL_UUID {0x1B, 0xD7, 0x90, 0xEC, 0xE8, 0xB9, 0x75, 0x80, 0x0A, 0x46, 0x44, 0xD3, 0x04, 0x02, 0xED, 0xA6}

/**@brief Macros for conversion of 128bit to 16bit UUID. */
#define ATT_128_PRIMARY_SERVICE BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_PRIMARY_SERVICE)
#define ATT_128_CHARACTERISTIC  BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_CHARACTERISTIC)
#define ATT_128_CLIENT_CHAR_CFG BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DESC_CLIENT_CHAR_CFG)

/*
 * ENUMERATIONS
 *****************************************************************************************
 */
/**@brief Goodix UART Service Attributes Indexes. */
enum ghealth_attr_idx_t
{
    GHEALTH_IDX_SVC,
    
    GHEALTH_IDX_TX_CHAR,
    GHEALTH_IDX_TX_VAL,
    GHEALTH_IDX_TX_CFG,

    GHEALTH_IDX_RX_CHAR,
    GHEALTH_IDX_RX_VAL,

    GHEALTH_IDX_FLOW_CTRL_CHAR,
    GHEALTH_IDX_FLOW_CTRL_VAL,
    GHEALTH_IDX_FLOW_CTRL_CFG,

    GHEALTH_IDX_NB,
};

/*
 * STRUCTURES
 *****************************************************************************************
 */
/**@brief Goodix UART Service environment variable. */
struct ghealth_env_t
{
    ghealth_init_t ghealth_init;                 /**< Goodix UART Service initialization variables. */
    uint16_t       start_hdl;                    /**< Start handle of services */
    uint16_t tx_ntf_cfg[GHEALTH_CONNECTION_MAX]; /**< TX Characteristic Notification configuration of the peers. */
    uint16_t flow_ctrl_ntf_cfg[GHEALTH_CONNECTION_MAX]; /**< Flow Control Characteristic Notification configuration of
                                                           the peers. */
};

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static sdk_err_t ghealth_init(void);
static void      ghealth_write_att_cb(uint8_t conn_idx, const gatts_write_req_cb_t* p_param);
static void      ghealth_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t* p_param);
static void      ghealth_cccd_set_cb(uint8_t conn_idx, uint16_t handle, uint16_t cccd_value);
static void      ghealth_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t* p_ntf_ind);

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static struct ghealth_env_t s_ghealth_env;
static const uint16_t   s_char_mask = 0xFFFF;

/**@brief Full GHEALTH Database Description which is used to add attributes into the ATT database. */
static const attm_desc_128_t ghealth_att_db[GHEALTH_IDX_NB] =
{
    // GHEALTH service
    [GHEALTH_IDX_SVC]            = {ATT_128_PRIMARY_SERVICE, READ_PERM_UNSEC, 0, 0},

    // GHEALTH TX Characteristic Declaration
    [GHEALTH_IDX_TX_CHAR]        = {ATT_128_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // GHEALTH TX Characteristic Value
    [GHEALTH_IDX_TX_VAL]         = {GHEALTH_SERVER_TX_UUID, 
                                NOTIFY_PERM_UNSEC,
                                (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
                                GHEALTH_MAX_DATA_LEN},
    // GHEALTH TX Characteristic - Client Characteristic Configuration Descriptor
    [GHEALTH_IDX_TX_CFG]         = {ATT_128_CLIENT_CHAR_CFG,
                                READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
                                0,
                                0},

    // GHEALTH RX Characteristic Declaration
    [GHEALTH_IDX_RX_CHAR]        = {ATT_128_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // GHEALTH RX Characteristic Value
    [GHEALTH_IDX_RX_VAL]         = {GHEALTH_SERVER_RX_UUID,
                                WRITE_REQ_PERM_UNSEC | WRITE_CMD_PERM_UNSEC,
                                (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
                                GHEALTH_MAX_DATA_LEN},

    // GHEALTH FLOW_CTRL Characteristic Declaration
    [GHEALTH_IDX_FLOW_CTRL_CHAR] = {ATT_128_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // GHEALTH FLOW_CTRL Characteristic Value
    [GHEALTH_IDX_FLOW_CTRL_VAL]  = {GHEALTH_FLOW_CTRL_UUID,
                                NOTIFY_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
                                (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
                                GHEALTH_MAX_DATA_LEN},
    // GHEALTH FLOW_CTRL Characteristic - Client Characteristic Configuration Descriptor
    [GHEALTH_IDX_FLOW_CTRL_CFG]  = {ATT_128_CLIENT_CHAR_CFG,
                                READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
                                0,
                                0},
};

/**@brief GHEALTH Service interface required by profile manager. */
static ble_prf_manager_cbs_t ghealth_mgr_cbs =
{
    (prf_init_func_t)ghealth_init,
    NULL,
    NULL,
};

/**@brief GHEALTH GATT Server Callbacks. */
static gatts_prf_cbs_t ghealth_gatts_cbs =
{
    ghealth_read_att_cb,
    ghealth_write_att_cb,
    NULL,
    ghealth_ntf_ind_cb,
    ghealth_cccd_set_cb
};

/**@brief GHEALTH Server Information. */
static const prf_server_info_t ghealth_prf_info =
{
    .max_connection_nb = GHEALTH_CONNECTION_MAX,
    .manager_cbs       = &ghealth_mgr_cbs,
    .gatts_prf_cbs     = &ghealth_gatts_cbs
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Initialize GHEALTH and create DB in ATT.
 *
 * @return Error code to know if service initialization succeed or not.
 *****************************************************************************************
 */
static sdk_err_t ghealth_init(void)
{
    const uint8_t     ghealth_svc_uuid[] = {GHEALTH_SERVICE_UUID};
    uint16_t          start_hdl      = PRF_INVALID_HANDLE;
    sdk_err_t         error_code;
    gatts_create_db_t gatts_db;
    
    memset(&gatts_db, 0, sizeof(gatts_db));

    gatts_db.shdl                  = &start_hdl;
    gatts_db.uuid                  = ghealth_svc_uuid;
    gatts_db.attr_tab_cfg          = (uint8_t *)&s_char_mask;
    gatts_db.max_nb_attr           = GHEALTH_IDX_NB;
    gatts_db.srvc_perm             = SRVC_UUID_TYPE_SET(UUID_TYPE_128);
    gatts_db.attr_tab_type         = SERVICE_TABLE_TYPE_128;
    gatts_db.attr_tab.attr_tab_128 = ghealth_att_db;

    error_code = ble_gatts_srvc_db_create(&gatts_db);

    if (SDK_SUCCESS == error_code)
    {
        s_ghealth_env.start_hdl = *gatts_db.shdl;
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
static void ghealth_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t *p_param)
{
    gatts_read_cfm_t cfm;
    uint16_t         handle    = p_param->handle;
    uint8_t          tab_index = 0;

    tab_index  = prf_find_idx_by_handle(handle, s_ghealth_env.start_hdl, GHEALTH_IDX_NB, (uint8_t *)&s_char_mask);
    cfm.handle = handle;
    cfm.status = BLE_SUCCESS;

    switch (tab_index)
    {
        case GHEALTH_IDX_TX_CFG:
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t *)&s_ghealth_env.tx_ntf_cfg[conn_idx];
            cfm.status = BLE_SUCCESS;
            break;

        case GHEALTH_IDX_FLOW_CTRL_CFG:
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t *)&s_ghealth_env.flow_ctrl_ntf_cfg[conn_idx];
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
static void   ghealth_write_att_cb(uint8_t conn_idx, const gatts_write_req_cb_t *p_param)
{
    uint8_t           handle    = p_param->handle;
    uint8_t           tab_index = 0;
    uint16_t          flow_ctrl_state;
    uint16_t          cccd_value;
    ghealth_evt_t         event;
    gatts_write_cfm_t cfm;

    tab_index      = prf_find_idx_by_handle(handle,s_ghealth_env.start_hdl, GHEALTH_IDX_NB, (uint8_t *)&s_char_mask);
    event.conn_idx = conn_idx;
    cfm.handle     = handle;
    cfm.status     = BLE_SUCCESS;
    
    switch (tab_index)
    {
        case GHEALTH_IDX_RX_VAL:
            event.evt_type       = GHEALTH_EVT_RX_DATA_RECEIVED;
            event.p_data = (uint8_t *)p_param->value;
            event.length = p_param->length;
            break;

        case GHEALTH_IDX_TX_CFG:
            cccd_value     = le16toh(&p_param->value[0]);
            event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? GHEALTH_EVT_TX_PORT_OPENED : GHEALTH_EVT_TX_PORT_CLOSED;
            s_ghealth_env.tx_ntf_cfg[conn_idx] = cccd_value;
            break;

        case GHEALTH_IDX_FLOW_CTRL_CFG:
            cccd_value     = le16toh(&p_param->value[0]);
            event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? GHEALTH_EVT_FLOW_CTRL_ENABLE : GHEALTH_EVT_FLOW_CTRL_DISABLE;
            s_ghealth_env.flow_ctrl_ntf_cfg[conn_idx] = cccd_value;
            break;

        case GHEALTH_IDX_FLOW_CTRL_VAL:
            flow_ctrl_state = p_param->value[0];

            if (FLOW_OFF == flow_ctrl_state)
            {
                event.evt_type = GHEALTH_EVT_TX_FLOW_OFF;
            }
            else if (FLOW_ON == flow_ctrl_state)
            {
                event.evt_type = GHEALTH_EVT_TX_FLOW_ON;
            }
            break;

        default:
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }

    if (BLE_ATT_ERR_INVALID_HANDLE != cfm.status && GHEALTH_EVT_INVALID != event.evt_type && s_ghealth_env.ghealth_init.evt_handler)
    {
        s_ghealth_env.ghealth_init.evt_handler(&event);
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
static void ghealth_cccd_set_cb(uint8_t conn_idx, uint16_t handle, uint16_t cccd_value)
{
    uint8_t           tab_index = 0;
    ghealth_evt_t         event;

    if (!prf_is_cccd_value_valid(cccd_value))
    {
        return;
    }

    tab_index      = prf_find_idx_by_handle(handle,s_ghealth_env.start_hdl, GHEALTH_IDX_NB, (uint8_t *)&s_char_mask);
    event.conn_idx = conn_idx;
    event.evt_type = GHEALTH_EVT_INVALID;

    switch (tab_index)
    {
        case GHEALTH_IDX_TX_CFG:
            event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? GHEALTH_EVT_TX_PORT_OPENED : GHEALTH_EVT_TX_PORT_CLOSED;
            s_ghealth_env.tx_ntf_cfg[conn_idx] = cccd_value;
            break;

        case GHEALTH_IDX_FLOW_CTRL_CFG:
            event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? GHEALTH_EVT_FLOW_CTRL_ENABLE : GHEALTH_EVT_FLOW_CTRL_DISABLE;
            s_ghealth_env.flow_ctrl_ntf_cfg[conn_idx] = cccd_value;
            break;

        default:
            break;
    }

    if (GHEALTH_EVT_INVALID != event.evt_type && s_ghealth_env.ghealth_init.evt_handler)
    {
        s_ghealth_env.ghealth_init.evt_handler(&event);
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
static void ghealth_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t *p_ntf_ind)
{
    if (NULL != s_ghealth_env.ghealth_init.evt_handler)
    {
        ghealth_evt_t event;
        event.conn_idx = conn_idx;

        if (BLE_SUCCESS == status && BLE_GATT_NOTIFICATION == p_ntf_ind->type)
        {
            event.evt_type = GHEALTH_EVT_TX_DATA_SENT;
            s_ghealth_env.ghealth_init.evt_handler(&event);
        }
    }
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
sdk_err_t ghealth_tx_data_send(uint8_t conn_idx, uint8_t *p_data, uint16_t length)
{
    sdk_err_t        error_code = SDK_ERR_NTF_DISABLED;
    gatts_noti_ind_t send_cmd;

    if (PRF_CLI_START_NTF == s_ghealth_env.tx_ntf_cfg[conn_idx])
    {
        // Fill in the parameter structure
        send_cmd.type   = BLE_GATT_NOTIFICATION;
        send_cmd.handle = prf_find_handle_by_idx(GHEALTH_IDX_TX_VAL, s_ghealth_env.start_hdl, (uint8_t *)&s_char_mask);

        // Pack measured value in database
        send_cmd.length = length;
        send_cmd.value  = p_data;

        // Send notification to peer device
        error_code = ble_gatts_noti_ind(conn_idx, &send_cmd);
    }

    return error_code;
}

sdk_err_t ghealth_rx_flow_ctrl_set(uint8_t conn_idx, uint8_t flow_ctrl)
{
    sdk_err_t        error_code = BLE_SUCCESS;
    gatts_noti_ind_t send_cmd;

    if (PRF_CLI_START_NTF == s_ghealth_env.flow_ctrl_ntf_cfg[conn_idx])
    {
        // Fill in the parameter structure
        send_cmd.type   = BLE_GATT_NOTIFICATION;
        send_cmd.handle = prf_find_handle_by_idx(GHEALTH_IDX_FLOW_CTRL_VAL, s_ghealth_env.start_hdl, (uint8_t *)&s_char_mask);

        // Pack measured value in database
        send_cmd.length = 1;
        send_cmd.value  = &flow_ctrl;

        // Send notification to peer device
        error_code      = ble_gatts_noti_ind(conn_idx, &send_cmd);
    }

    return error_code;
}

sdk_err_t ghealth_service_init(ghealth_init_t *p_ghealth_init)
{
    if (NULL == p_ghealth_init)
    {
        return SDK_ERR_POINTER_NULL;
    }

    memcpy(&s_ghealth_env.ghealth_init, p_ghealth_init, sizeof(ghealth_init_t));

    return ble_server_prf_add(&ghealth_prf_info);
}
