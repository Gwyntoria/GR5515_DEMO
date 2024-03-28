#ifndef USER_LSM6DSO_H
#define USER_LSM6DSO_H

#include "LSM6DSO/lsm6dso.h"
#include "LSM6DSO/lsm6dso_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int lsm6dso_init(void);
int lsm6dso_deinit(void);

int lsm6dso_get_activity_status(void);
int lsm6dso_get_free_fall_status(void);
int lsm6dso_get_tilt_status(void);
int lsm6dso_get_wake_up_status(void);
int lsm6dso_get_tap_status(void);
int lsm6dso_get_orientation_status(void);

int lsm6dso_get_event_status(void);

uint16_t lsm6dso_get_step_count(void);
int lsm6dso_reset_step_counter(void);

float lsm6dso_get_temperature(void);

void lsm6dso_test(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
