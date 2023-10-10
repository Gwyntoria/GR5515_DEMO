#ifndef USER_RTC_H
#define USER_RTC_H

#include "app_rtc.h"
#include "gr55xx_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum RtcStatus {
    kRtcOn = 0,
    kRtcOff,
    kRtcDefault,
} RtcStatus;

uint16_t rtc_init();
uint16_t rtc_deinit();

RtcStatus rtc_get_rtc_status();

/**
 * @brief Set the clock alarm at millisecond intervals
 *
 * @param tick_interval_ms The time interval between two alarms
 * @return uint16_t Result of operation
 */
uint16_t rtc_set_tick_alarm(uint32_t tick_interval_ms);
uint16_t rtc_disable_tick_alarm();

/**
 * @brief Set a clock alarm triggered by a date
 *
 * @param alarm Date that triggered the clock alarm
 * @return uint16_t Result of operation
 */
uint16_t rtc_set_date_alarm(app_rtc_alarm_t* alarm);
uint16_t rtc_disable_date_alarm();

/**
 * @brief Disable all clock alarms
 *
 * @return uint16_t Result of operation
 */
uint16_t rtc_disable_all_alarm();

uint16_t rtc_get_current_time(app_rtc_time_t* time);
uint32_t rtc_get_current_time_ms();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif