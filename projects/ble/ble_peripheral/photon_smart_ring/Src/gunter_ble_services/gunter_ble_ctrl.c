/**
 * @file gunter_ble_ctrl.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "gunter_ble_ctrl.h"

#include "ble_prf_types.h"
#include "ble_prf_utils.h"
#include "utility.h"

#define GBC_CHAR_SETTING_UUID \
    { 0xFB, 0x34, 0x9B, 0x5f, 0x40, 0x90, 0x11, 0x10, 0x20, 0x24, 0x01, 0x10, 0x0E, 0xA0, 0x01, 0x02}
#define GBC_CHAR_DATA_UUID \
    { 0xFB, 0x34, 0x9B, 0x5f, 0x40, 0x90, 0x11, 0x10, 0x20, 0x24, 0x01, 0x10, 0x0E, 0xA0, 0x01, 0x03 }

/**
 * @defgroup Macros for conversion of 128bit to 16bit UUID.
 */
#define ATT_128_PRIMARY_SERVICE BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_PRIMARY_SERVICE)
#define ATT_128_CHARACTERISTIC  BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_CHARACTERISTIC)
#define ATT_128_CLIENT_CHAR_CFG BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DESC_CLIENT_CHAR_CFG)

enum gbc_attr_idx_t {
    GBC_IDX_SVC,

    GBC_IDX_SETTINGS_CHAR,
    GBC_IDX_SETTINGS_VAL,
    GBC_IDX_SETTING_CFG,

    GBC_IDX_DATA_CHAR,
    GBC_IDX_DATA_VAL,
    GBC_IDX_DATA_CFG,

    GBC_IDX_MAX,
};

typedef struct gbc_env_t {
    gbc_init_t gbc_init;                         /**< Sample Service initialization variables. */
    uint16_t   start_hdl;                        /**< Service start handle. */
    uint16_t   setting_ntf_cfg[GBC_CONNECTION_MAX]; /**< Init Character Notification configuration of peer devices. */
    uint16_t   data_ntf_cfg[GBC_CONNECTION_MAX]; /**< Data Character Notification configuration of peer devices. */
} gbc_env_t;

static sdk_err_t gbc_init(void);
static void      gbc_write_att_cb(uint8_t conn_idx, const gatts_write_req_cb_t* p_param);
static void      gbc_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t* p_param);
static void      gbc_cccd_set_cb(uint8_t conn_idx, uint16_t handle, uint16_t cccd_value);
static void      gbc_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t* p_ntf_ind);

static gbc_env_t      s_gbc_env;
static gbc_evt_type_t s_now_notify_cmp_type = GBC_EVT_DATA_NOTIFICATION_COMPLETE;
static const uint16_t s_gbc_features        = 0xFFFF;

static const attm_desc_128_t gbc_att_db[GBC_IDX_MAX] = {
    [GBC_IDX_SVC] = {
        ATT_128_PRIMARY_SERVICE,
        READ_PERM_UNSEC,
        0,
        0,
    },

    // * @brief Settings Characteristic Declaration
    [GBC_IDX_SETTINGS_CHAR] = {
        ATT_128_CHARACTERISTIC,
        READ_PERM_UNSEC,
        0,
        0,
    },
    [GBC_IDX_SETTINGS_VAL] = {
        GBC_CHAR_SETTING_UUID,
        (WRITE_CMD_PERM_UNSEC | NOTIFY_PERM_UNSEC),
        (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
        GBC_CHAR_SETTINGS_VALUE_LEN,
    },
    [GBC_IDX_SETTING_CFG] = {
        ATT_128_CLIENT_CHAR_CFG,
        (READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC | WRITE_CMD_PERM_UNSEC),
        0,
        0,
    },

    // * @brief Data Characteristic Declaration
    [GBC_IDX_DATA_CHAR] = {
        ATT_128_CHARACTERISTIC,
        READ_PERM_UNSEC,
        0,
        0,
    },
    [GBC_IDX_DATA_VAL] = {
        GBC_CHAR_DATA_UUID,
        NOTIFY_PERM_UNSEC,
        (ATT_VAL_LOC_USER | ATT_UUID_TYPE_SET(UUID_TYPE_128)),
        GBC_MAX_DATA_LEN,
    },
    [GBC_IDX_DATA_CFG] = {
        ATT_128_CLIENT_CHAR_CFG,
        (READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC),
        0,
        0,
    },
};

static ble_prf_manager_cbs_t gbc_tack_cbs = {
    (prf_init_func_t)gbc_init,
    NULL,
    NULL,
};

static gatts_prf_cbs_t gbc_cb_func = {
    gbc_read_att_cb,
    gbc_write_att_cb,
    NULL,
    gbc_ntf_ind_cb,
    gbc_cccd_set_cb,
};

static const prf_server_info_t gbc_prf_info = {
    .max_connection_nb = GBC_CONNECTION_MAX,
    .manager_cbs       = &gbc_tack_cbs,
    .gatts_prf_cbs     = &gbc_cb_func,
};

static sdk_err_t gbc_init(void) {
    const uint8_t     gbc_svc_uuid[] = {GBC_SERVICE_UUID};
    sdk_err_t         error_code     = SDK_SUCCESS;
    uint16_t          start_hdl      = 0;
    gatts_create_db_t gatts_db;

    memset(&gatts_db, 0, sizeof(gatts_db));

    gatts_db.shdl                  = &start_hdl;
    gatts_db.uuid                  = gbc_svc_uuid;
    gatts_db.attr_tab_cfg          = (uint8_t*)&s_gbc_features;
    gatts_db.max_nb_attr           = GBC_IDX_MAX;
    gatts_db.srvc_perm             = SRVC_UUID_TYPE_SET(UUID_TYPE_128);
    gatts_db.attr_tab_type         = SERVICE_TABLE_TYPE_128;
    gatts_db.attr_tab.attr_tab_128 = gbc_att_db;

    error_code = ble_gatts_srvc_db_create(&gatts_db);

    if (SDK_SUCCESS == error_code) {
        s_gbc_env.start_hdl = *(gatts_db.shdl);
    } else {
        return error_code;
    }

    return error_code;
}

/**
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] conn_idx: Connection index
 * @param[in] p_param:  Pointer to the parameters of the read request.
 */
static void gbc_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t* p_param) {
    uint8_t          handle    = p_param->handle;
    uint8_t          tab_index = 0;
    uint8_t          i         = 0;
    gatts_read_cfm_t cfm;

    tab_index = prf_find_idx_by_handle(handle, s_gbc_env.start_hdl, GBC_IDX_MAX, (uint8_t*)&s_gbc_features);

    cfm.handle = handle;
    cfm.status = BLE_SUCCESS;

    switch (tab_index) {
        case GBC_IDX_DATA_CFG:
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t*)&s_gbc_env.data_ntf_cfg[conn_idx];
            break;

        default:
            break;
    }

    ble_gatts_read_cfm(conn_idx, &cfm);
}

/**
 * @brief Handles reception of the write request.
 *
 * @param[in] conn_idx: Connection index
 * @param[in] p_param:  Pointer to the parameters of the write request.
 */
static void gbc_write_att_cb(uint8_t conn_idx, const gatts_write_req_cb_t* p_param) {
    uint8_t           handle     = p_param->handle;
    uint8_t           tab_index  = 0;
    uint16_t          cccd_value = 0;
    gbc_evt_t         event;
    gatts_write_cfm_t cfm;

    cfm.handle = handle;
    cfm.status = BLE_SUCCESS;

    tab_index = prf_find_idx_by_handle(handle, s_gbc_env.start_hdl, GBC_IDX_MAX, (uint8_t*)&s_gbc_features);

    event.conn_idx = conn_idx;
    event.evt_type = GBC_EVT_INVALID;

    switch (tab_index) {
        case GBC_IDX_SETTINGS_VAL:
            event.evt_type = THS_EVT_SETTINGS_CHANGED;
            event.length   = p_param->length;
            event.p_data   = (uint8_t*)p_param->value;
            break;

        case GBC_IDX_SETTING_CFG:
            cccd_value = le16toh(&p_param->value[0]);
            s_gbc_env.setting_ntf_cfg[conn_idx] = cccd_value;
            break;

        case GBC_IDX_DATA_CFG:
            cccd_value = le16toh(&p_param->value[0]);
            event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? GBC_EVT_DATA_NOTIFICATION_ENABLED : GBC_EVT_DATA_NOTIFICATION_DISABLED;
            s_gbc_env.data_ntf_cfg[conn_idx] = cccd_value;
            break;

        default:
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }
    if ((BLE_ATT_ERR_INVALID_HANDLE != cfm.status) && (GBC_EVT_INVALID != event.evt_type) && s_gbc_env.gbc_init.evt_handler) {
        s_gbc_env.gbc_init.evt_handler(&event);
    }

    ble_gatts_write_cfm(conn_idx, &cfm);
}

/**
 *****************************************************************************************
 * @brief Handles reception of the cccd recover request.
 *
 * @param[in] conn_idx:   Connection index
 * @param[in] handle:     The handle of cccd attribute.
 * @param[in] cccd_value: The value of cccd attribute.
 *****************************************************************************************
 */
static void gbc_cccd_set_cb(uint8_t conn_idx, uint16_t handle, uint16_t cccd_value) {
    uint8_t   tab_index = 0;
    gbc_evt_t event;

    if (!prf_is_cccd_value_valid(cccd_value)) {
        return;
    }

    event.conn_idx = conn_idx;
    event.evt_type = GBC_EVT_INVALID;

    tab_index = prf_find_idx_by_handle(handle, s_gbc_env.start_hdl, GBC_IDX_MAX, (uint8_t*)&s_gbc_features);

    switch (tab_index) {
       case GBC_IDX_SETTING_CFG:
            s_gbc_env.setting_ntf_cfg[conn_idx] = cccd_value;
            break;

        case GBC_IDX_DATA_CFG:
            event.evt_type = (PRF_CLI_START_NTF == cccd_value) ? GBC_EVT_DATA_NOTIFICATION_ENABLED : GBC_EVT_DATA_NOTIFICATION_DISABLED;
            s_gbc_env.data_ntf_cfg[conn_idx] = cccd_value;
            break;

        default:
            break;
    }
    if ((GBC_EVT_INVALID != event.evt_type) && (s_gbc_env.gbc_init.evt_handler)) {
        s_gbc_env.gbc_init.evt_handler(&event);
    }
}

/**
 * @brief Callback function for handling notifications and indications.
 *
 * This function is called when a notification or indication is received from the BLE stack.
 * It checks if there is a valid event handler registered and if the status is BLE_SUCCESS.
 * If the received message is a notification, it creates an event structure and calls the event handler.
 *
 * @param conn_idx The connection index.
 * @param status The status of the notification or indication.
 * @param p_ntf_ind Pointer to the notification or indication data.
 */
static void gbc_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t* p_ntf_ind) {
    if (s_gbc_env.gbc_init.evt_handler != NULL) {
        gbc_evt_t event;
        event.conn_idx = conn_idx;

        if (status == BLE_SUCCESS) {
            if (BLE_GATT_NOTIFICATION == p_ntf_ind->type) {
                event.evt_type = s_now_notify_cmp_type;
                s_gbc_env.gbc_init.evt_handler(&event);
            }
        }
    }
}

sdk_err_t gbc_setting_notify(uint8_t conn_idx, uint8_t* p_data, uint16_t length) {
    sdk_err_t        error_code = SDK_ERR_NTF_DISABLED;
    gatts_noti_ind_t send_cmd;

    if (PRF_CLI_START_NTF == s_gbc_env.setting_ntf_cfg[conn_idx]) {
            // Fill in the parameter structure
            send_cmd.type = BLE_GATT_NOTIFICATION;
            send_cmd.handle = prf_find_handle_by_idx(GBC_IDX_SETTINGS_VAL, s_gbc_env.start_hdl, (uint8_t*)&s_gbc_features);
            // pack measured value in database
            send_cmd.length = length;
            send_cmd.value = p_data;
            // send notification to peer device
            error_code = ble_gatts_noti_ind(conn_idx, &send_cmd);
    }

    return error_code;
}

sdk_err_t gbc_data_notify(uint8_t conn_idx, uint8_t* p_data, uint16_t length) {
    sdk_err_t        error_code = SDK_ERR_NTF_DISABLED;
    gatts_noti_ind_t send_cmd;

    if (PRF_CLI_START_NTF == s_gbc_env.data_ntf_cfg[conn_idx]) {
            // Fill in the parameter structure
            send_cmd.type = BLE_GATT_NOTIFICATION;
            send_cmd.handle = prf_find_handle_by_idx(GBC_IDX_DATA_VAL, s_gbc_env.start_hdl, (uint8_t*)&s_gbc_features);
            // pack measured value in database
            send_cmd.length = length;
            send_cmd.value = p_data;
            s_now_notify_cmp_type = GBC_EVT_DATA_NOTIFICATION_COMPLETE;
            // send notification to peer device
            error_code = ble_gatts_noti_ind(conn_idx, &send_cmd);
    }

    return error_code;
}

sdk_err_t gbc_service_init(gbc_init_t* gbc_init) {
    if (NULL == gbc_init) {
        return SDK_ERR_POINTER_NULL;
    }

    memcpy(&s_gbc_env.gbc_init, gbc_init, sizeof(gbc_init_t));

    return ble_server_prf_add(&gbc_prf_info);
}
