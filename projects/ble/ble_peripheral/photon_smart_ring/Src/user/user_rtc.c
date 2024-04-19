#include "user_rtc.h"

#include "app_log.h"
#include "app_log_store.h"
#include "app_rtc.h"
#include "gr55xx_sys.h"

// #include "gh3x2x_drv.h"

#include "user_app.h"
#include "user_common.h"
#include "user_func_ctrl.h"
#include "user_lsm6dso.h"

#define RTC_TIMESTAMP_SIZE 26 /**< [00000000.000] */

#define MS_PER_SEC   1000
#define SEC_PER_MIN  60
#define MIN_PER_HOUR 60
#define HOUR_PER_DAY 24
#define DAY_PER_YEAR 365
#define BASE_YEAR    2010

// static char* const week_str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

static RtcStatus s_rtc_status = kRtcDefault;

void rtc_evt_tick_alarm_handler(viod) {
    APP_LOG_INFO("rtc tick alarm triggered");

    func_ctrl_set_switch_func(kFuncSwitchOn);
    func_ctrl_set_switch_adt(kFuncSwitchOn);
    func_ctrl_set_switch_bms(kFuncSwitchOn);
    func_ctrl_set_switch_tmp(kFuncSwitchOn);
}

void rtc_evt_date_alarm_handler(viod) {
    APP_LOG_INFO("rtc date alarm triggered");

    func_ctrl_set_switch_rst(kFuncSwitchOn);
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

RtcStatus rtc_get_rtc_status(viod) {
    return s_rtc_status;
}

uint16_t user_rtc_init(viod) {
    uint16_t ret = 1;

    ret = app_rtc_init(rtc_alarm_evt_handler);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_init failed with %#x!\n", ret);
        return ret;
    }

    app_rtc_time_t time;
    memset(&time, 0, sizeof(app_rtc_time_t));

    // TODO: Get time from BLE

    time.year = 14;
    time.mon  = 4;
    time.date = 18;
    time.hour = 0;
    time.min  = 0;
    time.sec  = 0;
    time.ms   = 0;
    time.week = 4;

    APP_LOG_INFO("set rtc time: [%04d/%02d/%02d %02d:%02d:%02d:%03d]",
                 time.year + BASE_YEAR,
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

    // APP_LOG_INFO("user_rtc_init success!");

    return 0;
}

uint16_t user_rtc_deinit(viod) {
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

uint16_t rtc_disable_tick_alarm(viod) {
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

uint16_t rtc_disable_date_alarm(viod) {
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

uint16_t rtc_disable_all_alarm(viod) {
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

char* rtc_get_current_timestamp(void) {
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
             rtc_time.year + BASE_YEAR, rtc_time.mon, rtc_time.date,
             rtc_time.hour, rtc_time.min, rtc_time.sec, rtc_time.ms);

    return timestamp;
}

void rtc_get_log_real_time(app_log_store_time_t* p_time) {
    if (p_time == NULL) {
        APP_LOG_ERROR("init log time: pointer is null");
    }

    if (s_rtc_status != kRtcOn) {
        APP_LOG_ERROR("rtc is not initialized");
    }

    int ret = 0;

    app_rtc_time_t rtc_time;
    memset(&rtc_time, 0, sizeof(app_rtc_time_t));

    ret = app_rtc_get_time(&rtc_time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_get_time failed with %#x", ret);
    }

    p_time->year  = rtc_time.year;
    p_time->month = rtc_time.mon;
    p_time->day   = rtc_time.date;
    p_time->hour  = rtc_time.hour;
    p_time->min   = rtc_time.min;
    p_time->sec   = rtc_time.sec;
    p_time->msec  = rtc_time.ms;
}

uint64_t rtc_get_relative_ms(void) {
    uint16_t ret = HAL_ERROR;
    uint64_t ms = 0;
    app_rtc_time_t rtc_time;
    memset(&rtc_time, 0, sizeof(app_rtc_time_t));

    ret = app_rtc_get_time(&rtc_time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_get_time failed with %#x!\n", ret);
        return -1;
    }

    // APP_LOG_DEBUG("rtc_time: [%04d/%02d/%02d %02d:%02d:%02d:%03d]",
    //               rtc_time.year + BASE_YEAR,
    //               rtc_time.mon,
    //               rtc_time.date,
    //               rtc_time.hour,
    //               rtc_time.min,
    //               rtc_time.sec,
    //               rtc_time.ms);

    int years = (int)(rtc_time.year) + BASE_YEAR;

    // Convert years to milliseconds
    ms += (uint64_t)(rtc_time.year) * DAY_PER_YEAR * HOUR_PER_DAY * MIN_PER_HOUR * SEC_PER_MIN * MS_PER_SEC;

    // Convert months to milliseconds
    static const uint16_t days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (uint8_t i = 1; i < rtc_time.mon; i++) {
        ms += days_in_month[i] * HOUR_PER_DAY * MIN_PER_HOUR * SEC_PER_MIN * MS_PER_SEC;
        // Leap year check
        if (i == 2 && ((years % 4 == 0 && years % 100 != 0) || years % 400 == 0)) {
            ms += HOUR_PER_DAY * MIN_PER_HOUR * SEC_PER_MIN * MS_PER_SEC;
        }
    }

    // Convert days to milliseconds
    ms += (uint64_t)(rtc_time.date) * HOUR_PER_DAY * MIN_PER_HOUR * SEC_PER_MIN * MS_PER_SEC;

    // Convert hours, minutes, seconds, and milliseconds to milliseconds
    ms += (uint64_t)(rtc_time.hour) * MIN_PER_HOUR * SEC_PER_MIN * MS_PER_SEC;
    ms += (uint64_t)(rtc_time.min) * SEC_PER_MIN * MS_PER_SEC;
    ms += (uint64_t)(rtc_time.sec) * MS_PER_SEC;
    ms += (uint64_t)(rtc_time.ms);

    return ms;
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
                 time->year + BASE_YEAR,
                 time->mon,
                 time->date,
                 time->hour,
                 time->min,
                 time->sec,
                 time->ms);

    return ret;
}

uint16_t rtc_time_diff_test(viod) {
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
