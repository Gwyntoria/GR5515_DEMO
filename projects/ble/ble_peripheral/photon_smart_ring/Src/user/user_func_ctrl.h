#ifndef USER_FUNC_CTRL_H
#define USER_FUNC_CTRL_H

#include "user_common.h"

#define BASE_OPTION (0x01)

typedef enum FuncOption {
    kFuncOptAdt  = BASE_OPTION << 0,
    kFuncOptHr   = BASE_OPTION << 1,
    kFuncOptSpo2 = BASE_OPTION << 2,
    kFuncOptHrv  = BASE_OPTION << 3,
    kFuncOptRr   = BASE_OPTION << 4,
    kFuncOptNull
} FuncOption;

typedef enum FuncSwitch {
    kFuncSwitchOff = 0,
    kFuncSwitchOn  = 1,
    kFuncSwitchNull
} FuncSwitch;

#ifdef __cplusplus
extern "C" {
#endif

FuncOption func_ctrl_get_option();
void       func_ctrl_set_option(FuncOption func_option);

uint16_t func_ctrl_init();
uint16_t func_ctrl_deinit();

void func_ctrl_start_sampling(FuncOption func_option);
void func_ctrl_stop_sampling(FuncOption func_option);

uint16_t func_ctrl_change_option();

#ifdef __cplusplus
}
#endif

#endif
