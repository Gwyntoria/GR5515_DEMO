#include "user_func_ctrl.h"

#include "app_log.h"
#include "gh3x2x_demo.h"

#include "user_common.h"
#include "user_rtc.h"

static FuncOption gs_func_option = kFuncOptNull;
static FuncSwitch gs_func_switch = kFuncSwitchNull;

FuncOption func_ctrl_get_option()
{
    return gs_func_option;
}

void func_ctrl_set_option(FuncOption func_option)
{
    gs_func_option = func_option;
}

uint16_t func_ctrl_init()
{
    gs_func_option = kFuncOptAdt;
    rtc_set_tick_alarm(MIN_TO_MS * 5);
}

uint16_t func_ctrl_deinit()
{
    gs_func_option = kFuncOptNull;
    gs_func_switch = kFuncSwitchNull;
}

void func_ctrl_start_sampling(FuncOption func_option)
{
    switch (func_option) {
        case kFuncOptAdt:
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
            break;

        case kFuncOptHr:
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
            break;

        case kFuncOptHrv:
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HRV);
            break;

        case kFuncOptSpo2:
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SPO2);
            break;

        case kFuncOptRr:
            // TODO: Respiratory rate
            break;

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}

void func_ctrl_stop_sampling(FuncOption func_option)
{
    switch (func_option) {
        case kFuncOptAdt:
            Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
            break;

        case kFuncOptHr:
            Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HR);
            break;

        case kFuncOptHrv:
            Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HRV);
            break;

        case kFuncOptSpo2:
            Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2);
            break;

        case kFuncOptRr:
            // TODO: Respiratory rate
            break;

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}

uint16_t func_ctrl_change_option()
{
    if (gs_func_switch != kFuncSwitchOn)
        return PHOTON_SUCCESS;

    gs_func_option <<= 1;

    if (gs_func_option >= kFuncOptNull)
        gs_func_option = kFuncOptHr;

    gs_func_switch = kFuncSwitchOff;
    return PHOTON_SUCCESS;
}
