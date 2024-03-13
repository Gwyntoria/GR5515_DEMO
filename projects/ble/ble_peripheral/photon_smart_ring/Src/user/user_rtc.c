#include "user_rtc.h"

#include "app_log.h"
#include "app_rtc.h"
#include "app_log_store.h"
#include "gr55xx_sys.h"

// #include "gh3x2x_drv.h"

#include "user_app.h"
#include "user_common.h"
#include "user_func_ctrl.h"

#define RTC_TIMESTAMP_SIZE 26 /**< [00000000.000] */

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
    
    time.year = 24;
    time.mon  = 3;
    time.date = 12;
    time.hour = 18;
    time.min  = 0;
    time.sec  = 0;
    time.ms   = 0;
    time.week = 2;

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

char* rtc_get_current_timestamp() {
    uint16_t       ret = HAL_ERROR;
    static char    timestamp[RTC_TIMESTAMP_SIZE];
    app_rtc_time_t rtc_time;

    memset(timestamp, 0, sizeof(timestamp));
    memset(&rtc_time, 0, sizeof(rtc_time));

    ret = app_rtc_get_time(&rtc_time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_get_time failed with %#x!\n", ret);
        return NULL;
    }

    snprintf(timestamp,
             RTC_TIMESTAMP_SIZE,
             "[%04d/%02d/%02d %02d:%02d:%02d:%03d] ",
             rtc_time.year, rtc_time.mon, rtc_time.date,
             rtc_time.hour, rtc_time.min, rtc_time.sec, rtc_time.ms);

    return timestamp;
}

int rtc_get_log_real_time(app_log_store_time_t* p_time) {
    if (p_time == NULL) {
        APP_LOG_ERROR("init log time: pointer is null");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (s_rtc_status != kRtcOn) {
        APP_LOG_ERROR("rtc is not initialized");
        return GUNTER_FAILURE;
    }

    int ret = 0;

    app_rtc_time_t rtc_time;
    memset(&rtc_time, 0, sizeof(app_rtc_time_t));

    ret = app_rtc_get_time(&rtc_time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_get_time failed with %#x", ret);
        return ret;
    }

    p_time->year  = rtc_time.year;
    p_time->month = rtc_time.mon;
    p_time->day   = rtc_time.date;
    p_time->hour  = rtc_time.hour;
    p_time->min   = rtc_time.min;
    p_time->sec   = rtc_time.sec;
    p_time->msec  = rtc_time.ms;

    return GUNTER_SUCCESS;
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
