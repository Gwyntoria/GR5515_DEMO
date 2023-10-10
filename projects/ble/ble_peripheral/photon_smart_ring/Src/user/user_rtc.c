#include "user_rtc.h"

#include "app_log.h"
#include "app_rtc.h"
#include "gr55xx_sys.h"

// #include "gh3x2x_drv.h"

#include "user_app.h"
#include "user_common.h"

static char* const week_str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

static RtcStatus rtc_status = kRtcDefault;

void rtc_evt_tick_alarm_handler() {
    // start_sampling(get_function_option());
    // printf("rtc tick trigger\n");
}

void rtc_evt_date_alarm_handler() {
    // printf("rtc data trigger\n");
}

void rtc_alarm_evt_handler(app_rtc_evt_t* p_evt) {
    switch (p_evt->type) {
        case APP_RTC_EVT_TICK_ALARM:
            // printf("[%s] APP_RTC_EVT_TICK_ALARM\n", __FUNCTION__);
            rtc_evt_tick_alarm_handler();
            break;

        case APP_RTC_EVT_DATE_ALARM:
            // printf("[%s] APP_RTC_EVT_DATE_ALARM\n", __FUNCTION__);
            rtc_evt_date_alarm_handler();
            break;

        default:
            APP_LOG_ERROR("NONE\n");
            break;
    }
}

RtcStatus rtc_get_rtc_status() {
    return rtc_status;
}

uint16_t rtc_init() {
    uint16_t       ret  = 1;
    app_rtc_time_t time = {0};

    ret = app_rtc_init(rtc_alarm_evt_handler);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_init failed with %#x!\n", ret);
        return ret;
    }

    // TODO: Get time from BLE

    time.year = 23;
    time.mon  = 10;
    time.date = 7;
    time.hour = 14;
    time.min  = 0;
    time.sec  = 0;
    time.week = 6;

    APP_LOG_INFO("Set rtc time %02d.%02d.%02d %02d:%02d:%02d\n", time.mon, time.date, time.year, time.hour, time.min,
                 time.sec);

    ret = app_rtc_init_time(&time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_init_time failed with %#x!\n", ret);
        return ret;
    }

    rtc_status = kRtcOn;

    APP_LOG_INFO("Initialize RTC success!\n");

    return 0;
}

uint16_t rtc_deinit() {
    rtc_status = kRtcOff;
    app_rtc_deinit();
}

uint16_t rtc_set_tick_alarm(uint32_t tick_interval_ms) {
    uint16_t ret = 1;

    ret = app_rtc_setup_tick(tick_interval_ms);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_setup_tick failed with %#x!\n", ret);
        return ret;
    }

    return 0;
}

uint16_t rtc_disable_tick_alarm() {
    uint16_t ret = HAL_ERROR;

    ret = app_rtc_disable_event(APP_RTC_ALARM_DISABLE_TICK);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_disable_event failed with %#x!\n", ret);
    }

    return ret;
}

uint16_t rtc_set_date_alarm(app_rtc_alarm_t* alarm) {
    uint16_t ret = HAL_ERROR;

    ret = app_rtc_setup_alarm(alarm);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_setup_alarm failed with %#x!\n", ret);
        return ret;
    }
}

uint16_t rtc_disable_date_alarm() {
    uint16_t ret = HAL_ERROR;

    ret = app_rtc_disable_event(APP_RTC_ALARM_DISABLE_DATE);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_disable_event failed with %#x!\n", ret);
    }

    return ret;
}

uint16_t rtc_disable_all_alarm() {
    uint16_t ret = HAL_ERROR;

    ret = app_rtc_disable_event(APP_RTC_ALARM_DISABLE_ALL);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_disable_event failed with %#x!\n", ret);
    }

    return ret;
}

uint16_t rtc_adjust_time(app_rtc_time_t* time) {}

uint16_t rtc_get_current_time(app_rtc_time_t* time) {
    uint16_t ret = HAL_ERROR;

    ret = app_rtc_get_time(time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_get_time failed with %#x!\n", ret);
        return (uint32_t)ret;
    }

    return ret;
}

uint32_t rtc_get_current_time_ms() {
    uint16_t       ret  = HAL_ERROR;
    app_rtc_time_t time = {0};

    ret = app_rtc_get_time(&time);
    if (ret != HAL_OK) {
        APP_LOG_ERROR("app_rtc_get_time failed with %#x!\n", ret);
        return (uint32_t)ret;
    }

    uint32_t current_time_ms = time.ms + time.sec * SEC_TO_MS + time.min * MIN_TO_MS + time.hour * HOR_TO_MS;

    return current_time_ms;
}
