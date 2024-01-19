#include "user_rtc.h"

#include "app_log.h"
#include "app_rtc.h"
#include "gr55xx_sys.h"

// #include "gh3x2x_drv.h"

#include "user_app.h"
#include "user_common.h"
#include "user_func_ctrl.h"

// static char* const week_str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

static RtcStatus s_rtc_status = kRtcDefault;

void rtc_evt_tick_alarm_handler() {
    APP_LOG_INFO("rtc tick alarm triggered\r\n");

    func_ctrl_set_func_switch(kFuncSwitchOn);
}

void rtc_evt_date_alarm_handler() {
    APP_LOG_INFO("rtc date alarm triggered\r\n");

}

void rtc_alarm_evt_handler(app_rtc_evt_t* p_evt) {
    switch (p_evt->type) {
        case APP_RTC_EVT_TICK_ALARM:
            rtc_evt_tick_alarm_handler();
            break;

        case APP_RTC_EVT_DATE_ALARM:
            rtc_evt_date_alarm_handler();
            break;

        default:
            APP_LOG_ERROR("NONE\n");
            break;
    }
}

RtcStatus rtc_get_rtc_status() {
    return s_rtc_status;
}

uint16_t user_rtc_init() {
    uint16_t ret = 1;

    ret = app_rtc_init(rtc_alarm_evt_handler);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_init failed with %#x!\n", ret);
        return ret;
    }

    app_rtc_time_t time;
    memset(&time, 0, sizeof(app_rtc_time_t));

    // TODO: Get time from BLE
    
    time.year = 23;
    time.mon  = 10;
    time.date = 19;
    time.hour = 10;
    time.min  = 45;
    time.sec  = 0;
    time.ms   = 0;
    time.week = 3;

    APP_LOG_INFO("set rtc time: %04d%02d%02d%02d%02d%02d%03d\n",
                 time.year + 2000,
                 time.mon,
                 time.date,
                 time.hour,
                 time.min,
                 time.sec,
                 time.ms);

    ret = app_rtc_init_time(&time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_init_time failed with %#x!\n", ret);
        return ret;
    }

    s_rtc_status = kRtcOn;

    APP_LOG_INFO("user_rtc_init success!\n");

    return 0;
}

uint16_t user_rtc_deinit() {
    if (s_rtc_status != kRtcOn) {
        return 1;
    }

    s_rtc_status = kRtcOff;
    app_rtc_deinit();

    return 0;
}

uint16_t rtc_set_tick_alarm(uint32_t tick_interval_ms) {
    uint16_t ret = 1;

    if (s_rtc_status != kRtcOn) {
        return ret;
    }

    ret = app_rtc_setup_tick(tick_interval_ms);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_setup_tick failed with %#x!\n", ret);
        return ret;
    }

    return ret;
}

uint16_t rtc_disable_tick_alarm() {
    uint16_t ret = HAL_ERROR;

    if (s_rtc_status != kRtcOn) {
        return ret;
    }

    ret = app_rtc_disable_event(APP_RTC_ALARM_DISABLE_TICK);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_disable_event failed with %#x!\n", ret);
    }

    return ret;
}

uint16_t rtc_set_date_alarm(app_rtc_alarm_t* alarm) {
    uint16_t ret = HAL_ERROR;

    if (s_rtc_status != kRtcOn) {
        return ret;
    }

    ret = app_rtc_setup_alarm(alarm);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_setup_alarm failed with %#x!\n", ret);
        return ret;
    }

    return ret;
}

uint16_t rtc_disable_date_alarm() {
    uint16_t ret = HAL_ERROR;

    if (s_rtc_status != kRtcOn) {
        return ret;
    }

    ret = app_rtc_disable_event(APP_RTC_ALARM_DISABLE_DATE);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_disable_event failed with %#x!\n", ret);
    }

    return ret;
}

uint16_t rtc_disable_all_alarm() {
    uint16_t ret = HAL_ERROR;

    if (s_rtc_status != kRtcOn) {
        return ret;
    }

    ret = app_rtc_disable_event(APP_RTC_ALARM_DISABLE_ALL);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_disable_event failed with %#x!\n", ret);
    }

    return ret;
}


uint16_t rtc_get_current_local_time(app_rtc_time_t* time) {
    uint16_t ret = HAL_ERROR;

    if (s_rtc_status != kRtcOn) {
        return ret;
    }

    ret = app_rtc_get_time(time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_get_time failed with %#x!\n", ret);
        return ret;
    }

    return ret;
}

uint16_t rtc_adjust_time(app_rtc_time_t* time) {
    uint16_t ret = HAL_ERROR;

    if (s_rtc_status != kRtcOn) {
        return ret;
    }

    ret = app_rtc_init_time(time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_init_time failed with %#x!\n", ret);
        return ret;
    }

    APP_LOG_INFO("set rtc time: %04d%02d%02d%02d%02d%02d%03d",
                 time->year + 2000,
                 time->mon,
                 time->date,
                 time->hour,
                 time->min,
                 time->sec,
                 time->ms);

    return ret;
}

uint16_t rtc_time_diff_test() {
    uint16_t ret = HAL_ERROR;

    if (s_rtc_status != kRtcOn) {
        return ret;
    }

    app_rtc_time_t time = {0};

    ret = rtc_get_current_local_time(&time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("rtc_time_diff_test failed!\n");
        return ret;
    }

    APP_LOG_INFO("current time: %04d%02d%02d%02d%02d%02d%03d",
                 time.year + 2000,
                 time.mon,
                 time.date,
                 time.hour,
                 time.min,
                 time.sec,
                 time.ms);

    return ret;
}
