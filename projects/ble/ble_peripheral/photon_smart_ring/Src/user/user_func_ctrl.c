#include "user_func_ctrl.h"

#include "app_log.h"
#include "gh3x2x_demo.h"

#include "user_rtc.h"
#include "user_common.h"

static FuncOption gs_func_option = kFuncOptionNull;
static FuncSwitch gs_func_switch = kFuncSwitchNull;

FuncOption func_ctrl_get_option()
{
    return gs_func_option;
}

void func_ctrl_set_option(FuncOption func_option)
{
    gs_func_option = func_option;
}

uint16_t func_ctrl_init() {}

uint16_t func_ctrl_deinit() {}

void func_ctrl_start_sampling(FuncOption func_option) {
    switch (func_option) {
        case kFuncOptionHr:{
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
            break;
        }

        case kFuncOptionHrv:{
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HRV);
            break;
        }

        case kFuncOptionSpo2:{
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SPO2);
            break;
        }

        case kFuncOptionRr:{
            //TODO Respiratory rate
            break;
        }

        case kFuncOptionAdt:{
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
            break;
        }

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}

uint16_t func_ctrl_stop_sampling() {}

uint16_t func_ctrl_change_option() {
    if (gs_func_switch != kFuncSwitchOn)
        return PHOTON_SUCCESS;




    gs_func_switch = kFuncSwitchOff;
    return PHOTON_SUCCESS;
}
