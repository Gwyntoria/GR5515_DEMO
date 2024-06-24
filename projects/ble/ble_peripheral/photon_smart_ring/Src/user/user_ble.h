#ifndef USER_BLE_H
#define USER_BLE_H

#include "gunter_ble_ctrl.h"
#include "health.h"

#define GBC_CMD_TYPE_SIZE 1
#define GBC_RSP_TYPE_SIZE 1

/**
 * @defgroup health service request
 * 
 * 0x01~0x0F
 */
#define HEALTH_REQ_OPEN_HR    0x01
#define HEALTH_REQ_CLOSE_HR   0x02
#define HEALTH_REQ_OPEN_HRV   0x03
#define HEALTH_REQ_CLOSE_HRV  0x04
#define HEALTH_REQ_OPEN_SPO2  0x05
#define HEALTH_REQ_CLOSE_SPO2 0x06

/**
 * @defgroup health service response
 * 
 * 0x10~0x1F
 */
#define HEALTH_RSP_UNKNOWN_CMD 0x10

/**
 * @defgroup gbc service request
 * 
 * 0x21~0x2F
 */
#define GBC_REQ_ERASE_FLASH 0x21
#define GBC_REQ_SEND_DATA   0x22

/**
 * @defgroup gbc service response
 * 
 * 0x30~0x3F
 */
#define GBC_RSP_UNKNOWN_CMD 0x30
#define GBC_RSP_FLASH_EMPTY 0x31
#define GBC_RSP_FLASH_ERROR 0x32

typedef enum ble_notify_status {
    BLE_NOTIFY_CLOSED = 0,
    BLE_NOTIFY_OPENED = 1,
} ble_notify_status;

typedef enum ble_notify_type {
    BLE_NOTIFY_HEALTH_CMD  = 0,
    BLE_NOTIFY_HEALTH_HR   = 1,
    BLE_NOTIFY_HEALTH_HRV  = 2,
    BLE_NOTIFY_HEALTH_SPO2 = 3,
    BLE_NOTIFY_GBC_CMD     = 4,
    BLE_NOTIFY_GBC_DATA    = 5,
} ble_notify_type;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

bool is_ble_notify_opened(ble_notify_type type);

void health_service_process_event(health_evt_t* p_evt);
void gbc_service_process_event(gbc_evt_t* p_evt);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
