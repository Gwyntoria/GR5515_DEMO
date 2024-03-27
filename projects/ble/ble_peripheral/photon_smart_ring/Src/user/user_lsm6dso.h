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

int lsm6dso_get_event_status(void);

void lsm6dso_test(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
