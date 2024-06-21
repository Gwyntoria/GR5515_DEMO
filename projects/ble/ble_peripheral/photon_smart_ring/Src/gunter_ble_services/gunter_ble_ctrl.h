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

#define GBC_CONNECTION_MAX (10 < CFG_MAX_CONNECTIONS ? 10 : CFG_MAX_CONNECTIONS)

#define GBC_MAX_DATA_LEN (247)
#define GBC_MAX_CMD_LEN  (20)

typedef enum gbc_evt_type_t {
    GBC_EVT_INVALID,

    GBC_EVT_CMD_RECEIVED,
    GBC_EVT_CMD_NOTIFIED,

    // GBC_EVT_DATA_PORT_OPENED,
    // GBC_EVT_DATA_PORT_CLOSED,
    GBC_EVT_DATA_NOTIFIED,

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

sdk_err_t gbc_notify_cmd(uint8_t conn_idx, uint8_t* p_data, uint16_t length);
sdk_err_t gbc_notify_data(uint8_t conn_idx, uint8_t* p_data, uint16_t length);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* GUNTER_BLE_CTRL_H */
