#include "user_func_ctrl.h"

#include "app_log.h"
#include "gh3x2x_demo.h"

#include "NST112/nst112x.h"

#include "user_battery.h"
#include "user_common.h"
#include "user_lsm6dso.h"
#include "user_rtc.h"
#include "user_timer.h"

// 功能开关, 默认为NULL，ON时为开启，OFF时为关闭
static FuncSwitch s_switch_func = kFuncSwitchNull; // 功能总开关

static FuncSwitch s_switch_init = kFuncSwitchNull; // 初始化开关
static FuncSwitch s_switch_adt  = kFuncSwitchNull; // 活体检测开关
static FuncSwitch s_switch_hr   = kFuncSwitchNull; // 心率检测开关
static FuncSwitch s_switch_hrv  = kFuncSwitchNull; // 心率变异性检测开关
static FuncSwitch s_switch_spo2 = kFuncSwitchNull; // 血氧检测开关
static FuncSwitch s_switch_rr   = kFuncSwitchNull; // 呼吸率检测开关

static FuncSwitch s_switch_bms = kFuncSwitchNull; // 电池检测开关
static FuncSwitch s_switch_tmp = kFuncSwitchNull; // 温度检测开关
static FuncSwitch s_switch_stp = kFuncSwitchNull; // 步数检测开关
static FuncSwitch s_switch_rst = kFuncSwitchNull; // 重置功能开关
static FuncSwitch s_switch_act = kFuncSwitchNull; // 活动检测开关
static FuncSwitch s_switch_slp = kFuncSwitchNull; // 睡眠检测开关

static FuncResult s_result_adt = kFuncResultNull; // 活体检测结果
static FuncResult s_result_act = kFuncResultNull; // 活动检测结果
static FuncResult s_result_chg = kFuncResultNull; // 充电检测结果
static FuncResult s_result_slp = kFuncResultNull; // 睡眠检测结果

// 功能状态, 默认为NULL，ON时为开启，OFF时为关闭
static FuncStatus s_status_test = kFuncStatusNull;
static FuncStatus s_status_init = kFuncStatusNull;
static FuncStatus s_status_adt  = kFuncStatusNull;
static FuncStatus s_status_hr   = kFuncStatusNull;
static FuncStatus s_status_hrv  = kFuncStatusNull;
static FuncStatus s_status_spo2 = kFuncStatusNull;
static FuncStatus s_status_rr   = kFuncStatusNull;

// switch

void func_ctrl_set_switch_func(FuncSwitch func_switch) {
    s_switch_func = func_switch;
}

void func_ctrl_set_switch_init(FuncSwitch func_switch) {
    s_switch_init = func_switch;
}

void func_ctrl_set_switch_adt(FuncSwitch func_switch) {
    s_switch_adt = func_switch;
}

void func_ctrl_set_switch_hr(FuncSwitch func_switch) {
    s_switch_hr = func_switch;
}

void func_ctrl_set_switch_hrv(FuncSwitch func_switch) {
    s_switch_hrv = func_switch;
}

void func_ctrl_set_switch_spo2(FuncSwitch func_switch) {
    s_switch_spo2 = func_switch;
}

void func_ctrl_set_switch_rr(FuncSwitch func_switch) {
    s_switch_rr = func_switch;
}

void func_ctrl_set_switch_bms(FuncSwitch func_switch) {
    s_switch_bms = func_switch;
}

void func_ctrl_set_switch_tmp(FuncSwitch func_switch) {
    s_switch_tmp = func_switch;
}

void func_ctrl_set_switch_stp(FuncSwitch func_switch) {
    s_switch_stp = func_switch;
}

void func_ctrl_set_switch_rst(FuncSwitch func_switch) {
    s_switch_rst = func_switch;
}

void func_ctrl_set_switch_act(FuncSwitch func_switch) {
    s_switch_act = func_switch;
}

void func_ctrl_set_switch_slp(FuncSwitch func_switch) {
    s_switch_slp = func_switch;
}

// result

void func_ctrl_set_result_adt(FuncResult func_result) {
    s_result_adt = func_result;
}

void func_ctrl_set_result_act(FuncResult func_result) {
    s_result_act = func_result;
}

void func_ctrl_set_result_chg(FuncResult func_result) {
    s_result_chg = func_result;
}

void func_ctrl_set_result_slp(FuncResult func_result) {
    s_result_slp = func_result;
}

uint16_t func_ctrl_init(void) {
    uint16_t ret = GUNTER_SUCCESS;

    s_result_adt = kFuncResultOff;
    s_result_act = kFuncResultOff;
    s_result_chg = kFuncResultOff;
    s_result_slp = kFuncResultOff;

    s_status_test = kFuncStatusOff;
    s_status_init = kFuncStatusOff;
    s_status_adt  = kFuncStatusOff;
    s_status_hr   = kFuncStatusOff;
    s_status_hrv  = kFuncStatusOff;
    s_status_spo2 = kFuncStatusOff;
    s_status_rr   = kFuncStatusOff;

    user_timer_init(); // 定时器初始化

    // RTC间隔中断初始化
    ret = rtc_set_tick_alarm(DETECTION_INTERVAL);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("rtc_set_tick_alarm failed");
        return ret;
    }

    APP_LOG_INFO("DETECTION_INTERVAL is %us!",
                 (DETECTION_INTERVAL / SEC_TO_MS));

    // RTC日期中断初始化
    app_rtc_alarm_t rtc_alarm = {
        .min                  = 2,
        .hour                 = 0,
        .alarm_sel            = CALENDAR_ALARM_SEL_WEEKDAY,
        .alarm_date_week_mask = CALENDAR_ALARM_WEEKDAY_SUN |
                                CALENDAR_ALARM_WEEKDAY_MON |
                                CALENDAR_ALARM_WEEKDAY_TUE |
                                CALENDAR_ALARM_WEEKDAY_WED |
                                CALENDAR_ALARM_WEEKDAY_THU |
                                CALENDAR_ALARM_WEEKDAY_FRI |
                                CALENDAR_ALARM_WEEKDAY_SAT,
    };

    ret = rtc_set_date_alarm(&rtc_alarm);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("rtc_set_date_alarm failed");
        return ret;
    }

    APP_LOG_INFO("func_ctrl_init success!");

    return GUNTER_SUCCESS;
}

uint16_t func_ctrl_deinit(void) {
    s_switch_func = kFuncSwitchNull;
    return GUNTER_SUCCESS;
}

// start & stop

void func_ctrl_start(FuncOption func_option) {
    hal_gh3x2x_spi_init();

    // user_timer_start(func_option);

    switch (func_option) {
        case kFuncOptInitDev:
            if (s_status_init == kFuncStatusOff) {
                APP_LOG_INFO("func_ctrl_handler: InitDev is on!");

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
                user_timer_start(func_option);
                s_status_init = kFuncStatusOn;
            }
            break;

        case kFuncOptAdt:
            if (s_status_adt == kFuncStatusOff) {
                APP_LOG_INFO("func_ctrl_start: Adt is on!");

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
                user_timer_start(func_option);
                s_status_adt = kFuncStatusOn;
            }
            break;

        case kFuncOptHr:
            if (s_status_hr == kFuncStatusOff) {
                APP_LOG_INFO("func_ctrl_start: Hr is on!");

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
                user_timer_start(func_option);
                s_status_hr = kFuncStatusOn;
            }
            break;

        case kFuncOptHrv:
            if (s_status_hrv == kFuncStatusOff) {
                APP_LOG_INFO("func_ctrl_start: Hrv is on!");

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HRV);
                user_timer_start(func_option);
                s_status_hrv = kFuncStatusOn;
            }
            break;

        case kFuncOptSpo2:
            if (s_status_spo2 == kFuncStatusOff) {
                APP_LOG_INFO("func_ctrl_start: Spo2 is on!");

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SPO2);
                user_timer_start(func_option);
                s_status_spo2 = kFuncStatusOn;
            }
            break;

        case kFuncOptRr:
            if (s_status_rr == kFuncStatusOff) {
                APP_LOG_INFO("func_ctrl_start: Rr is on!");

                // TODO: Respiratory rate

                // user_timer_start(func_option);
                s_status_rr = kFuncStatusOn;
            }
            break;

        case kFuncOptBms:
            APP_LOG_INFO("func_ctrl_start: Bms is on!");

            uint16_t percentage = 0;
            if (s_result_chg == kFuncResultOn) {
                percentage = get_battery_percentage(VOL_CHAN_CHARGING);
            } else {
                percentage = get_battery_percentage(VOL_CHAN_DISCHARGING);
            }

            APP_LOG_DEBUG("Battery percentage: %d%%", percentage);
            break;

        case kFuncOptTmp:
            APP_LOG_INFO("func_ctrl_start: Temp is on!");

            uint8_t cnt = 0;
            while (cnt < NST_STATS_SET_SIZE) {
                nst112x_fifo_add_sensor_value();
                cnt++;
                delay_ms(10);
            }

            float tmp = nst112x_get_temperature();
            APP_LOG_DEBUG("Temperature: %.2f", tmp);
            break;

        case kFuncOptStp:
            APP_LOG_INFO("func_ctrl_start: Step Counter is on!");

            uint16_t step_cnt = lsm6dso_get_step_count();
            APP_LOG_DEBUG("Step Counter: %u", step_cnt);
            break;

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}

extern GU16 lower_lvl_cnt_adt;
extern GU16 higher_lvl_cnt_adt;

void func_ctrl_stop(FuncOption func_option) {
    hal_gh3x2x_spi_init();

    if (func_option != kFuncOptAdt && s_result_adt != kFuncResultNull) {
        s_result_adt = kFuncResultNull;
    }

    switch (func_option) {
        case kFuncOptAdt:
            if (s_status_adt == kFuncStatusOn) {
                APP_LOG_INFO("func_ctrl_stop: Adt is off!");

                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
                s_switch_adt       = kFuncSwitchOff;
                lower_lvl_cnt_adt  = 0;
                higher_lvl_cnt_adt = 0;

                s_status_adt = kFuncStatusOff;
            }
            break;

        case kFuncOptHr:
            if (s_status_hr == kFuncStatusOn) {
                APP_LOG_INFO("func_ctrl_stop: Hr is off!");

                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HR);
                s_status_hr = kFuncStatusOff;
            }
            break;

        case kFuncOptHrv:
            if (s_status_hrv == kFuncStatusOn) {
                APP_LOG_INFO("func_ctrl_stop: Hrv is off!");

                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HRV);
                s_status_hrv = kFuncStatusOff;
            }
            break;

        case kFuncOptSpo2:
            if (s_status_spo2 == kFuncStatusOn) {
                APP_LOG_INFO("func_ctrl_stop: Spo2 is off!");

                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2);
                s_status_spo2 = kFuncStatusOff;
            }
            break;

        case kFuncOptRr:
            if (s_status_rr == kFuncStatusOn) {
                APP_LOG_INFO("func_ctrl_stop: Rr is off!");

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

int func_ctrl_reset_component() {
    int ret = GUNTER_FAILURE;

    ret = lsm6dso_reset_step_counter();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("lsm6dso_reset_step_counter failed with %#x!\n", ret);
    }

    return GUNTER_SUCCESS;
}

void func_ctrl_handler(void) {
    // static uint32_t count = 0;
    // APP_LOG_DEBUG("func_ctrl_handler: %u", count++);

    // APP_LOG_DEBUG("s_switch_func: %d", s_switch_func);
    // APP_LOG_DEBUG("s_switch_adt: %d", s_switch_adt);
    // APP_LOG_DEBUG("s_switch_hr: %d", s_switch_hr);
    // APP_LOG_DEBUG("s_switch_hrv: %d", s_switch_hrv);
    // APP_LOG_DEBUG("s_switch_spo2: %d", s_switch_spo2);
    // APP_LOG_DEBUG("s_switch_rr: %d", s_switch_rr);

    // APP_LOG_DEBUG("s_switch_bms: %d", s_switch_bms);
    // APP_LOG_DEBUG("s_switch_tmp: %d", s_switch_tmp);
    // APP_LOG_DEBUG("s_switch_stp: %d", s_switch_stp);

    // APP_LOG_DEBUG("s_result_act: %d", s_result_act);
    // APP_LOG_DEBUG("s_result_adt: %d", s_result_adt);

    // 重置部分功能
    if (s_switch_rst == kFuncSwitchOn) {
        func_ctrl_reset_component();
        s_switch_rst = kFuncSwitchNull;
    }

    // 处于充电模式下，后续功能不执行
    if (s_result_chg == kFuncResultOn) {
        APP_LOG_DEBUG("s_result_chg: %d", s_result_chg);
        return;
    }

    // 功能总开关未指定时，后续功能不执行
    if (s_switch_func == kFuncSwitchNull) {
        // APP_LOG_DEBUG("func_ctrl_handler: func_switch is off!");
        return;
    }

    if (s_switch_bms == kFuncSwitchOn) {
        func_ctrl_start(kFuncOptBms);
        s_switch_bms = kFuncSwitchNull;
    }

    if (s_switch_tmp == kFuncSwitchOn) {
        func_ctrl_start(kFuncOptTmp);
        s_switch_tmp = kFuncSwitchNull;
    }

    if (s_switch_stp == kFuncSwitchOn) {
        func_ctrl_start(kFuncOptStp);
        s_switch_stp = kFuncSwitchNull;
    }

    // if (s_switch_func == kFuncSwitchOn &&
    //     s_switch_adt == kFuncSwitchOn &&
    //     s_result_act == kFuncResultOff) {
    //     func_ctrl_start(kFuncOptAdt);
    // }

    // if ((s_switch_func == kFuncSwitchOn && s_status_adt == kFuncStatusOff) &&
    //     (s_result_adt == kFuncResultOn || s_result_act == kFuncResultOn)) {
    //
    //     func_ctrl_start(kFuncOptHr);
    //     func_ctrl_start(kFuncOptHrv);
    //     func_ctrl_start(kFuncOptSpo2);

    //     s_switch_func = kFuncSwitchOff;
    // }

    if (s_switch_func == kFuncSwitchOn) {
        if (s_switch_init == kFuncSwitchOn) {
            func_ctrl_start(kFuncOptInitDev);
            s_switch_init = kFuncSwitchNull;
        }

        if (s_switch_adt == kFuncStatusOn &&
            (s_result_act != kFuncResultOn && s_result_adt != kFuncResultOn)) {
            func_ctrl_start(kFuncOptAdt);
            s_switch_adt = kFuncSwitchNull;
        }

        if (s_status_adt == kFuncStatusOff &&
            (s_result_act == kFuncResultOn || s_result_adt == kFuncResultOn)) {
            if (s_switch_hr == kFuncStatusOn) {
                func_ctrl_start(kFuncOptHr);
                s_switch_hr = kFuncSwitchNull;
            }

            if (s_switch_hrv == kFuncStatusOn) {
                func_ctrl_start(kFuncOptHrv);
                s_switch_hrv = kFuncSwitchNull;
            }

            if (s_switch_spo2 == kFuncStatusOn) {
                func_ctrl_start(kFuncOptSpo2);
                s_switch_spo2 = kFuncSwitchNull;
            }

            if (s_switch_rr == kFuncStatusOn) {
                func_ctrl_start(kFuncOptRr);
                s_switch_rr = kFuncSwitchNull;
            }
        }

        s_switch_func = kFuncSwitchNull;

    } else if (s_switch_func == kFuncSwitchOff) {
        if (s_switch_adt == kFuncStatusOff) {
            func_ctrl_stop(kFuncOptAdt);
            s_switch_adt = kFuncSwitchNull;
        }

        if (s_switch_hr == kFuncStatusOff) {
            func_ctrl_stop(kFuncOptHr);
            s_switch_hr = kFuncSwitchNull;
        }

        if (s_switch_hrv == kFuncStatusOff) {
            func_ctrl_stop(kFuncOptHrv);
            s_switch_hrv = kFuncSwitchNull;
        }

        if (s_switch_spo2 == kFuncStatusOff) {
            func_ctrl_stop(kFuncOptSpo2);
            s_switch_spo2 = kFuncSwitchNull;
        }

        if (s_switch_rr == kFuncStatusOff) {
            func_ctrl_stop(kFuncOptRr);
            s_switch_rr = kFuncSwitchNull;
        }

        s_switch_func = kFuncSwitchNull;
    }
}

void func_ctrl_test(void) {
    static uint32_t count = 0;
    APP_LOG_DEBUG("func_ctrl_test: %u", count++);

    APP_LOG_DEBUG("s_switch_func: %d", s_switch_func);

    if (s_switch_func == kFuncSwitchOn) {
        // func_ctrl_start(kFuncOptAdt);
        func_ctrl_start(kFuncOptHr);
        func_ctrl_start(kFuncOptHrv);
        func_ctrl_start(kFuncOptSpo2);
        // func_ctrl_start(kFuncOptRr);

        s_switch_func = kFuncSwitchOff;
    }

    // func_ctrl_start(kFuncOptBms);
    // func_ctrl_start(kFuncOptTmp);
}
