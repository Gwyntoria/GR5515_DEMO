/**
 * @file user_timer.h
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief Create and operate timers to control the switching of individual functions
 * @version 0.1
 * @date 2023-12-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef USER_TIMER_H
#define USER_TIMER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "user_func_ctrl.h"

void user_timer_init(void);
// void user_timer_deinit(void);

void user_timer_start(FuncOption func_option);
void user_timer_stop(FuncOption func_option);

void lsm6dso_start_timer(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
