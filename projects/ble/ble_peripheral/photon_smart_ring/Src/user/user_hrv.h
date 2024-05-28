#ifndef USER_HRV_H
#define USER_HRV_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void user_hrv_init(void);

int      user_hrv_add_data(int rri_data);
int      user_hrv_get_data(int* rri_data);
uint16_t user_hrv_get_rri_data_count(void);
void     user_hrv_clean_data(void);

int user_hrv_get_data_average(int* hrv_data_average);
int user_hrv_calculate_hrv_rr(uint16_t* rmssd, uint16_t* respiratory_rate);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif