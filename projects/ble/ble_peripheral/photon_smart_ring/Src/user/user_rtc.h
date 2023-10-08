#ifndef USER_RTC_H
#define USER_RTC_H

#include "gr55xx_sys.h"


#ifdef __cplusplus
extern "C" {
#endif


uint16_t rtc_init();
uint16_t rtc_set_alarm();
uint16_t rtc_reset_alarm();

uint32_t rtc_get_current_time_ms();

#ifdef __cplusplus
}
#endif

#endif