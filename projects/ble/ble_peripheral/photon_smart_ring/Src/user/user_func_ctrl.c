#include "user_func_ctrl.h"

#include "app_log.h"
#include "gh3x2x_demo.h"

#include "user_common.h"
#include "user_rtc.h"
#include "user_timer.h"

static FuncSwitch s_func_switch = kFuncSwitchNull;
static FuncSwitch s_adt_switch = kFuncSwitchNull;

static WearingStatus s_wearing_status = kWearingStatusNull;

static FuncStatus s_status_init = kFuncStatusOff;
static FuncStatus s_status_adt  = kFuncStatusOff;
static FuncStatus s_status_hr   = kFuncStatusOff;
static FuncStatus s_status_hrv  = kFuncStatusOff;
static FuncStatus s_status_spo2 = kFuncStatusOff;
static FuncStatus s_status_rr   = kFuncStatusOff;

uint16_t func_ctrl_init() {
    s_func_switch    = kFuncSwitchOff;
    s_adt_switch     = kFuncSwitchOff;
    s_wearing_status = kWearingStatusOff;
    user_timer_init(); // 定时器初始化
    rtc_set_tick_alarm(DETECTION_INTERVAL); // RTC中断间隔初始化

    APP_LOG_INFO("func_ctrl_init success!\n");
    APP_LOG_INFO("DETECTION_INTERVAL is %us!\n",
                 (DETECTION_INTERVAL / SEC_TO_MS));

    return GUNTER_SUCCESS;
}

uint16_t func_ctrl_deinit() {
    s_func_switch = kFuncSwitchNull;
    return GUNTER_SUCCESS;
}

// s_func_switch access function
uint16_t func_ctrl_get_func_switch() {
    return s_func_switch;
}

uint16_t func_ctrl_set_func_switch(FuncSwitch func_switch) {
    s_func_switch = func_switch;
    return GUNTER_SUCCESS;
}

// s_adt_switch access function
uint16_t func_ctrl_set_adt_switch(FuncSwitch func_switch) {
    s_adt_switch = func_switch;
    return GUNTER_SUCCESS;
}

// s_wearing_status access function
uint16_t func_ctrl_get_wearing_status() {
    return s_wearing_status;
}

uint16_t func_ctrl_set_wearing_status(WearingStatus wearing_status) {
    s_wearing_status = wearing_status;
    return GUNTER_SUCCESS;
}

void func_ctrl_start(FuncOption func_option) {
    // user_timer_start(func_option);

    switch (func_option) {
        case kFuncOptInitDev:
            if (s_status_init == kFuncStatusOff) {
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
                user_timer_start(func_option);
                s_status_init = kFuncStatusOn;
            }
            break;

        case kFuncOptAdt:
            if (s_status_adt == kFuncStatusOff) {
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
                user_timer_start(func_option);
                s_status_adt = kFuncStatusOn;
            }
            break;

        case kFuncOptHr:
            if (s_status_hr == kFuncStatusOff) {
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
                user_timer_start(func_option);
                s_status_hr = kFuncStatusOn;
            }
            break;

        case kFuncOptHrv:
            if (s_status_hrv == kFuncStatusOff) {
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HRV);
                user_timer_start(func_option);
                s_status_hrv = kFuncStatusOn;
            }
            break;

        case kFuncOptSpo2:
            if (s_status_spo2 == kFuncStatusOff) {
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SPO2);
                user_timer_start(func_option);
                s_status_spo2 = kFuncStatusOn;
            }
            break;

        case kFuncOptRr:
            if (s_status_rr == kFuncStatusOff) {
                // TODO: Respiratory rate

                user_timer_start(func_option);
                s_status_rr = kFuncStatusOn;
            }
            break;

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}

void func_ctrl_stop(FuncOption func_option) {
    // user_timer_stop(func_option);

    switch (func_option) {
        case kFuncOptAdt:
            if (s_status_adt == kFuncStatusOn) {
                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
                s_status_adt = kFuncStatusOff;
            }
            break;

        case kFuncOptHr:
            if (s_status_hr == kFuncStatusOn) {
                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HR);
                s_status_hr = kFuncStatusOff;
            }
            break;

        case kFuncOptHrv:
            if (s_status_hrv == kFuncStatusOn) {
                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HRV);
                s_status_hrv = kFuncStatusOff;
            }
            break;

        case kFuncOptSpo2:
            if (s_status_spo2 == kFuncStatusOn) {
                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2);
                s_status_spo2 = kFuncStatusOff;
            }
            break;

        case kFuncOptRr:
            if (s_status_rr == kFuncStatusOn) {
                user_timer_stop(func_option);

                // TODO: Respiratory rate
                s_status_rr = kFuncStatusOff;
            }
            break;

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}

void func_ctrl_handler() {
    if (s_func_switch == kFuncSwitchOn) {
        // if (s_wearing_status == kWearingStatusOff) {
            // func_ctrl_start(kFuncOptAdt);
            s_adt_switch = kFuncSwitchOn;
            s_func_switch = kFuncSwitchOff;
        // }
    }

    switch (s_adt_switch) {
        case kFuncSwitchOn:
            func_ctrl_start(kFuncOptAdt);
            break;
        
        case kFuncSwitchOff:
            func_ctrl_stop(kFuncOptAdt);
            break;

        default:
            break;
    }

    switch (s_wearing_status) {
        case kWearingStatusOn:
            func_ctrl_start(kFuncOptHr);
            func_ctrl_start(kFuncOptHrv);
            func_ctrl_start(kFuncOptSpo2);
            break;

        case kWearingStatusOff:
            func_ctrl_stop(kFuncOptHr);
            func_ctrl_stop(kFuncOptHrv);
            func_ctrl_stop(kFuncOptSpo2);
            break;

        default:
            break;
    }
}
