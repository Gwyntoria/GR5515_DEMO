#ifndef USER_RTC_H
#define USER_RTC_H

#include "app_rtc.h"
#include "app_log_store.h"
#include "gr55xx_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum RtcStatus {
    kRtcOff,
    kRtcOn,
    kRtcDefault,
} RtcStatus;

uint16_t user_rtc_init(void);
uint16_t user_rtc_deinit(void);

RtcStatus rtc_get_rtc_status(void);

/**
 * @brief Set the clock alarm at millisecond intervals
 *
 * @param tick_interval_ms The time interval between two alarms
 * @return uint16_t Result of operation
 */
uint16_t rtc_set_tick_alarm(uint32_t tick_interval_ms);
uint16_t rtc_disable_tick_alarm(void);

/**
 * @brief Set a clock alarm triggered by a date
 *
 * @param alarm Date that triggered the clock alarm
 * @return uint16_t Result of operation
 */
uint16_t rtc_set_date_alarm(app_rtc_alarm_t* alarm);
uint16_t rtc_disable_date_alarm(void);

/**
 * @brief Disable all clock alarms
 *
 * @return uint16_t Result of operation
 */
uint16_t rtc_disable_all_alarm(void);

uint16_t rtc_get_current_local_time(app_rtc_time_t* time);
int rtc_get_log_real_time(app_log_store_time_t* p_time);

uint16_t rtc_adjust_time(app_rtc_time_t* time);

uint16_t rtc_time_diff_test(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
