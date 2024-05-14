#include "user_lsm6dso.h"

#include <stdio.h>

#include "app_i2c.h"
#include "app_log.h"
#include "gr55xx_delay.h"

#include "LSM6DSO/lsm6dso.h"
#include "LSM6DSO/lsm6dso_reg.h"

#include "user_rtc.h"
#include "user_func_ctrl.h"
#include "user_common.h"

#define LSM_I2C_ID  APP_I2C_ID_0
#define LSM_IO_MUX  APP_IO_MUX_4
#define LSM_SCL_PIN APP_IO_PIN_10
#define LSM_SDA_PIN APP_IO_PIN_11

#define LSM_MASTER_ADDR (0xA1)
#define LSM_SLAVE_ADDR  (0x6A)

#define LSM_REG_ADDR_SIZE (1)

void _lsm6dso_i2c_evt_handler(app_i2c_evt_t* p_evt) {
    switch (p_evt->type) {
        case APP_I2C_EVT_ERROR:
            // APP_LOG_INFO("APP_I2C_EVT_ERROR");
            break;

        case APP_I2C_EVT_TX_CPLT:
            // APP_LOG_INFO("APP_I2C_EVT_TX_CPLT");
            break;

        case APP_I2C_EVT_RX_DATA:
            // APP_LOG_INFO("APP_I2C_EVT_RX_DATA");
            break;
    }
}

int _lsm6dso_i2c_init(void) {
    // int ret = 0;

    // app_i2c_params_t lsm6dso_i2c_params_t;
    // memset(&lsm6dso_i2c_params_t, 0, sizeof(app_i2c_params_t));

    // // 六轴
    // lsm6dso_i2c_params_t.id   = LSM_I2C_ID;
    // lsm6dso_i2c_params_t.role = APP_I2C_ROLE_MASTER;

    // lsm6dso_i2c_params_t.pin_cfg.scl.type = APP_IO_TYPE_NORMAL;
    // lsm6dso_i2c_params_t.pin_cfg.scl.mux  = LSM_IO_MUX;
    // lsm6dso_i2c_params_t.pin_cfg.scl.pin  = LSM_SCL_PIN;
    // lsm6dso_i2c_params_t.pin_cfg.scl.pull = APP_IO_NOPULL;

    // lsm6dso_i2c_params_t.pin_cfg.sda.type = APP_IO_TYPE_NORMAL;
    // lsm6dso_i2c_params_t.pin_cfg.sda.mux  = LSM_IO_MUX;
    // lsm6dso_i2c_params_t.pin_cfg.sda.pin  = LSM_SDA_PIN;
    // lsm6dso_i2c_params_t.pin_cfg.sda.pull = APP_IO_NOPULL;

    // lsm6dso_i2c_params_t.use_mode.type = APP_I2C_TYPE_INTERRUPT;
    // // lsm6dso_i2c_params_t.use_mode.tx_dma_channel	= DMA_Channel0;
    // // lsm6dso_i2c_params_t.use_mode.rx_dma_channel	= DMA_Channel1;

    // lsm6dso_i2c_params_t.init.speed             = I2C_SPEED_400K;
    // lsm6dso_i2c_params_t.init.own_address       = LSM_MASTER_ADDR;
    // lsm6dso_i2c_params_t.init.addressing_mode   = I2C_ADDRESSINGMODE_7BIT;
    // lsm6dso_i2c_params_t.init.general_call_mode = I2C_GENERALCALL_DISABLE;

    // ret = app_i2c_init(&lsm6dso_i2c_params_t, _lsm6dso_i2c_evt_handler);
    // if (ret != 0) {
    //     APP_LOG_ERROR("lsm6dso i2c init failed with %#.4x", ret);
    //     return LSM6DSO_ERROR;
    // }

    // APP_LOG_INFO("lsm6dso i2c init success");

    return LSM6DSO_OK;
}

int _lsm6dso_i2c_deinit(void) {
    return LSM6DSO_OK;
}

int _lsm6dso_i2c_write_reg(uint16_t dev_address, uint16_t reg, uint8_t* data, uint16_t length) {
    int ret = 0;

    ret = app_i2c_mem_write_sync(LSM_I2C_ID, dev_address, reg, LSM_REG_ADDR_SIZE, data, length, 0x1000);
    if (ret != 0) {
        APP_LOG_ERROR("lsm6dso i2c write reg [%#.2x] failed", reg);
        return LSM6DSO_ERROR;
    }

    // APP_LOG_DEBUG("lsm6dso i2c write: reg[%#.2x], data[%#.2x], length[%d]", reg, *data, length);
    return ret;
}

int _lsm6dso_i2c_read_reg(uint16_t dev_address, uint16_t reg, uint8_t* data, uint16_t length) {
    uint16_t  ret = 0;

    ret = app_i2c_mem_read_sync(LSM_I2C_ID, dev_address, reg, LSM_REG_ADDR_SIZE, data, length, 0x1000);
    if (ret != 0) {
        APP_LOG_ERROR("lsm6dso i2c read reg [%#.2x] failed", reg);
        return LSM6DSO_ERROR;
    }

    // APP_LOG_DEBUG("lsm6dso i2c read: reg[%#.2x], data[%#.2x], length[%d]", reg, *data, length);
    return ret;
}

int _lsm6dso_get_tick(void) {
    uint64_t tick = rtc_get_relative_ms();

    // while (tick > 0x7FFF0000) {
    //     tick -= 0x7FFF0000;

    //     APP_LOG_DEBUG("tick: %llu", tick);
    // }

    return (int)tick;
}

void _lsm6dso_delay_ms(uint32_t ms) {
    delay_ms(ms);
}

static LSM6DSO_Object_t lsm6dso_obj;

static LSM6DSO_IO_t lsm6dso_io = {
    .Init     = _lsm6dso_i2c_init,
    .DeInit   = _lsm6dso_i2c_deinit,
    .BusType  = 0,
    .Address  = LSM_SLAVE_ADDR,
    .WriteReg = _lsm6dso_i2c_write_reg,
    .ReadReg  = _lsm6dso_i2c_read_reg,
    .GetTick  = _lsm6dso_get_tick,
    .Delay    = _lsm6dso_delay_ms,
};

/**
 * @brief Initialize the LSM6DSO sensor
 * @return 0 in case of success, an error code otherwise
 */
int lsm6dso_init(void) {
    int ret = 0;

    memset(&lsm6dso_obj, 0, sizeof(LSM6DSO_Object_t));

    ret = LSM6DSO_RegisterBusIO(&lsm6dso_obj, &lsm6dso_io);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso register bus io failed");
        return LSM6DSO_ERROR;
    }

    //* Check device id
    uint8_t device_id = 0;

    ret = LSM6DSO_ReadID(&lsm6dso_obj, &device_id);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("Read lsm6dso device_id failed");
        return LSM6DSO_ERROR;
    }

    if (device_id != LSM6DSO_ID) {
        APP_LOG_ERROR("lsm6dso device_id [%#.2x] is wrong", device_id);
        return LSM6DSO_ERROR;
    } else {
        APP_LOG_INFO("lsm6dso device connected successfully !");
    }

    //* Initialize the LSM6DSO sensor
    ret = LSM6DSO_Init(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso init failed");
        return LSM6DSO_ERROR;
    }

    //* Enable the LSM6DSO sensor
    ret = LSM6DSO_ACC_Enable(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_GYRO_Enable(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso gyro enable failed");
        return LSM6DSO_ERROR;
    }

    //* Enable the LSM6DSO sensor features
    ret = LSM6DSO_ACC_Enable_Inactivity_Detection(&lsm6dso_obj, LSM6DSO_XL_12Hz5_GY_PD, LSM6DSO_INT1_PIN);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable inactivity detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Enable_Free_Fall_Detection(&lsm6dso_obj, LSM6DSO_INT1_PIN);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable free fall detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Enable_Pedometer(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable pedometer failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Enable_Tilt_Detection(&lsm6dso_obj, LSM6DSO_INT1_PIN);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable tilt detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Enable_Wake_Up_Detection(&lsm6dso_obj, LSM6DSO_INT1_PIN);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable wake up detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Enable_Single_Tap_Detection(&lsm6dso_obj, LSM6DSO_INT1_PIN);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable single tap detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Enable_Double_Tap_Detection(&lsm6dso_obj, LSM6DSO_INT1_PIN);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable double tap detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Enable_6D_Orientation(&lsm6dso_obj, LSM6DSO_INT1_PIN);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc enable 6D orientation failed");
        return LSM6DSO_ERROR;
    }

    // ret = LSM6DSO_ACC_Enable_DRDY_On_INT1(&lsm6dso_obj);
    // if (ret != LSM6DSO_OK) {
    //     APP_LOG_ERROR("lsm6dso acc enable drdy on int1 failed");
    //     return LSM6DSO_ERROR;
    // }

    APP_LOG_INFO("lsm6dso init success");

    return ret;
}

/**
 * @brief Deinitialize the LSM6DSO sensor
 * @return 0 in case of success, an error code otherwise
 */
int lsm6dso_deinit(void) {
    int ret = 0;

    ret = LSM6DSO_ACC_Disable_Inactivity_Detection(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc disable inactivity detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Disable_Free_Fall_Detection(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc disable free fall detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Disable_Pedometer(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc disable pedometer failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Disable_Tilt_Detection(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc disable tilt detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Disable_Wake_Up_Detection(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc disable wake up detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Disable_Single_Tap_Detection(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc disable single tap detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Disable_Double_Tap_Detection(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc disable double tap detection failed");
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_ACC_Disable_6D_Orientation(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso acc disable 6D orientation failed");
        return LSM6DSO_ERROR;
    }

    // ret = LSM6DSO_ACC_Disable_DRDY_On_INT1(&lsm6dso_obj);
    // if (ret != LSM6DSO_OK) {
    //     APP_LOG_ERROR("lsm6dso acc disable drdy on int1 failed");
    //     return LSM6DSO_ERROR;
    // }

    ret = LSM6DSO_DeInit(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso deinit failed");
        return LSM6DSO_ERROR;
    }

    APP_LOG_INFO("lsm6dso deinit success");

    return ret;
}

int lsm6dso_get_activity_status(void) {
    APP_LOG_INFO("lsm6dso_get_activity_status");

    int ret = 0;

    lsm6dso_all_sources_t all_source;
    memset(&all_source, 0, sizeof(lsm6dso_all_sources_t));

    /* Check if Activity/Inactivity events */
    ret = lsm6dso_all_sources_get(&(lsm6dso_obj.Ctx), &all_source);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get all sources failed");
        return LSM6DSO_ERROR;
    }

    if (all_source.sleep_state) {
        APP_LOG_INFO("activity_status: Inactivity Detected");
    }

    if (all_source.wake_up) {
        APP_LOG_INFO("activity_status: Activity Detected");
    }

    return ret;
}

int lsm6dso_get_free_fall_status(void) {
    APP_LOG_INFO("lsm6dso_get_free_fall_status");

    int ret = 0;

    lsm6dso_all_sources_t all_source;
    memset(&all_source, 0, sizeof(lsm6dso_all_sources_t));

    /* Check if Free Fall events */
    ret = lsm6dso_all_sources_get(&(lsm6dso_obj.Ctx), &all_source);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get all sources failed");
        return LSM6DSO_ERROR;
    }

    if (all_source.free_fall) {
        APP_LOG_INFO("free_fall_status: Free Fall Detected");
    }

    return ret;
}

int lsm6dso_get_tilt_status(void) {
    APP_LOG_INFO("lsm6dso_get_tilt_status");

    int ret = 0;

    // lsm6dso_all_sources_t all_source;
    // memset(&all_source, 0, sizeof(lsm6dso_all_sources_t));

    // /* Check if Tilt events */
    // ret = lsm6dso_all_sources_get(&(lsm6dso_obj.Ctx), &all_source);
    // if (ret != LSM6DSO_OK) {
    //     APP_LOG_ERROR("lsm6dso get all sources failed");
    //     return LSM6DSO_ERROR;
    // }

    // if (all_source.tilt) {
    //     APP_LOG_INFO("tilt_status: Tilt Detected");
    // }

    uint8_t is_tilt;
    ret = lsm6dso_tilt_flag_data_ready_get(&(lsm6dso_obj.Ctx), &is_tilt);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get tilt flag failed");
        return LSM6DSO_ERROR;
    }

    if (is_tilt) {
        APP_LOG_INFO("tilt_status: Tilt Detected");
    }

    return ret;
}

int lsm6dso_get_wake_up_status(void) {
    APP_LOG_INFO("lsm6dso_get_wake_up_status");

    int ret = 0;

    lsm6dso_all_sources_t all_source;
    memset(&all_source, 0, sizeof(lsm6dso_all_sources_t));

    /* Check if Wake Up events */
    ret = lsm6dso_all_sources_get(&(lsm6dso_obj.Ctx), &all_source);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get all sources failed");
        return LSM6DSO_ERROR;
    }

    char log[128];
    memset(log, 0, sizeof(log));

    if (all_source.wake_up) {
        APP_LOG_INFO("wake_up_status: Wake Up Detected");

        sprintf(log, "Wake-Up event on ");

        if (all_source.wake_up_x) {
            strcat(log, "X");
        }

        if (all_source.wake_up_y) {
            strcat(log, "Y");
        }

        if (all_source.wake_up_z) {
            strcat(log, "Z");
        }

        strcat(log, " direction");

        APP_LOG_INFO(log);
    }

    return ret;
}

int lsm6dso_get_tap_status(void) {
    APP_LOG_INFO("lsm6dso_get_tap_status");

    int ret = 0;

    lsm6dso_all_sources_t all_source;
    memset(&all_source, 0, sizeof(lsm6dso_all_sources_t));

    /* Check if Tap events */
    ret = lsm6dso_all_sources_get(&(lsm6dso_obj.Ctx), &all_source);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get all sources failed");
        return LSM6DSO_ERROR;
    }

    char log[128];
    memset(log, 0, sizeof(log));

    if (all_source.double_tap) {
        if (all_source.tap_x) {
            strcat(log, "x-axis");
        }

        else if (all_source.tap_y) {
            strcat(log, "y-axis");
        }

        else {
            strcat(log, "z-axis");
        }

        if (all_source.tap_sign) {
            strcat(log, " negative");
        }

        else {
            strcat(log, " positive");
        }

        strcat(log, " sign");

        APP_LOG_INFO("D-Tap: %s", log);
    }

    if (all_source.single_tap) {
        if (all_source.tap_x) {
            strcat(log, "x-axis");
        }

        else if (all_source.tap_y) {
            strcat(log, "y-axis");
        }

        else {
            strcat(log, "z-axis");
        }

        if (all_source.tap_sign) {
            strcat(log, " negative");
        }

        else {
            strcat(log, " positive");
        }

        strcat(log, " sign");

        APP_LOG_INFO("S-Tap: %s", log);
    }

    return ret;
}

int lsm6dso_get_orientation_status(void) {
    APP_LOG_INFO("lsm6dso_get_orientation_status");

    int ret = 0;

    lsm6dso_all_sources_t all_source;
    memset(&all_source, 0, sizeof(lsm6dso_all_sources_t));

    /* Check if 6D Orientation events */
    ret = lsm6dso_all_sources_get(&(lsm6dso_obj.Ctx), &all_source);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get all sources failed");
        return LSM6DSO_ERROR;
    }

    char log[128];
    memset(log, 0, sizeof(log));

    if (all_source.six_d) {
        if (all_source.six_d_xh) {
            strcat(log, "XH");
        }

        if (all_source.six_d_xl) {
            strcat(log, "XL");
        }

        if (all_source.six_d_yh) {
            strcat(log, "YH");
        }

        if (all_source.six_d_yl) {
            strcat(log, "YL");
        }

        if (all_source.six_d_zh) {
            strcat(log, "ZH");
        }

        if (all_source.six_d_zl) {
            strcat(log, "ZL");
        }

        APP_LOG_INFO("6D Or. switched to %s", log);
    }

    return ret;
}

int lsm6dso_get_event_status(void) {
    // APP_LOG_INFO("lsm6dso_get_event_status");

    int ret = 0;

    static uint32_t sleep_time_op  = 0;
    static uint32_t sleep_time     = 0;
    static uint32_t sleep_duration = 0;

    static uint32_t wake_time_op  = 0;
    static uint32_t wake_time     = 0;
    static uint32_t wake_duration = 0;

    LSM6DSO_Event_Status_t status;
    memset(&status, 0, sizeof(LSM6DSO_Event_Status_t));

    ret = LSM6DSO_ACC_Get_Event_Status(&lsm6dso_obj, &status);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get event status failed");
        return GUNTER_FAILURE;
    }

    if (status.FreeFallStatus) {
        // APP_LOG_INFO("Event: FREE_FALL detected");
    }

    if (status.WakeUpStatus) {
        // APP_LOG_INFO("Event: WAKE_UP detected");

        sleep_time_op = 0;
        sleep_time    = 0;

        func_ctrl_set_result_act(kFuncResultOn);
    }

    if (status.TapStatus) {
        // APP_LOG_INFO("Event: S-TAP detected");
    }

    if (status.DoubleTapStatus) {
        // APP_LOG_INFO("Event: D-TAP detected");
    }

    if (status.D6DOrientationStatus) {
        // APP_LOG_INFO("Event: 6D OR. detected");
    }

    if (status.StepStatus) {
        // APP_LOG_INFO("Event: STEP detected");
    }

    if (status.TiltStatus) {
        // APP_LOG_INFO("Event: TILT detected");
    }

    if (status.SleepStatus) {

        if (sleep_time_op == 0) {
            sleep_time_op = rtc_get_relative_ms();
            sleep_time = sleep_time_op;
        } else {
            sleep_time = rtc_get_relative_ms();
        }

        if (sleep_time_op > 0 &&
            sleep_time > 0 &&
            sleep_time > sleep_time_op) {
            sleep_duration = sleep_time - sleep_time_op;
            // APP_LOG_INFO("Sleep Duration: %d ms", sleep_duration);
        }

        if (sleep_duration > LSM6DSO_RELAX_THRESHOLD && sleep_duration < LSM6DSO_SLEEP_THRESHOLD) {
            func_ctrl_set_result_act(kFuncResultOff);
        } else if (sleep_duration > LSM6DSO_SLEEP_THRESHOLD) {
            func_ctrl_set_result_slp(kFuncResultOn);
        }

        // APP_LOG_INFO("Event: SLEEP detected");
    }

    return ret;
}

static uint16_t s_lsm6dso_step_count = 0;

uint16_t lsm6dso_get_step_count(void) {
    // APP_LOG_INFO("lsm6dso_get_step_count");

    int ret = 0;

    ret = LSM6DSO_ACC_Get_Step_Count(&lsm6dso_obj, &s_lsm6dso_step_count);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get step count failed");
        return LSM6DSO_ERROR;
    }

    APP_LOG_INFO("step_count: %d", s_lsm6dso_step_count);

    return s_lsm6dso_step_count;
}

int lsm6dso_reset_step_counter(void) {
    APP_LOG_INFO("lsm6dso_reset_step_counter");

    int ret = 0;

    ret = LSM6DSO_ACC_Step_Counter_Reset(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso reset step count failed");
        return LSM6DSO_ERROR;
    }

    s_lsm6dso_step_count = 0;

    return ret;
}

float lsm6dso_get_temperature(void) {
    // APP_LOG_INFO("lsm6dso_get_temperature");

    int32_t ret             = 0;
    int16_t temperature_raw = 0;
    float   temperature     = 0;

    ret = lsm6dso_temperature_raw_get(&(lsm6dso_obj.Ctx), &temperature_raw);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso get temperature failed");
        return LSM6DSO_ERROR;
    }

    temperature = temperature_raw * (1 / 256.0f) + 25.0f;

    APP_LOG_DEBUG("lsm6dso temperature: %.2f", temperature);

    return temperature;
}

/**
 * @brief Test the LSM6DSO sensor
 */
void lsm6dso_test(void) {
    int ret = 0;

    // ret = lsm6dso_init();
    // if (ret != LSM6DSO_OK) {
    //     APP_LOG_ERROR("lsm6dso init failed");
    //     return;
    // }

    LSM6DSO_Axes_t acceleration;
    LSM6DSO_Axes_t angular_rate;

    lsm6dso_reset_step_counter();

    int cnt = 0;
    while (cnt++ < 10) {
        ret = LSM6DSO_ACC_GetAxes(&lsm6dso_obj, &acceleration);
        if (ret != LSM6DSO_OK) {
            APP_LOG_ERROR("lsm6dso get axes failed");
            return;
        }
        APP_LOG_INFO("lsm6dso accel: x[%d], y[%d], z[%d]", acceleration.x, acceleration.y, acceleration.z);

        ret = LSM6DSO_GYRO_GetAxes(&lsm6dso_obj, &angular_rate);
        if (ret != LSM6DSO_OK) {
            APP_LOG_ERROR("lsm6dso get axes failed");
            return;
        }
        APP_LOG_INFO("lsm6dso gyro:  x[%d], y[%d], z[%d]", angular_rate.x, angular_rate.y, angular_rate.z);

        // // lsm6dso_get_free_fall_status();
        // // lsm6dso_get_tap_status();
        // // lsm6dso_get_wake_up_status();
        // // lsm6dso_get_tilt_status();
        // // lsm6dso_get_orientation_status();
        // // lsm6dso_get_activity_status();

        lsm6dso_get_event_status();

        lsm6dso_get_step_count();

        lsm6dso_get_temperature();

        delay_ms(200);
    }
}
