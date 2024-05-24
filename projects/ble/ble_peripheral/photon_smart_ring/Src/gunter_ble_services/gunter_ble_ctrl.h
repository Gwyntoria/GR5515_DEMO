/**
 * @file gunter_ble_ctrl.h
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef GUNTER_BLE_CTRL_H
#define GUNTER_BLE_CTRL_H

#include "custom_config.h"
#include "gr55xx_sys.h"
#include <stdbool.h>
#include <stdint.h>

#define GBC_SERVICE_UUID 0x1B, 0xD7, 0x90, 0xEC, 0xE8, 0xB9, 0x75, 0x80, 0x0A, 0x46, 0x44, 0xD3, 0x01, 0x01, 0xED, 0xA6

#define GBC_CONNECTION_MAX (10 < CFG_MAX_CONNECTIONS ? 10 : CFG_MAX_CONNECTIONS)

#define GBC_MAX_DATA_LEN            (247)
#define GBC_CHAR_SETTINGS_VALUE_LEN (20)

typedef enum gbc_evt_type_t {
    GBC_EVT_INVALID,

    GBC_EVT_TX_NOTIFICATION_ENABLED,
    GBC_EVT_TX_NOTIFICATION_DISABLED,
    GBC_EVT_TX_NOTIFY_COMPLETE,

    GBC_EVT_RX_RECEIVE_DATA,

    THS_EVT_SETTINGS_CHANGED,

    GBC_EVT_DATA_INDICATION_ENABLED,
    GBC_EVT_DATA_INDICATION_DISABLED,
    GBC_EVT_DATA_INDICATION_COMPLETE,

} gbc_evt_type_t;

typedef struct gbc_evt_t {
    gbc_evt_type_t evt_type; /**< The gbc service event. */
    uint8_t        conn_idx; /**< The connection index. */
    uint8_t*       p_data;   /**< Pointer to event data. */
    uint16_t       length;   /**< Length of event data. */
} gbc_evt_t;

typedef void (*gbc_evt_handler_t)(gbc_evt_t* p_evt);

typedef struct gbc_init_t {
    gbc_evt_handler_t evt_handler; /**<Service event handler. */
} gbc_init_t;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

sdk_err_t gbc_service_init(gbc_init_t* gbc_init);

sdk_err_t gbc_tx_data_send(uint8_t conn_idx, uint8_t* p_data, uint16_t length);
sdk_err_t gbc_setting_notify(uint8_t conn_idx, uint8_t* p_data, uint16_t length);
sdk_err_t gbc_loc_data_send(uint8_t conn_idx, uint8_t* p_data, uint16_t length);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* GUNTER_BLE_CTRL_H */
