#ifndef USER_FUNC_CTRL_H
#define USER_FUNC_CTRL_H

#include "user_common.h"

#define DETECTION_INTERVAL      (SEC_TO_MS * 60)
#define DETECTION_CONTIOUS_TIME (SEC_TO_MS * 30)

#define BASE_OPTION (0x01)

typedef enum FuncOption {
    kFuncOptInitDev = BASE_OPTION << 0,
    kFuncOptAdt     = BASE_OPTION << 1,
    kFuncOptHr      = BASE_OPTION << 2,
    kFuncOptHrv     = BASE_OPTION << 3,
    kFuncOptSpo2    = BASE_OPTION << 4,
    kFuncOptRr      = BASE_OPTION << 5,
    kFuncOptNull
} FuncOption;

typedef enum FuncSwitch {
    kFuncSwitchOff = 0,
    kFuncSwitchOn  = 1,
    kFuncSwitchNull
} FuncSwitch;

typedef enum WearingStatus {
    kWearingStatusOff = 0,
    kWearingStatusOn  = 1,
    kWearingStatusNull
} WearingStatus;

typedef enum FuncStatus {
    kFuncStatusOff = 0,
    kFuncStatusOn  = 1,
    kFuncStatusNull
} FuncStatus;

#ifdef __cplusplus
extern "C" {
#endif

uint16_t func_ctrl_init();
uint16_t func_ctrl_deinit();

uint16_t func_ctrl_get_func_switch();
uint16_t func_ctrl_set_func_switch(FuncSwitch func_switch);

void func_ctrl_start(FuncOption func_option);
void func_ctrl_stop(FuncOption func_option);

void func_ctrl_run();


#ifdef __cplusplus
}
#endif

#endif
