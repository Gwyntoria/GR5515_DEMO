#ifndef USER_FUNC_CTRL_H
#define USER_FUNC_CTRL_H

#include "user_common.h"

#define INIT_DURATION          (SEC_TO_MS * 10)
#define DETECTION_INTERVAL     (SEC_TO_MS * 60)
#define DETECTION_DURATION     (SEC_TO_MS * 40)
#define DETECTION_DURATION_ADT (SEC_TO_MS * 30)

#define LSM6DSO_RELAX_THRESHOLD (SEC_TO_MS * 30)
#define LSM6DSO_SLEEP_THRESHOLD (SEC_TO_MS * 90)

#define CONFIDENCE_THRESHOLD            (60)
#define CONFIDENCE_THRESHOLD_NADT_GREEN (55)

#define LOWER_CONFIDENCE_THRESHOLD_CNT (30)

// #define BASE_OPTION (0x01)

typedef enum FuncOption {
    kFuncOptInitDev,
    kFuncOptAdt,
    kFuncOptHr,
    kFuncOptHrv,
    kFuncOptSpo2,
    kFuncOptRr,
    kFuncOptBms,
    kFuncOptTmp,
    kFuncOptStp,
    kFuncOptNull
} FuncOption;

typedef enum FuncSwitch {
    kFuncSwitchOff = 0,
    kFuncSwitchOn  = 1,
    kFuncSwitchNull
} FuncSwitch;

typedef enum FuncResult {
    kFuncResultOff = 0,
    kFuncResultOn  = 1,
    kFuncResultNull
} FuncResult;

typedef enum FuncStatus {
    kFuncStatusOff = 0,
    kFuncStatusOn  = 1,
    kFuncStatusNull
} FuncStatus;

#ifdef __cplusplus
extern "C" {
#endif

uint16_t func_ctrl_init(void);
uint16_t func_ctrl_deinit(void);

void func_ctrl_set_switch_func(FuncSwitch func_switch);
void func_ctrl_set_switch_init(FuncSwitch func_switch);
void func_ctrl_set_switch_adt(FuncSwitch func_switch);
void func_ctrl_set_switch_hr(FuncSwitch func_switch);
void func_ctrl_set_switch_hrv(FuncSwitch func_switch);
void func_ctrl_set_switch_spo2(FuncSwitch func_switch);
void func_ctrl_set_switch_rr(FuncSwitch func_switch);
void func_ctrl_set_switch_bms(FuncSwitch func_switch);
void func_ctrl_set_switch_tmp(FuncSwitch func_switch);
void func_ctrl_set_switch_stp(FuncSwitch func_switch);
void func_ctrl_set_switch_rst(FuncSwitch func_switch);
void func_ctrl_set_switch_act(FuncSwitch func_switch);
void func_ctrl_set_switch_slp(FuncSwitch func_switch);

void func_ctrl_set_result_adt(FuncResult func_result);
void func_ctrl_set_result_act(FuncResult func_result);
void func_ctrl_set_result_chg(FuncResult func_result);
void func_ctrl_set_result_slp(FuncResult func_result);

void func_ctrl_handler(void);

void func_ctrl_test(void);

#ifdef __cplusplus
}
#endif

#endif
