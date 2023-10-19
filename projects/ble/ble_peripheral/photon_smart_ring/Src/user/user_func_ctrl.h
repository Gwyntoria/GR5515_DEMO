#ifndef USER_FUNC_CTRL_H
#define USER_FUNC_CTRL_H

#include "user_common.h"

typedef enum FuncOption {
    kFuncOptionHr = 0,
    kFuncOptionHrv,
    kFuncOptionSpo2,
    kFuncOptionRr,
    kFuncOptionAdt,
    kFuncOptionNull
} FuncOption;

typedef enum FuncSwitch {
    kFuncSwitchOff = 0,
    kFuncSwitchOn,
    kFuncSwitchNull
} FuncSwitch;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
