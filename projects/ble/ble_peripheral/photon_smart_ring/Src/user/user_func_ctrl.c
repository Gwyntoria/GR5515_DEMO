#include "user_func_ctrl.h"

#include "app_error.h"
#include "app_log.h"
#include "gh3x2x_demo.h"
#include "NST112/nst112x.h"
#include "ble_error.h"

#include "gunter_ble_ctrl.h"
#include "user_battery.h"
#include "user_common.h"
#include "user_data_center.h"
#include "user_file_sys.h"
#include "user_hrv.h"
#include "user_lsm6dso.h"
#include "user_rtc.h"
#include "user_timer.h"
#include "user_3x2x_log.h"
#include "user_ble.h"

GU16 g_low_confidence_cnt_hr   = 0;
GU16 g_low_confidence_cnt_spo2 = 0;
GU16 g_low_confidence_cnt_hrv  = 0;
GU16 g_low_confidence_cnt_adt  = 0;
GU16 g_high_confidence_cnt_adt = 0;
GU16 g_data_cnt_hr             = 0;
GU16 g_data_cnt_spo2           = 0;
GU16 g_data_cnt_hrv            = 0;

// 功能开关, 默认为NULL，ON时为开启，OFF时为关闭
static FuncSwitch s_switch_3x2x = kFuncSwitchNull; // 3x2x开关
static FuncSwitch s_switch_init = kFuncSwitchNull; // 初始化开关
static FuncSwitch s_switch_adt  = kFuncSwitchNull; // 活体检测开关
static FuncSwitch s_switch_hr   = kFuncSwitchNull; // 心率检测开关
static FuncSwitch s_switch_hrv  = kFuncSwitchNull; // 心率变异性检测开关
static FuncSwitch s_switch_spo2 = kFuncSwitchNull; // 血氧检测开关

static FuncSwitch s_switch_bms = kFuncSwitchNull; // 电池检测开关
static FuncSwitch s_switch_tmp = kFuncSwitchNull; // 温度检测开关
static FuncSwitch s_switch_stp = kFuncSwitchNull; // 步数检测开关
static FuncSwitch s_switch_rst = kFuncSwitchNull; // 重置功能开关
static FuncSwitch s_switch_act = kFuncSwitchNull; // 活动检测开关
static FuncSwitch s_switch_slp = kFuncSwitchNull; // 睡眠检测开关
static FuncSwitch s_switch_ble = kFuncSwitchNull; // BLE发送数据开关
static FuncSwitch s_switch_6ds = kFuncSwitchNull; // LSM6DSO传感器开关
static FuncSwitch s_switch_fla = kFuncSwitchNull; // 擦除 Flash 开关

static FuncResult s_result_adt = kFuncResultNull; // 活体检测结果
static FuncResult s_result_act = kFuncResultNull; // 活动检测结果
static FuncResult s_result_chg = kFuncResultNull; // 充电检测结果
static FuncResult s_result_slp = kFuncResultNull; // 睡眠检测结果

// 功能状态, 默认为NULL，ON时为开启，OFF时为关闭
static FuncStatus s_status_test = kFuncStatusNull;
static FuncStatus s_status_init = kFuncStatusNull;
static FuncStatus s_status_adt  = kFuncStatusNull;
static FuncStatus s_status_hr   = kFuncStatusNull;
static FuncStatus s_status_hrv  = kFuncStatusNull;
static FuncStatus s_status_spo2 = kFuncStatusNull;
static FuncStatus s_status_3x2x  = kFuncStatusNull; // 3x2x 工作状态

// wear off
static int s_wear_off_cnt = 0;


int func_ctrl_init(void) {
    uint16_t ret = 0;

    s_switch_bms = kFuncSwitchOff;
    s_switch_tmp = kFuncSwitchOff;
    s_switch_stp = kFuncSwitchOff;
    s_switch_rst = kFuncSwitchOff;
    s_switch_act = kFuncSwitchOff;
    s_switch_slp = kFuncSwitchOff;
    s_switch_ble = kFuncSwitchOff;
    s_switch_6ds = kFuncSwitchOff;
    s_switch_fla = kFuncSwitchOff;

    s_result_adt = kFuncResultOff;
    s_result_act = kFuncResultOff;
    s_result_chg = kFuncResultOff;
    s_result_slp = kFuncResultOff;

    s_status_test = kFuncStatusOff;
    s_status_init = kFuncStatusOff;
    s_status_adt  = kFuncStatusOff;
    s_status_hr   = kFuncStatusOff;
    s_status_hrv  = kFuncStatusOff;
    s_status_spo2 = kFuncStatusOff;
    s_status_3x2x = kFuncStatusOff;

    user_timer_init(); // 定时器初始化

    // RTC间隔中断初始化
    ret = rtc_set_tick_alarm(DETECTION_INTERVAL);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("rtc_set_tick_alarm failed");
        return ret;
    }

    APP_LOG_INFO("DETECTION_INTERVAL is %us!",
                 (DETECTION_INTERVAL / SEC_TO_MS));

    // RTC日期中断初始化
    app_rtc_alarm_t rtc_alarm = {
        .min                  = 2,
        .hour                 = 0,
        .alarm_sel            = CALENDAR_ALARM_SEL_WEEKDAY,
        .alarm_date_week_mask = CALENDAR_ALARM_WEEKDAY_SUN |
                                CALENDAR_ALARM_WEEKDAY_MON |
                                CALENDAR_ALARM_WEEKDAY_TUE |
                                CALENDAR_ALARM_WEEKDAY_WED |
                                CALENDAR_ALARM_WEEKDAY_THU |
                                CALENDAR_ALARM_WEEKDAY_FRI |
                                CALENDAR_ALARM_WEEKDAY_SAT,
    };

    ret = rtc_set_date_alarm(&rtc_alarm);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("rtc_set_date_alarm failed");
        return ret;
    }

    APP_LOG_INFO("func_ctrl_init success!");

    return GUNTER_SUCCESS;
}

int func_ctrl_deinit(void) {
    s_switch_3x2x = kFuncSwitchNull;
    return GUNTER_SUCCESS;
}

/**
 * @defgroup switch function
*/

void func_ctrl_set_switch_3x2x(FuncSwitch func_switch) {
    s_switch_3x2x = func_switch;
}

void func_ctrl_set_switch_init(FuncSwitch func_switch) {
    s_switch_init = func_switch;
}

void func_ctrl_set_switch_adt(FuncSwitch func_switch) {
    s_switch_adt = func_switch;
}

void func_ctrl_set_switch_hr(FuncSwitch func_switch) {
    s_switch_hr = func_switch;
}

void func_ctrl_set_switch_hrv(FuncSwitch func_switch) {
    s_switch_hrv = func_switch;
}

void func_ctrl_set_switch_spo2(FuncSwitch func_switch) {
    s_switch_spo2 = func_switch;
}

void func_ctrl_set_switch_bms(FuncSwitch func_switch) {
    s_switch_bms = func_switch;
}

void func_ctrl_set_switch_tmp(FuncSwitch func_switch) {
    s_switch_tmp = func_switch;
}

void func_ctrl_set_switch_stp(FuncSwitch func_switch) {
    s_switch_stp = func_switch;
}

void func_ctrl_set_switch_rst(FuncSwitch func_switch) {
    s_switch_rst = func_switch;
}

void func_ctrl_set_switch_act(FuncSwitch func_switch) {
    s_switch_act = func_switch;
}

void func_ctrl_set_switch_slp(FuncSwitch func_switch) {
    s_switch_slp = func_switch;
}

void func_ctrl_set_switch_ble(FuncSwitch func_switch) {
    s_switch_ble = func_switch;
}

void func_ctrl_set_switch_lsm(FuncSwitch func_switch) {
    s_switch_6ds = func_switch;
}

void func_ctrl_set_switch_fla(FuncSwitch func_switch) {
    s_switch_fla = func_switch;
}

/**
 * @defgroup result function
*/

void func_ctrl_set_result_adt(FuncResult func_result) {
    s_result_adt = func_result;
}

void func_ctrl_set_result_act(FuncResult func_result) {
    s_result_act = func_result;
}

void func_ctrl_set_result_chg(FuncResult func_result) {
    s_result_chg = func_result;
}

void func_ctrl_set_result_slp(FuncResult func_result) {
    s_result_slp = func_result;
}

FuncResult func_ctrl_get_result_slp(void) {
    return s_result_slp;
}

/**
 * @defgroup status function
*/

void func_ctrl_set_status_3x2x_off(void) {
    s_status_3x2x = kFuncStatusOff;
}

// wear off

void wear_off_cnt_reset(void) {
    s_wear_off_cnt = 0;
}

void wear_off_cnt_condition_increase(void) {
    if (s_result_act == kFuncResultOff) {
        s_wear_off_cnt++;
    }
}

/**
 * Checks if the sampling is in progress.
 *
 * @return true if any of the sampling statuses (init, adt, hr, hrv, spo2, rr) is true, 
 * @return false otherwise.
 */
bool func_ctrl_is_sampling(void) {
    return s_status_init || s_status_adt || s_status_hr || s_status_hrv || s_status_spo2;
}

void _set_status_3x2x_off(void) {
    if (!func_ctrl_is_sampling() && s_status_3x2x == kFuncStatusOn) {
        s_status_3x2x = kFuncStatusOff;
    }
}

/**
 * @brief Caches the given data in the data center.
 *
 * This function caches the provided data in the data center for the specified data type.
 *
 * @param data_type The type of data to be cached.
 * @param data The data to be cached.
 * @return Returns GUNTER_SUCCESS if the data is successfully cached, otherwise returns GUNTER_FAILURE.
 */
int _cache_data_to_data_center(DataType data_type, uint16_t data) {
    DataCenterS2f* data_center_s2f = get_data_center_s2f();
    if (data_center_s2f == NULL) {
        APP_LOG_ERROR("Get data center f2b failed");
        return GUNTER_FAILURE;
    }

    if (data_center_s2f->recv_sensor_func) {
        int ret = data_center_s2f->recv_sensor_func(data_center_s2f, data_type, data);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("Recv sensor data failed with %d", ret);
            return ret;
        }
    }

    return GUNTER_SUCCESS;
}

/**
 * @brief Saves data to flash memory.
 *
 * This function saves data to flash memory if sampling is not in progress.
 * It retrieves the data center structure and checks if it is valid.
 * Then, it calls the send_flash_func() function of the data center to write the data to flash.
 *
 * @return GUNTER_SUCCESS if the data is successfully saved to flash, GUNTER_FAILURE otherwise.
 */
int _save_data_to_flash() {
    if (s_switch_ble == kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    if (func_ctrl_is_sampling() || s_status_3x2x) {
        return GUNTER_SUCCESS;
    }

    DataCenterS2f* data_center_s2f = get_data_center_s2f();
    if (data_center_s2f == NULL) {
        APP_LOG_ERROR("Get data center f2b failed");
        return GUNTER_FAILURE;
    }

    // S2F数据中心将数据写入到Flash
    int ret = data_center_s2f->send_flash_func(data_center_s2f);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Send data to flash failed with %d", ret);
        return ret;
    }

    return GUNTER_SUCCESS;
}

/**
 * Sends data over BLE if the BLE switch is on and there is data available in flash.
 *
 * @return GUNTER_SUCCESS if the data is sent successfully, otherwise GUNTER_FAILURE.
 */
int _ble_send_data() {
    // APP_LOG_DEBUG("s_switch_ble: %d", s_switch_ble);

    if (s_switch_ble != kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    if (func_ctrl_is_sampling() || s_status_3x2x) {
        return GUNTER_SUCCESS;
    }

    // APP_LOG_INFO("_ble_send_data");

    int ret = 0;

    DataCenterF2b* data_center_f2b = get_data_center_f2b();
    if (data_center_f2b == NULL) {
        APP_LOG_ERROR("Get data center f2b failed");
        return GUNTER_FAILURE;
    }

    // 获取Flash中可读取数据的大小
    int flash_data_len = ufs_get_readable_zone_data_size(kFlashZoneData);
    if (flash_data_len < 0) {
        APP_LOG_ERROR("Get readable zone data size failed");
        return GUNTER_FAILURE;
    }

    // APP_LOG_DEBUG("FLASH: Data length: %d", flash_data_len);

    uint16_t data_len = 0;
    static uint8_t sequence = 0;
    static uint16_t data_len_whole = 0;

    if (flash_data_len == 0) {
        APP_LOG_INFO("No data in flash");

        uint8_t response = GBC_RSP_FLASH_EMPTY;
        sdk_err_t err = gbc_notify_cmd(0, &response, GBC_RSP_TYPE_SIZE);
        APP_ERROR_CHECK(err);

        s_switch_ble = kFuncSwitchOff;
        return GUNTER_SUCCESS;

    } else if (flash_data_len > SINGLE_PACKET_MAX_DATA_LEN) {
        data_len = SINGLE_PACKET_MAX_DATA_LEN;

    } else {
        data_len = (uint16_t)flash_data_len;
    }

    if (data_len_whole == 0) {
        data_len_whole = (uint16_t)flash_data_len;
    }

    // APP_LOG_DEBUG("data_len_whole: %d", data_len_whole);

    // F2B数据中心分配内存
    ret = data_center_f2b->alloc_mem_func(data_center_f2b, data_len);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Alloc memory failed");

        uint8_t response = GBC_RSP_FLASH_ERROR;
        sdk_err_t err = gbc_notify_cmd(0, &response, GBC_RSP_TYPE_SIZE);
        APP_ERROR_CHECK(err);

        s_switch_ble = kFuncSwitchOff;
        sequence = 0;
        data_len_whole = 0;
        return GUNTER_FAILURE;
    }

    // F2B数据中心从Flash中读取数据
    ret = data_center_f2b->recv_flash_func(data_center_f2b, data_len, false, true);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Receive data from flash failed with %d", ret);

        // F2B数据中心释放内存
        ret = data_center_f2b->free_mem_func(data_center_f2b);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("Free memory failed");
        }

        uint8_t response = GBC_RSP_FLASH_ERROR;
        sdk_err_t err = gbc_notify_cmd(0, &response, GBC_RSP_TYPE_SIZE);
        APP_ERROR_CHECK(err);

        s_switch_ble = kFuncSwitchOff;
        sequence = 0;
        data_len_whole = 0;

        return GUNTER_FAILURE;
    }

    uint32_t center_data_len = data_center_f2b->get_data_size_func(data_center_f2b);

    // APP_LOG_DEBUG("CENTER: Data length: %d", center_data_len);

    // BLE发送数据
    uint32_t pack_len = center_data_len + DATA_PACKET_HEADER_SIZE + DATA_PACKET_CHEKSUM_SIZE;
    uint8_t* packet   = (uint8_t*)sys_malloc(pack_len);
    if (packet == NULL) {
        APP_LOG_ERROR("Memory allocation failed");

        // F2B数据中心释放内存
        ret = data_center_f2b->free_mem_func(data_center_f2b);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("Free memory failed");
        }

        uint8_t response = GBC_RSP_FLASH_ERROR;
        sdk_err_t err = gbc_notify_cmd(0, &response, GBC_RSP_TYPE_SIZE);
        APP_ERROR_CHECK(err);

        s_switch_ble = kFuncSwitchOff;
        sequence = 0;
        data_len_whole = 0;

        return GUNTER_ERR_NULL_POINTER;
    }

    memset(packet, 0, pack_len);

    ret = data_center_f2b->send_ble_func(data_center_f2b, packet, (uint8_t)center_data_len, sequence, data_len_whole);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Send data to BLE failed");
        sys_free(packet);
        packet = NULL;

        // F2B数据中心释放内存
        ret = data_center_f2b->free_mem_func(data_center_f2b);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("Free memory failed");
        }

        uint8_t response = GBC_RSP_FLASH_ERROR;
        sdk_err_t err = gbc_notify_cmd(0, &response, GBC_RSP_TYPE_SIZE);
        APP_ERROR_CHECK(err);

        s_switch_ble = kFuncSwitchOff;
        sequence = 0;
        data_len_whole = 0;

        return GUNTER_FAILURE;
    }

    // data_stream_hex(buffer, pack_len);
    // uint64_t time_op = rtc_get_relative_ms();

    sdk_err_t err = gbc_notify_data(0, packet, (uint16_t)pack_len);
    APP_ERROR_CHECK(err);

    sys_free(packet);
    packet = NULL;

    // uint64_t time_ed = rtc_get_relative_ms();
    // APP_LOG_INFO("BLE send data time: %d ms", (int)(time_ed - time_op));

    calculate_interval_from_previous("BLE");

    // F2B数据中心释放内存
    ret = data_center_f2b->free_mem_func(data_center_f2b);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Free memory failed");
        return GUNTER_FAILURE;
    }

    int left_data_len = flash_data_len - (int)data_len;
    if (left_data_len > 0) {
        s_switch_ble = kFuncSwitchOn;
        sequence++;

    } else if (left_data_len == 0) {
        APP_LOG_INFO("The data in flash has been sent all");

        s_switch_ble = kFuncSwitchOff;
        sequence = 0;
        data_len_whole = 0;

        // ret = ufs_erase_zone_data(kFlashZoneData);
        // if (ret != GUNTER_SUCCESS) {
        //     APP_LOG_ERROR("Erase zone data failed");
        //     return ret;
        // }

        // APP_LOG_INFO("Erase zone data success");
    }

    return GUNTER_SUCCESS;
}

int _get_battery_percentage(void) {
    if (s_switch_bms != kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    if (s_switch_ble == kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    uint16_t percentage = 0;
    if (s_result_chg == kFuncResultOn) {
        percentage = get_battery_percentage(VOL_CHAN_CHARGING);
    } else {
        percentage = get_battery_percentage(VOL_CHAN_DISCHARGING);
    }

    APP_LOG_DEBUG("Battery percentage: %d%%", percentage);

    if (_cache_data_to_data_center(kDataTypeBattery, percentage) != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Cache battery data failed");
        s_switch_bms = kFuncSwitchOff;
        return GUNTER_FAILURE;
    }

    s_switch_bms = kFuncSwitchOff;

    return GUNTER_SUCCESS;
}

int _get_temperature(void) {
    if (s_switch_tmp != kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    if (s_switch_ble == kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    float tmp = nst112x_get_temperature();
    APP_LOG_DEBUG("Temperature: %.2f", tmp);

    uint16_t data = (uint16_t)float_to_fixed_point(tmp, NST112_TEMPERATURE_RANGE_MIN, NST112_TEMPERATURE_RANGE_MAX, 100);

    if (_cache_data_to_data_center(kDataTypeTemp, data) != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Cache temperature data failed");
        s_switch_tmp = kFuncSwitchOff;
        return GUNTER_FAILURE;
    }

    s_switch_tmp = kFuncSwitchOff;

    return GUNTER_SUCCESS;
}

int _get_step_counter(void) {
    if (s_switch_stp != kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    if (s_switch_ble == kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    uint16_t step_cnt = lsm6dso_get_step_count();
    APP_LOG_DEBUG("Step Counter: %u", step_cnt);

    if (_cache_data_to_data_center(kDataTypeStep, step_cnt) != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Cache step counter data failed");
        s_switch_stp = kFuncSwitchOff;
        return GUNTER_FAILURE;
    }

    s_switch_stp = kFuncSwitchOff;

    return GUNTER_SUCCESS;
}

int _get_lsm6dso_event() {
    if (s_switch_6ds != kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    if (s_switch_ble == kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    int ret = lsm6dso_get_event_status();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Get event status failed");
        s_switch_6ds = kFuncSwitchOff;
        return GUNTER_FAILURE;
    }

    s_switch_6ds = kFuncSwitchOff;

    return GUNTER_SUCCESS;
}

int _reset_component() {
    if (s_switch_rst != kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    int ret = GUNTER_FAILURE;

    ret = lsm6dso_reset_step_counter();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("lsm6dso_reset_step_counter failed with %#x!\n", ret);
    }

    s_switch_rst = kFuncSwitchOff;

    return GUNTER_SUCCESS;
}

int _erase_data_in_flash() {
    if (s_switch_fla != kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    if (s_switch_ble == kFuncSwitchOn) {
        return GUNTER_SUCCESS;
    }

    if (func_ctrl_is_sampling() || s_status_3x2x) {
        return GUNTER_SUCCESS;
    }

    int ret = ufs_erase_zone_data(kFlashZoneData);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Erase data in flash failed");
        return GUNTER_FAILURE;
    }

    s_switch_fla = kFuncSwitchOff;

    return GUNTER_SUCCESS;
}

// start & stop

void _3x2x_start(FuncOption func_option) {
    switch (func_option) {
        case kFuncOptInitDev:
            if (s_status_init == kFuncStatusOff) {
                APP_LOG_INFO("func_ctrl_handler: InitDev is on!");

                if (s_status_3x2x != kFuncStatusOn) {
                    hal_gh3x2x_spi_init();
                    s_status_3x2x = kFuncStatusOn;
                }

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
                user_timer_start(func_option);
                s_status_init = kFuncStatusOn;
            }
            break;

        case kFuncOptAdt:
            if (s_status_adt == kFuncStatusOff) {
                APP_LOG_INFO("_3x2x_start: Adt is on!");

                if (s_status_3x2x != kFuncStatusOn) {
                    hal_gh3x2x_spi_init();
                    s_status_3x2x = kFuncStatusOn;
                }

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
                user_timer_start(func_option);
                s_status_adt = kFuncStatusOn;
            }
            break;

        case kFuncOptHr:
            if (s_status_hr == kFuncStatusOff) {
                APP_LOG_INFO("_3x2x_start: Hr is on!");

                if (s_status_3x2x != kFuncStatusOn) {
                    hal_gh3x2x_spi_init();
                    s_status_3x2x = kFuncStatusOn;
                }

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
                user_timer_start(func_option);
                s_status_hr = kFuncStatusOn;
            }
            break;

        case kFuncOptHrv:
            if (s_status_hrv == kFuncStatusOff) {
                APP_LOG_INFO("_3x2x_start: Hrv is on!");

                if (s_status_3x2x != kFuncStatusOn) {
                    hal_gh3x2x_spi_init();
                    s_status_3x2x = kFuncStatusOn;
                }

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HRV);
                user_timer_start(func_option);
                s_status_hrv = kFuncStatusOn;
            }
            break;

        case kFuncOptSpo2:
            if (s_status_spo2 == kFuncStatusOff) {
                APP_LOG_INFO("_3x2x_start: Spo2 is on!");

                if (s_status_3x2x != kFuncStatusOn) {
                    hal_gh3x2x_spi_init();
                    s_status_3x2x = kFuncStatusOn;
                }

                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SPO2);
                user_timer_start(func_option);
                s_status_spo2 = kFuncStatusOn;
            }
            break;

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}


void _3x2x_stop(FuncOption func_option) {
    hal_gh3x2x_spi_init();

    if (func_option != kFuncOptAdt && s_result_adt != kFuncResultOff) {
        s_result_adt = kFuncResultOff;
    }

    switch (func_option) {
        case kFuncOptAdt:
            if (s_status_adt == kFuncStatusOn) {
                APP_LOG_INFO("_3x2x_stop: Adt is off!");

                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
                s_switch_adt = kFuncSwitchOff;

                g_low_confidence_cnt_adt  = 0;
                g_high_confidence_cnt_adt = 0;

                s_status_adt = kFuncStatusOff;
            }
            break;

        case kFuncOptHr:
            if (s_status_hr == kFuncStatusOn) {
                APP_LOG_INFO("_3x2x_stop: Hr is off!");

                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HR);

                g_low_confidence_cnt_hr = 0;
                g_data_cnt_hr           = 0;

                s_status_hr = kFuncStatusOff;
            }
            break;

        case kFuncOptHrv:
            if (s_status_hrv == kFuncStatusOn) {
                APP_LOG_INFO("_3x2x_stop: Hrv is off!");

                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HRV);

                g_low_confidence_cnt_hrv = 0;
                g_data_cnt_hrv           = 0;

                s_status_hrv = kFuncStatusOff;
            }

            uint16_t hrv = 0;
            uint16_t rr  = 0;

            if (user_hrv_calculate_hrv_rr(&hrv, &rr) == GUNTER_SUCCESS) {
                APP_LOG_DEBUG("HRV: %u ms, RR: %u bpm", hrv, rr);

                if (_cache_data_to_data_center(kDataTypeHrv, hrv) != GUNTER_SUCCESS) {
                    APP_LOG_ERROR("Cache hrv data failed");
                }

                if (_cache_data_to_data_center(kDataTypeRr, rr) != GUNTER_SUCCESS) {
                    APP_LOG_ERROR("Cache rr data failed");
                }
            }

            user_hrv_clean_data();
            break;

        case kFuncOptSpo2:
            if (s_status_spo2 == kFuncStatusOn) {
                APP_LOG_INFO("_3x2x_stop: Spo2 is off!");

                user_timer_stop(func_option);
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2);

                g_low_confidence_cnt_spo2 = 0;
                g_data_cnt_spo2           = 0;

                s_status_spo2 = kFuncStatusOff;
            }
            break;

        default:
            APP_LOG_ERROR("Function option error\r\n");
            break;
    }
}

void _handle_3x2x_function_on(FuncOption opt, FuncSwitch* sw) {
    if (*sw == kFuncSwitchOn) {
        _3x2x_start(opt);
        *sw = kFuncSwitchNull;
    }
}

void _handle_3x2x_function_off(FuncOption opt, FuncSwitch* sw) {
    if (*sw == kFuncSwitchOff) {
        _3x2x_stop(opt);
        *sw = kFuncSwitchNull;
    }
}

void _handle_3x2x_switch_on(void) {
    _handle_3x2x_function_on(kFuncOptInitDev, &s_switch_init);

    if (s_switch_adt == kFuncStatusOn && s_result_act != kFuncResultOn && s_result_adt != kFuncResultOn) {
        // Wear off detected
        if (s_wear_off_cnt > WEAR_OFF_CNT_THRESHOLD) {
            APP_LOG_INFO("Wear off detected, stop adt!");
        } else {
            _handle_3x2x_function_on(kFuncOptAdt, &s_switch_adt);
        }
    }

    if (s_status_adt == kFuncStatusOff &&
        (s_result_act == kFuncResultOn || s_result_adt == kFuncResultOn)) {
        _handle_3x2x_function_on(kFuncOptHr, &s_switch_hr);
        _handle_3x2x_function_on(kFuncOptHrv, &s_switch_hrv);
        _handle_3x2x_function_on(kFuncOptSpo2, &s_switch_spo2);
    }

    s_switch_3x2x = kFuncSwitchNull;
}

void _handle_3x2x_switch_off(void) {
    _handle_3x2x_function_off(kFuncOptInitDev, &s_switch_init);
    _handle_3x2x_function_off(kFuncOptAdt, &s_switch_adt);
    _handle_3x2x_function_off(kFuncOptHr, &s_switch_hr);
    _handle_3x2x_function_off(kFuncOptHrv, &s_switch_hrv);
    _handle_3x2x_function_off(kFuncOptSpo2, &s_switch_spo2);


    s_switch_3x2x = kFuncSwitchNull;
}

void _control_3x2x_functionality(void) {
    if (s_switch_ble == kFuncSwitchOn) {
        return;
    }

    if (s_switch_3x2x == kFuncSwitchOn) {
        _handle_3x2x_switch_on();
    } else if (s_switch_3x2x == kFuncSwitchOff) {
        _handle_3x2x_switch_off();
    }
}

void _print_3x2x_log() {
    if (s_status_3x2x != kFuncStatusOn) {
        return;
    }

    if (s_status_hr == kFuncStatusOn) {
        // APP_LOG_DEBUG("g_low_confidence_cnt_hr: %u", g_low_confidence_cnt_hr);
        // APP_LOG_DEBUG("g_data_cnt_hr: %u", g_data_cnt_hr);

        user_3x2x_log_print_hr();
    }

    if (s_status_spo2 == kFuncStatusOn) {
        // APP_LOG_DEBUG("g_low_confidence_cnt_spo2: %u", g_low_confidence_cnt_spo2);
        // APP_LOG_DEBUG("g_data_cnt_spo2: %u", g_data_cnt_spo2);

        user_3x2x_log_print_spo2();
    }

    if (s_status_hrv == kFuncStatusOn) {
        // APP_LOG_DEBUG("g_low_confidence_cnt_hrv: %u", g_low_confidence_cnt_hrv);
        // APP_LOG_DEBUG("g_data_cnt_hrv: %u", g_data_cnt_hrv);

        user_3x2x_log_print_hrv();
    }

    if (s_status_adt == kFuncStatusOn) {
        // APP_LOG_DEBUG("g_low_confidence_cnt_adt: %u", g_low_confidence_cnt_adt);
        // APP_LOG_DEBUG("g_high_confidence_cnt_adt: %u", g_high_confidence_cnt_adt);

        user_3x2x_log_print_adt();
    }
}

void func_ctrl_handler(void) {
    int ret = 0;

    // static uint32_t count = 0;
    // APP_LOG_DEBUG("func_ctrl_handler: %u", count++);

    // APP_LOG_DEBUG("s_switch_3x2x: %d", s_switch_3x2x);
    // APP_LOG_DEBUG("s_switch_adt: %d", s_switch_adt);
    // APP_LOG_DEBUG("s_switch_hr: %d", s_switch_hr);
    // APP_LOG_DEBUG("s_switch_hrv: %d", s_switch_hrv);
    // APP_LOG_DEBUG("s_switch_spo2: %d", s_switch_spo2);

    // APP_LOG_DEBUG("s_switch_bms: %d", s_switch_bms);
    // APP_LOG_DEBUG("s_switch_tmp: %d", s_switch_tmp);
    // APP_LOG_DEBUG("s_switch_stp: %d", s_switch_stp);

    // APP_LOG_DEBUG("s_result_act: %d", s_result_act);
    // APP_LOG_DEBUG("s_result_adt: %d", s_result_adt);

    // APP_LOG_DEBUG("s_status_init: %d", s_status_init);
    // APP_LOG_DEBUG("s_status_adt: %d", s_status_adt);
    // APP_LOG_DEBUG("s_status_hr: %d", s_status_hr);
    // APP_LOG_DEBUG("s_status_hrv: %d", s_status_hrv);
    // APP_LOG_DEBUG("s_status_spo2: %d", s_status_spo2);

    // APP_LOG_DEBUG("s_status_3x2x: %d", s_status_3x2x);

    // 重置部分功能
    ret = _reset_component();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_reset_component failed");
    }

    // 擦除Flash数据
    ret = _erase_data_in_flash();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_erase_data_in_flash failed");
    }

    // 处于充电模式下，后续功能不执行
    if (s_result_chg == kFuncResultOn) {
        APP_LOG_DEBUG("s_result_chg: %d", s_result_chg);
        return;
    }

    ret = _save_data_to_flash();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_save_data_to_flash failed");
    }

    ret = _ble_send_data();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_ble_send_data failed");
    }

    ret = _get_lsm6dso_event();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_get_lsm6dso_event failed");
    }

    ret = _get_battery_percentage();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_get_battery_percentage failed");
    }

    ret = _get_temperature();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_get_temperature failed");
    }

    ret = _get_step_counter();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_get_step_counter failed");
    }

    _control_3x2x_functionality();

    _set_status_3x2x_off();

    _print_3x2x_log();

}

void func_ctrl_test(void) {
    static uint32_t count = 0;
    APP_LOG_DEBUG("func_ctrl_test: %u", count++);

    APP_LOG_DEBUG("s_switch_3x2x: %d", s_switch_3x2x);

    if (s_switch_3x2x == kFuncSwitchOn) {
        // _3x2x_start(kFuncOptAdt);
        _3x2x_start(kFuncOptHr);
        _3x2x_start(kFuncOptHrv);
        _3x2x_start(kFuncOptSpo2);

        s_switch_3x2x = kFuncSwitchOff;
    }

    // _3x2x_start(kFuncOptBms);
    // _3x2x_start(kFuncOptTmp);
}
