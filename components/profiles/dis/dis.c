/**
 *****************************************************************************************
 *
 * @file dis.h
 *
 * @brief Device Information Service Implementation.
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
#include "dis.h"
#include "ble_prf_types.h"
#include "ble_prf_utils.h"
#include "utility.h"

/*
 * ENUMERATIONS
 *****************************************************************************************
 */
/**@brief Device Information Service Attributes database index list. */
enum dis_attr_idx_t
{
    DIS_IDX_SVC,

    DIS_IDX_SYSTEM_ID_CHAR,
    DIS_IDX_SYSTEM_ID_VAL,

    DIS_IDX_MODEL_NB_STR_CHAR,
    DIS_IDX_MODEL_NB_STR_VAL,

    DIS_IDX_SERIAL_NB_STR_CHAR,
    DIS_IDX_SERIAL_NB_STR_VAL,

    DIS_IDX_HARD_REV_STR_CHAR,
    DIS_IDX_HARD_REV_STR_VAL,

    DIS_IDX_FIRM_REV_STR_CHAR,
    DIS_IDX_FIRM_REV_STR_VAL,

    DIS_IDX_SW_REV_STR_CHAR,
    DIS_IDX_SW_REV_STR_VAL,

    DIS_IDX_MANUFACTURER_NAME_CHAR,
    DIS_IDX_MANUFACTURER_NAME_VAL,

    DIS_IDX_IEEE_CHAR,
    DIS_IDX_IEEE_VAL,

    DIS_IDX_PNP_ID_CHAR,
    DIS_IDX_PNP_ID_VAL,

    DIS_IDX_NB,
};

/*
 * STRUCTURES
 *****************************************************************************************
 */
/**@brief Device Information Service environment variable. */
struct dis_env_t
{
    dis_init_t dis_init;       /**< Device Information Service initialization variables. */
    uint16_t   start_hdl;      /**< Device Information Service start handle. */
};

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static sdk_err_t   dis_init(void);
static void        dis_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t *p_param);

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static struct dis_env_t s_dis_env;

/**@brief Full DIS Database Description which is used to add attributes into the ATT database. */
static const attm_desc_t dis_attr_tab[DIS_IDX_NB] =
{
    // Device Information Service Declaration
    [DIS_IDX_SVC] = {BLE_ATT_DECL_PRIMARY_SERVICE, READ_PERM_UNSEC, 0, 0},

    // System ID Characteristic Declaration
    [DIS_IDX_SYSTEM_ID_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // System ID Characteristic Value
    [DIS_IDX_SYSTEM_ID_VAL]  = {BLE_ATT_CHAR_SYS_ID, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_SYS_ID_LEN},

    // Model Number String Characteristic Declaration
    [DIS_IDX_MODEL_NB_STR_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Model Number String Characteristic Value
    [DIS_IDX_MODEL_NB_STR_VAL]  = {BLE_ATT_CHAR_MODEL_NB, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_VAL_MAX_LEN},

    // Serial Number String Characteristic Declaration
    [DIS_IDX_SERIAL_NB_STR_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Serial Number String Characteristic Value
    [DIS_IDX_SERIAL_NB_STR_VAL]  = {BLE_ATT_CHAR_SERIAL_NB, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_VAL_MAX_LEN},

    // Firmware Revision String Characteristic Declaration
    [DIS_IDX_FIRM_REV_STR_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Firmware Revision String Characteristic Value
    [DIS_IDX_FIRM_REV_STR_VAL]  = {BLE_ATT_CHAR_FW_REV, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_VAL_MAX_LEN},

    // Hardware Revision String Characteristic Declaration
    [DIS_IDX_HARD_REV_STR_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Hardware Revision String Characteristic Value
    [DIS_IDX_HARD_REV_STR_VAL]  = {BLE_ATT_CHAR_HW_REV, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_VAL_MAX_LEN},

    // Software Revision String Characteristic Declaration
    [DIS_IDX_SW_REV_STR_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Software Revision String Characteristic Value
    [DIS_IDX_SW_REV_STR_VAL]  = {BLE_ATT_CHAR_SW_REV, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_VAL_MAX_LEN},

    // Manufacturer Name Characteristic Declaration
    [DIS_IDX_MANUFACTURER_NAME_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Manufacturer Name Characteristic Value
    [DIS_IDX_MANUFACTURER_NAME_VAL]  = {BLE_ATT_CHAR_MANUF_NAME, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_VAL_MAX_LEN},

    // IEEE 11073-20601 Regulatory Certification Data List Characteristic Declaration
    [DIS_IDX_IEEE_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // IEEE 11073-20601 Regulatory Certification Data List Characteristic Value
    [DIS_IDX_IEEE_VAL]  = {BLE_ATT_CHAR_IEEE_CERTIF, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_SYS_ID_LEN},

    // PnP ID Characteristic Declaration
    [DIS_IDX_PNP_ID_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // PnP ID Characteristic Value
    [DIS_IDX_PNP_ID_VAL]  = {BLE_ATT_CHAR_PNP_ID, READ_PERM_UNSEC, ATT_VAL_LOC_USER, DIS_PNP_ID_LEN},
};

/**@brief Device Information Service interface required by profile manager. */
static ble_prf_manager_cbs_t dis_mgr_cbs =
{
    (prf_init_func_t)dis_init,
    NULL,
    NULL
};

/**@brief Device Information GATT Server Callbacks. */
static gatts_prf_cbs_t dis_gatts_cbs =
{
    dis_read_att_cb,
    NULL,
    NULL,
    NULL
};

/**@brief Device Information Service Information. */
static const prf_server_info_t dis_prf_info =
{
    .max_connection_nb = DIS_CONNECTION_MAX,
    .manager_cbs       = &dis_mgr_cbs,
    .gatts_prf_cbs     = &dis_gatts_cbs
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Initialize device information service and create DB in ATT.
 *
 * @return Error code to know if service initialization succeed or not.
 *****************************************************************************************
 */
static sdk_err_t dis_init(void)
{
    // The start hanlde must be set with PRF_INVALID_HANDLE to be allocated automatically by BLE Stack.
    uint16_t          start_hdl      = PRF_INVALID_HANDLE;
    const uint8_t     dis_svc_uuid[] = BLE_ATT_16_TO_16_ARRAY(BLE_ATT_SVC_DEVICE_INFO);
    sdk_err_t         error_code     = SDK_SUCCESS;
    gatts_create_db_t gatts_db;

    memset(&gatts_db, 0, sizeof(gatts_db));

    gatts_db.shdl                 = &start_hdl;
    gatts_db.uuid                 = dis_svc_uuid;
    gatts_db.attr_tab_cfg         = (uint8_t *)&s_dis_env.dis_init.char_mask;
    gatts_db.max_nb_attr          = DIS_IDX_NB;
    gatts_db.srvc_perm            = 0;
    gatts_db.attr_tab_type        = SERVICE_TABLE_TYPE_16;
    gatts_db.attr_tab.attr_tab_16 = dis_attr_tab;

    error_code = ble_gatts_srvc_db_create(&gatts_db);

    if (SDK_SUCCESS == error_code)
    {
        s_dis_env.start_hdl = *gatts_db.shdl;
    }

    return error_code;
}

/**
 *****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] conn_idx: Connection index
 * @param[in] p_param:  Pointer to the parameters of the read request.
 *****************************************************************************************
 */
static void dis_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t *p_param)
{
    gatts_read_cfm_t cfm;
    uint16_t         handle    = p_param->handle;
    uint8_t          tab_index = prf_find_idx_by_handle(handle,
                                 s_dis_env.start_hdl,
                                 DIS_IDX_NB,
                                 (uint8_t *)&s_dis_env.dis_init.char_mask);
    cfm.handle = handle;
    cfm.status = BLE_SUCCESS;

    uint8_t buf[DIS_PNP_ID_LEN];

    switch (tab_index)
    {
        case DIS_IDX_SYSTEM_ID_VAL:
            cfm.length = DIS_SYS_ID_LEN;
            cfm.value  = (uint8_t *)s_dis_env.dis_init.p_sys_id;
            break;

        case DIS_IDX_MODEL_NB_STR_VAL:
            cfm.length = s_dis_env.dis_init.model_num_str.length;
            cfm.value  = (uint8_t *)s_dis_env.dis_init.model_num_str.p_str;
            break;

        case DIS_IDX_SERIAL_NB_STR_VAL:
            cfm.length = s_dis_env.dis_init.serial_num_str.length;
            cfm.value  = (uint8_t *)s_dis_env.dis_init.serial_num_str.p_str;
            break;

        case DIS_IDX_FIRM_REV_STR_VAL:
            cfm.length = s_dis_env.dis_init.fw_rev_str.length;
            cfm.value  = (uint8_t *)s_dis_env.dis_init.fw_rev_str.p_str;
            break;

        case DIS_IDX_HARD_REV_STR_VAL:
            cfm.length = s_dis_env.dis_init.hw_rev_str.length;
            cfm.value  = (uint8_t *)s_dis_env.dis_init.hw_rev_str.p_str;
            break;

        case DIS_IDX_SW_REV_STR_VAL:
            cfm.length = s_dis_env.dis_init.sw_rev_str.length;
            cfm.value  = (uint8_t *)s_dis_env.dis_init.sw_rev_str.p_str;
            break;

        case DIS_IDX_MANUFACTURER_NAME_VAL:
            cfm.length = s_dis_env.dis_init.manufact_name_str.length;
            cfm.value  = (uint8_t *)s_dis_env.dis_init.manufact_name_str.p_str;
            break;

        case DIS_IDX_IEEE_VAL:
            cfm.length = s_dis_env.dis_init.reg_cert_data_list.list_len;
            cfm.value  = (uint8_t *)s_dis_env.dis_init.reg_cert_data_list.p_list;
            break;

        case DIS_IDX_PNP_ID_VAL:
            {
                buf[0] = s_dis_env.dis_init.p_pnp_id->vendor_id_source;
                buf[1] = LO_U16(s_dis_env.dis_init.p_pnp_id->vendor_id);
                buf[2] = HI_U16(s_dis_env.dis_init.p_pnp_id->vendor_id);
                buf[3] = LO_U16(s_dis_env.dis_init.p_pnp_id->product_id);
                buf[4] = HI_U16(s_dis_env.dis_init.p_pnp_id->product_id);
                buf[5] = LO_U16(s_dis_env.dis_init.p_pnp_id->product_version);
                buf[6] = HI_U16(s_dis_env.dis_init.p_pnp_id->product_version);
                cfm.length = DIS_PNP_ID_LEN;
                cfm.value  = buf;
            }
            break;

        default:
            cfm.length = 0;
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }

    ble_gatts_read_cfm(conn_idx, &cfm);
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
sdk_err_t dis_service_init(dis_init_t *p_dis_init)
{
    if (NULL == p_dis_init)
    {
        return SDK_ERR_POINTER_NULL;
    }
    memcpy(&s_dis_env.dis_init, p_dis_init, sizeof(dis_init_t));

    return ble_server_prf_add(&dis_prf_info);
}

