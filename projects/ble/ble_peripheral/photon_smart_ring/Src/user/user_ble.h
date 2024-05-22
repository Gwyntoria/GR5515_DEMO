#ifndef USER_BLE_H
#define USER_BLE_H

#include "gunter_ble_ctrl.h"
#include "health.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


void health_service_process_event(health_evt_t* p_evt);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
