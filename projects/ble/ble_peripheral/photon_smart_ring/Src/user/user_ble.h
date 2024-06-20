#ifndef USER_BLE_H
#define USER_BLE_H

#include "gunter_ble_ctrl.h"
#include "health.h"

#define GBC_CMD_TYPE_SIZE 1
#define GBC_RSP_TYPE_SIZE 1

// types of requests
typedef enum GBC_CMD_TYPE {
    GBC_CMD_RESERVED = 0,
    GBC_CMD_ERASE_FLASH,
    GBC_CMD_SEND_DATA,

} GBC_CMD_TYPE;

// types of responses
typedef enum GBC_RSP_TYPE {
    GBC_RSP_RESERVED = 0,
    GBC_RSP_FLASH_EMPTY,
    GBC_RSP_FLASH_ERROR,
    GBC_RSP_UNKNOWN_CMD,

} GBC_RSP_TYPE;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


void health_service_process_event(health_evt_t* p_evt);
void gbc_service_process_event(gbc_evt_t* p_evt);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
