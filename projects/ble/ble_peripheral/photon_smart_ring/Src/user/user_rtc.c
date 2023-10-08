#include "user_rtc.h"

#include "app_rtc.h"
#include "gr55xx_sys.h"

// #include "gh3x2x_drv.h"

#include "user_app.h"

char *const weeday_str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void rtc_evt_tick_alarm_handler()
{
    // start_sampling(get_function_option());
    printf("rtc tick trigger\n");
}

void rtc_evt_date_alarm_handler() {
    printf("rtc data trigger\n");
}

void rtc_alarm_evt_handler(app_rtc_evt_t *p_evt)
{
    switch (p_evt->type)
    {
        case APP_RTC_EVT_TICK_ALARM:
            // printf("[%s] APP_RTC_EVT_TICK_ALARM\r\n", __FUNCTION__);
            rtc_evt_tick_alarm_handler();
            break;

        case APP_RTC_EVT_DATE_ALARM:
            // printf("[%s] APP_RTC_EVT_DATE_ALARM\r\n", __FUNCTION__);
            rtc_evt_date_alarm_handler();
            break;

        default:
            printf("NONE\r\n");
            break;
    }
}

uint16_t rtc_init()
{
    uint16_t       ret  = 1;
    app_rtc_time_t time = {0};

    ret = app_rtc_init(rtc_alarm_evt_handler);
    if (ret != HAL_OK)
    {
        printf("app_rtc_init failed with %#x!\r\n", ret);
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

    printf("Set system time %02d.%02d.%02d %02d:%02d:%02d\r\n", time.mon, time.date, time.year, time.hour, time.min,
           time.sec);
    ret = app_rtc_init_time(&time);
    if (ret != HAL_OK)
    {
        printf("app_rtc_init_time failed with %#x!\r\n", ret);
        return ret;
    }

    printf("Initialize RTC success!\r\n");

    // /** TEST: Get RTC time */
    // while (1)
    // {
    //     ret = app_rtc_get_time(&time);
    //     if (ret != HAL_OK)
    //     {
    //         printf("app_rtc_get_time failed with %#x!\r\n", ret);
    //         return ret;
    //     }

    //     printf("Current time: %02d.%02d.%02d %s %02d:%02d:%02d, %d\r\n", time.mon, time.date, time.year,
    //     weeday_str[time.week], time.hour, time.min, time.sec,
    //            time.ms);

    //     sys_delay_ms(100);
    // }

    return 0;
}

uint16_t rtc_deinit() {}

uint16_t rtc_set_alarm()
{
    uint16_t ret = 1;

    ret = app_rtc_setup_tick(500);
    if (ret != HAL_OK)
    {
        printf("app_rtc_setup_tick failed with %#x!\r\n", ret);
        return ret;
    }

    printf("Set tick alarm success!\r\n");

    // app_rtc_alarm_t alarm = {0};

    // alarm.min                  = 1;
    // alarm.hour                 = 9;
    // alarm.alarm_sel            = CALENDAR_ALARM_SEL_WEEKDAY;
    // alarm.alarm_date_week_mask = CALENDAR_ALARM_WEEKDAY_MON | CALENDAR_ALARM_WEEKDAY_TUE | CALENDAR_ALARM_WEEKDAY_WED |
    //                              CALENDAR_ALARM_WEEKDAY_THU | CALENDAR_ALARM_WEEKDAY_FRI | CALENDAR_ALARM_WEEKDAY_SAT |
    //                              CALENDAR_ALARM_WEEKDAY_SUN;

    // ret = app_rtc_setup_alarm(&alarm);
    // if (ret != HAL_OK)
    // {
    //     printf("app_rtc_setup_alarm failed with %#x!\r\n", ret);
    //     return ret;
    // }

    return 0;
}

uint16_t rtc_reset_alarm()
{
    uint16_t ret = 1;

    ret = app_rtc_disable_event(APP_RTC_ALARM_DISABLE_ALL);
    if (ret != HAL_OK)
    {
        printf("app_rtc_disable_event failed with %#x!\r\n", ret);
    }

    return ret;
}

#define SEC_TO_MS (1000)
#define MIN_TO_MS (1000 * 60)
#define HOR_TO_MS (1000 * 60 * 60)

uint32_t rtc_get_current_time_ms()
{
    uint16_t       ret  = 1;
    app_rtc_time_t time = {0};

    ret = app_rtc_get_time(&time);
    if (ret != HAL_OK)
    {
        printf("app_rtc_get_time failed with %#x!\r\n", ret);
        return (uint32_t)ret;
    }

    uint32_t current_time_ms = time.ms + time.sec * SEC_TO_MS + time.min * MIN_TO_MS + time.hour * HOR_TO_MS;
    return current_time_ms;
}
