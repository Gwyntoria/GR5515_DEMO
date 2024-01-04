/**
 * @file user_timer.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief Create and operate timers to control the switching of individual
 * functions
 * @version 0.1
 * @date 2023-12-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "user_timer.h"

#include "app_error.h"
#include "app_log.h"
#include "app_timer.h"

#include "user_func_ctrl.h"

static app_timer_id_t s_battery_level_timer_id;
static app_timer_id_t s_heart_rate_meas_timer_id;
static app_timer_id_t s_spo2_meas_timer_id;
static app_timer_id_t s_rr_interval_meas_timer_id;

/**
 * @brief Function for performing battery measurement and updating the Battery
 *        Level characteristic in Battery Service.
 *
 */
static void battery_level_update(void* args) {
    // TODO: 响应电池状态请求
}

static void heart_rate_meas_timeout_handler(void* args) {
    func_ctrl_stop_sampling(kFuncOptHr);
    func_ctrl_stop_sampling(kFuncOptHrv);
}

/**
 * @brief stop measuring spo2
 *
 */
static void spo2_meas_timeout_handler(void* args) {
    func_ctrl_stop_sampling(kFuncOptSpo2);
}

static void rr_interval_meas_timeout_handler(void* args) {
    // TODO: 结束呼吸频率检测
}

static void user_timer_init(void) {
    sdk_err_t error_code;

    error_code = app_timer_create(&s_battery_level_timer_id, ATIMER_REPEAT, battery_level_update);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&s_heart_rate_meas_timer_id, ATIMER_REPEAT, heart_rate_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&s_spo2_meas_timer_id, ATIMER_REPEAT, spo2_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&s_rr_interval_meas_timer_id, ATIMER_REPEAT, rr_interval_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);
}