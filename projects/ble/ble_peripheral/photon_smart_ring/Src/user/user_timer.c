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

#include "gh3x2x_drv.h"

#include "user_func_ctrl.h"

static app_timer_id_t s_init_dev_timer_id;
static app_timer_id_t s_adt_timer_id;
static app_timer_id_t s_hr_meas_timer_id;
static app_timer_id_t s_hrv_meas_timer_id;
static app_timer_id_t s_spo2_meas_timer_id;
static app_timer_id_t s_lsm6dso_timer_id;

void _init_dev_timeout_handler(void* args) {
    func_ctrl_set_switch_func(kFuncSwitchOff);
    func_ctrl_set_switch_init(kFuncSwitchOff);
}

void _adt_meas_timeout_handler(void* args) {
    func_ctrl_set_switch_func(kFuncSwitchOff);
    func_ctrl_set_switch_adt(kFuncSwitchOff);
}

void _hr_meas_timeout_handler(void* args) {
    func_ctrl_set_switch_func(kFuncSwitchOff);
    func_ctrl_set_switch_hr(kFuncSwitchOff);

}

void _hrv_meas_timeout_handler(void* args) {
    func_ctrl_set_switch_func(kFuncSwitchOff);
    func_ctrl_set_switch_hrv(kFuncSwitchOff);
}

void _spo2_meas_timeout_handler(void* args) {
    func_ctrl_set_switch_func(kFuncSwitchOff);
    func_ctrl_set_switch_spo2(kFuncSwitchOff);
}

void _lsm6dso_timeout_handler(void* args) {
    func_ctrl_set_switch_lsm(kFuncSwitchOn);
}

void user_timer_init(void) {
    sdk_err_t error_code;

    error_code = app_timer_create(&s_init_dev_timer_id, ATIMER_ONE_SHOT, _init_dev_timeout_handler);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&s_adt_timer_id, ATIMER_ONE_SHOT, _adt_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&s_hr_meas_timer_id, ATIMER_ONE_SHOT, _hr_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&s_hrv_meas_timer_id, ATIMER_ONE_SHOT, _hrv_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&s_spo2_meas_timer_id, ATIMER_ONE_SHOT, _spo2_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);

}

void user_timer_start(FuncOption func_option) {
    sdk_err_t error_code;

    switch (func_option) {
        case kFuncOptInitDev:
            error_code = app_timer_start(s_init_dev_timer_id, INIT_DURATION, NULL);
            APP_ERROR_CHECK(error_code);
            break;

        case kFuncOptAdt:
            error_code = app_timer_start(s_adt_timer_id, DETECTION_DURATION_ADT, NULL);
            APP_ERROR_CHECK(error_code);
            break;

        case kFuncOptHr:
            error_code = app_timer_start(s_hr_meas_timer_id, DETECTION_DURATION, NULL);
            APP_ERROR_CHECK(error_code);
            break;

        case kFuncOptHrv:
            error_code = app_timer_start(s_hrv_meas_timer_id, DETECTION_DURATION, NULL);
            APP_ERROR_CHECK(error_code);
            break;

        case kFuncOptSpo2:
            error_code = app_timer_start(s_spo2_meas_timer_id, DETECTION_DURATION, NULL);
            APP_ERROR_CHECK(error_code);
            break;

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}

void user_timer_stop(FuncOption func_option) {
    if (app_timer_get_status()) {
        switch (func_option) {
            case kFuncOptInitDev:
                app_timer_stop(s_init_dev_timer_id);
                break;

            case kFuncOptAdt:
                app_timer_stop(s_adt_timer_id);
                break;

            case kFuncOptHr:
                app_timer_stop(s_hr_meas_timer_id);
                break;

            case kFuncOptHrv:
                app_timer_stop(s_hrv_meas_timer_id);
                break;

            case kFuncOptSpo2:
                app_timer_stop(s_spo2_meas_timer_id);
                break;

            default:
                APP_LOG_ERROR("Function option error\r\n");
                break;
        }
    }
}

void lsm6dso_start_timer(void) {
    sdk_err_t error_code = 0;

    error_code = app_timer_create(&s_lsm6dso_timer_id, ATIMER_REPEAT, _lsm6dso_timeout_handler);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_start(s_lsm6dso_timer_id, 10, NULL);
    APP_ERROR_CHECK(error_code);

    APP_LOG_INFO("lsm6dso timer start success");
}