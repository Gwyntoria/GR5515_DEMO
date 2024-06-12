#ifndef USER_FUNC_CTRL_H
#define USER_FUNC_CTRL_H

#include "user_common.h"
#include "gunter_ble_ctrl.h"
#include "user_data_center.h"

#define INIT_DURATION          (SEC_TO_MS * 10)
#define DETECTION_INTERVAL     (SEC_TO_MS * 60)
#define DETECTION_DURATION     (SEC_TO_MS * 40)
#define DETECTION_DURATION_ADT (SEC_TO_MS * 40)

#define LSM6DSO_RELAX_THRESHOLD (SEC_TO_MS * 60 * 2)
#define LSM6DSO_SLEEP_THRESHOLD (SEC_TO_MS * 60 * 5)

#define CONFIDENCE_THRESHOLD_BASE       (60)
#define CONFIDENCE_THRESHOLD_NADT_GREEN (75)
#define CONFIDENCE_THRESHOLD_HR         (90)
#define CONFIDENCE_THRESHOLD_HRV        (75)
#define CONFIDENCE_THRESHOLD_SPO2       (4)

#define LOWER_CONFIDENCE_THRESHOLD_CNT (30)

#define WEAR_OFF_CNT_THRESHOLD  (2)
#define DATA_CNT_THRESHOLD_RRI  (15)
#define DATA_CNT_THRESHOLD_HR   (3)
#define DATA_CNT_THRESHOLD_SPO2 (3)

#define SINGLE_PACKET_MAX_DATA_LEN (((GBC_MAX_DATA_LEN - DATA_PACKET_HEADER_SIZE - DATA_PACKET_CHEKSUM_SIZE) / FRAME_SIZE) *  FRAME_SIZE) // 224
// #define SINGLE_TRANS_MAX_DATA_LEN (SINGLE_PACKET_MAX_LEN * 3) // 672

// #define BASE_OPTION (0x01)

typedef enum FuncOption {
    kFuncOptInitDev,
    kFuncOptAdt,
    kFuncOptHr,
    kFuncOptHrv,
    kFuncOptSpo2,
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

int func_ctrl_init(void);
int func_ctrl_deinit(void);

void func_ctrl_set_switch_3x2x(FuncSwitch func_switch);
void func_ctrl_set_switch_init(FuncSwitch func_switch);
void func_ctrl_set_switch_adt(FuncSwitch func_switch);
void func_ctrl_set_switch_hr(FuncSwitch func_switch);
void func_ctrl_set_switch_hrv(FuncSwitch func_switch);
void func_ctrl_set_switch_spo2(FuncSwitch func_switch);
void func_ctrl_set_switch_bms(FuncSwitch func_switch);
void func_ctrl_set_switch_tmp(FuncSwitch func_switch);
void func_ctrl_set_switch_stp(FuncSwitch func_switch);
void func_ctrl_set_switch_rst(FuncSwitch func_switch);
void func_ctrl_set_switch_act(FuncSwitch func_switch);
void func_ctrl_set_switch_slp(FuncSwitch func_switch);
void func_ctrl_set_switch_ble(FuncSwitch func_switch);
void func_ctrl_set_switch_lsm(FuncSwitch func_switch);
void func_ctrl_set_switch_fla(FuncSwitch func_switch);

void func_ctrl_set_result_adt(FuncResult func_result);
void func_ctrl_set_result_act(FuncResult func_result);
void func_ctrl_set_result_chg(FuncResult func_result);
void func_ctrl_set_result_slp(FuncResult func_result);

void wear_off_cnt_reset(void);
void wear_off_cnt_condition_increase(void);

void func_ctrl_set_status_3x2x_off(void);

bool func_ctrl_is_sampling(void);

void func_ctrl_handler(void);

void func_ctrl_test(void);

#ifdef __cplusplus
}
#endif

#endif
