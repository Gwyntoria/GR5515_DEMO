#ifndef USER_BLE_H
#define USER_BLE_H

#include "gunter_ble_ctrl.h"
#include "health.h"

#define GBC_CMD_TYPE_SIZE 1
#define GBC_RSP_TYPE_SIZE 1

/**
 * @defgroup health service request
 */
#define HEALTH_REQ_OPEN_HR    0x01
#define HEALTH_REQ_CLOSE_HR   0x02
#define HEALTH_REQ_OPEN_HRV   0x03
#define HEALTH_REQ_CLOSE_HRV  0x04
#define HEALTH_REQ_OPEN_SPO2  0x05
#define HEALTH_REQ_CLOSE_SPO2 0x06

/**
 * @defgroup health service response
 */
#define HEALTH_RSP_UNKNOWN_CMD 0xFF

/**
 * @defgroup gbc service request
 */
#define GBC_REQ_ERASE_FLASH 0x01
#define GBC_REQ_SEND_DATA   0x02

/**
 * @defgroup gbc service response
 */
#define GBC_RSP_FLASH_EMPTY 0x51
#define GBC_RSP_FLASH_ERROR 0x52
#define GBC_RSP_UNKNOWN_CMD 0xFF

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
