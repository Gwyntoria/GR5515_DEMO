#include "user_func_ctrl.h"

#include "app_log.h"
#include "gh3x2x_demo.h"

#include "NST112/nst112x.h"

#include "user_battery.h"
#include "user_common.h"
#include "user_rtc.h"
#include "user_timer.h"
#include "user_lsm6dso.h"

static FuncSwitch s_switch_fct = kFuncSwitchNull;
static FuncSwitch s_switch_adt = kFuncSwitchNull;
static FuncSwitch s_switch_bms = kFuncSwitchNull;
static FuncSwitch s_switch_tmp = kFuncSwitchNull;
static FuncSwitch s_switch_rst = kFuncSwitchNull;

static FuncResult s_result_adt = kFuncResultNull;
static FuncResult s_result_act = kFuncResultNull;
static FuncResult s_result_chg = kFuncResultNull;
static FuncResult s_result_slp = kFuncResultNull;

static FuncStatus s_status_test = kFuncStatusNull;
static FuncStatus s_status_init = kFuncStatusNull;
static FuncStatus s_status_adt  = kFuncStatusNull;
static FuncStatus s_status_hr   = kFuncStatusNull;
static FuncStatus s_status_hrv  = kFuncStatusNull;
static FuncStatus s_status_spo2 = kFuncStatusNull;
static FuncStatus s_status_rr   = kFuncStatusNull;

uint16_t func_ctrl_init(void) {
    uint16_t ret = GUNTER_SUCCESS;

    s_switch_fct = kFuncSwitchOff;
    s_switch_adt = kFuncSwitchOff;
    s_switch_bms = kFuncSwitchOff;
    s_switch_tmp = kFuncSwitchOff;
    s_switch_rst = kFuncSwitchOff;

    s_result_adt = kFuncResultOff;
    s_result_act = kFuncResultOff;
    s_result_chg = kFuncResultOff;

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
    s_switch_fct = kFuncSwitchNull;
    return GUNTER_SUCCESS;
}

// switch

void func_ctrl_set_switch_fct(FuncSwitch func_switch) {
    s_switch_fct = func_switch;
}

void func_ctrl_set_switch_adt(FuncSwitch func_switch) {
    s_switch_adt = func_switch;
}

void func_ctrl_set_switch_bms(FuncSwitch func_switch) {
    s_switch_bms = func_switch;
}

void func_ctrl_set_switch_tmp(FuncSwitch func_switch) {
    s_switch_tmp = func_switch;
}

void func_ctrl_set_switch_rst(FuncSwitch func_switch) {
    s_switch_rst = func_switch;
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

                user_timer_start(func_option);
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

            int step_cnt = 0;
            step_cnt     = lsm6dso_get_step_count();
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

    if (s_result_adt == kFuncResultOn) {
        s_result_adt = kFuncResultOff;
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

    // APP_LOG_DEBUG("s_switch_fct: %d", s_switch_fct);
    // APP_LOG_DEBUG("s_switch_adt: %d", s_switch_adt);
    // APP_LOG_DEBUG("s_switch_bms: %d", s_switch_bms);

    // APP_LOG_DEBUG("s_result_adt: %d", s_result_adt);
    // APP_LOG_DEBUG("s_result_act: %d", s_result_act);

    // 重置部分功能
    if (s_switch_rst == kFuncSwitchOn) {
        func_ctrl_reset_component();
        s_switch_rst = kFuncSwitchOff;
    }

    // 处于充电模式下，后续功能不执行
    if (s_result_chg == kFuncResultOn) {
        APP_LOG_DEBUG("s_result_chg: %d", s_result_chg);
        return;
    }

    if (s_switch_fct == kFuncSwitchOff) {
        // APP_LOG_DEBUG("func_ctrl_handler: func_switch is off!");
        return;
    }

    if (s_switch_bms == kFuncSwitchOn) {
        func_ctrl_start(kFuncOptBms);
        s_switch_bms = kFuncSwitchOff;
    }

    if (s_switch_tmp == kFuncSwitchOn) {
        func_ctrl_start(kFuncOptTmp);
        s_switch_tmp = kFuncSwitchOff;
    }

    if (s_switch_fct == kFuncSwitchOn && s_switch_adt == kFuncSwitchOn && s_result_act == kFuncResultOff) {
        func_ctrl_start(kFuncOptAdt);
    }

    if ((s_switch_fct == kFuncSwitchOn && s_status_adt == kFuncStatusOff) &&
        (s_result_adt == kFuncResultOn || s_result_act == kFuncResultOn)) {

        func_ctrl_start(kFuncOptHr);
        func_ctrl_start(kFuncOptHrv);
        func_ctrl_start(kFuncOptSpo2);

        s_switch_fct = kFuncSwitchOff;
    }
}

void func_ctrl_test(void) {
    static uint32_t count = 0;
    APP_LOG_DEBUG("func_ctrl_test: %u", count++);

    APP_LOG_DEBUG("s_switch_fct: %d", s_switch_fct);

    if (s_switch_fct == kFuncSwitchOn) {
        // func_ctrl_start(kFuncOptAdt);
        func_ctrl_start(kFuncOptHr);
        func_ctrl_start(kFuncOptHrv);
        func_ctrl_start(kFuncOptSpo2);
        // func_ctrl_start(kFuncOptRr);

        s_switch_fct = kFuncSwitchOff;
    }

    // func_ctrl_start(kFuncOptBms);
    // func_ctrl_start(kFuncOptTmp);
}
