#ifndef USER_BLE_H
#define USER_BLE_H

#include "gunter_ble_ctrl.h"
#include "health.h"

#define GBC_CMD_TYPE_SIZE 1
#define GBC_RSP_TYPE_SIZE 1

typedef enum HEALTH_REQ_TYPE {
    HEALTH_REQ_RESERVED = 0,
    HEALTH_REQ_OPEN_HR,
    HEALTH_REQ_CLOSE_HR,
    HEALTH_REQ_OPEN_HRV,
    HEALTH_REQ_CLOSE_HRV,
    HEALTH_REQ_OPEN_SPO2,
    HEALTH_REQ_CLOSE_SPO2,

} HEALTH_REQ_TYPE;

typedef enum HEALTH_RSP_TYPE {
    HEALTH_RSP_RESERVED = 0,
    HEALTH_RSP_UNKNOWN_CMD,

} HEALTH_RSP_TYPE;

// types of requests
typedef enum GBC_REQ_TYPE {
    GBC_REQ_RESERVED = 0,
    GBC_REQ_ERASE_FLASH,
    GBC_REQ_SEND_DATA,

} GBC_REQ_TYPE;

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
