#include "user_lsm6dso.h"

#include <stdio.h>

#include "app_i2c.h"
#include "app_log.h"
#include "gr55xx_delay.h"

#include "LSM6DSO/lsm6dso.h"
#include "LSM6DSO/lsm6dso_reg.h"

#include "user_rtc.h"

#define LSM_I2C_ID  APP_I2C_ID_0
#define LSM_IO_MUX  APP_IO_MUX_4
#define LSM_SCL_PIN APP_IO_PIN_10
#define LSM_SDA_PIN APP_IO_PIN_11

#define LSM_MASTER_ADDR (0xA1)
#define LSM_SLAVE_ADDR  (0x6A)

#define LSM_REG_ADDR_SIZE (1)

static void _lsm6dso_i2c_evt_handler(app_i2c_evt_t* p_evt) {
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

static int _lsm6dso_i2c_init(void) {
    int ret;

    app_i2c_params_t lsm6dso_i2c_params_t;

    // 六轴
    lsm6dso_i2c_params_t.id   = LSM_I2C_ID;
    lsm6dso_i2c_params_t.role = APP_I2C_ROLE_MASTER;

    lsm6dso_i2c_params_t.pin_cfg.scl.type = APP_IO_TYPE_NORMAL;
    lsm6dso_i2c_params_t.pin_cfg.scl.mux  = LSM_IO_MUX;
    lsm6dso_i2c_params_t.pin_cfg.scl.pin  = LSM_SCL_PIN;
    lsm6dso_i2c_params_t.pin_cfg.scl.pull = APP_IO_NOPULL;

    lsm6dso_i2c_params_t.pin_cfg.sda.type = APP_IO_TYPE_NORMAL;
    lsm6dso_i2c_params_t.pin_cfg.sda.mux  = LSM_IO_MUX;
    lsm6dso_i2c_params_t.pin_cfg.sda.pin  = LSM_SDA_PIN;
    lsm6dso_i2c_params_t.pin_cfg.sda.pull = APP_IO_NOPULL;

    lsm6dso_i2c_params_t.use_mode.type = APP_I2C_TYPE_INTERRUPT;
    // lsm6dso_i2c_params_t.use_mode.tx_dma_channel	= DMA_Channel0;
    // lsm6dso_i2c_params_t.use_mode.rx_dma_channel	= DMA_Channel1;

    lsm6dso_i2c_params_t.init.speed             = I2C_SPEED_400K;
    lsm6dso_i2c_params_t.init.own_address       = LSM_MASTER_ADDR;
    lsm6dso_i2c_params_t.init.addressing_mode   = I2C_ADDRESSINGMODE_7BIT;
    lsm6dso_i2c_params_t.init.general_call_mode = I2C_GENERALCALL_DISABLE;

    ret = app_i2c_init(&lsm6dso_i2c_params_t, _lsm6dso_i2c_evt_handler);
    if (ret != 0) {
        APP_LOG_ERROR("lsm6dso i2c init failed with 0x%04x", ret);
        return LSM6DSO_ERROR;
    }

    APP_LOG_INFO("lsm6dso i2c init success");

    return LSM6DSO_OK;
}

static int _lsm6dso_i2c_deinit(void) {
    return app_i2c_deinit(LSM_I2C_ID);
}

static int _lsm6dso_i2c_write_reg(uint16_t dev_address, uint16_t reg, uint8_t* data, uint16_t length) {
    int ret = 0;

    ret = app_i2c_mem_write_sync(LSM_I2C_ID, dev_address, reg, LSM_REG_ADDR_SIZE, data, length,0x1000);
    if (ret != 0) {
        APP_LOG_ERROR("lsm6dso i2c write reg [0x%02x] failed with 0x%04x", reg, ret);
        return LSM6DSO_ERROR;
    }

    // APP_LOG_DEBUG("lsm6dso i2c write: reg[0x%02x], data[0x%02x], length[%d]", reg, *data, length);
    return ret;
}

static int _lsm6dso_i2c_read_reg(uint16_t dev_address, uint16_t reg, uint8_t* data, uint16_t length) {
    int ret = 0;

    ret = app_i2c_mem_read_sync(LSM_I2C_ID, dev_address, reg, LSM_REG_ADDR_SIZE, data, length, 0x1000);
    if (ret != 0) {
        APP_LOG_ERROR("lsm6dso i2c read reg [0x%02x] failed with 0x%04x", reg, ret);
        return LSM6DSO_ERROR;
    }

    // APP_LOG_DEBUG("lsm6dso i2c read: reg[0x%02x], data[0x%02x], length[%d]", reg, *data, length);
    return ret;
}

static int _lsm6dso_get_tick(void) {
    return rtc_get_milliseconds();
}

static void _lsm6dso_delay_ms(uint32_t ms) {
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
        APP_LOG_ERROR("lsm6dso register bus io failed with 0x%02x", ret);
        return LSM6DSO_ERROR;
    }

    ret = LSM6DSO_Init(&lsm6dso_obj);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("lsm6dso init failed with 0x%02x", ret);
        return LSM6DSO_ERROR;
    }

    uint8_t id = 0;

    ret = LSM6DSO_ReadID(&lsm6dso_obj, &id);
    if (ret != LSM6DSO_OK) {
        APP_LOG_ERROR("Read lsm6dso id failed with 0x%02x", ret);
        return LSM6DSO_ERROR;
    }

    if (id != LSM6DSO_ID) {
        APP_LOG_ERROR("lsm6dso id [0x%02x] is wrong", id);
        return LSM6DSO_ERROR;
    } else {
        APP_LOG_INFO("lsm6dso device connected successfully !");
    }
    return ret;
}

// void lsm6dso_test(void) {
//     uint16_t LSM6D_Temp    = 0;
//     uint16_t LSM6D_Data[6];

//     memset(LSM6D_Data, 0, sizeof(LSM6D_Data));

//     LSM6D_Temp = LSM6DSOWTR_Read_Data(LSM6DSO_OUT_TEMP_L); // 获取温度
//     APP_LOG_DEBUG("Now LSM6D_Temp is %d\r\n", LSM6D_Temp);

//     LSM6D_Data[0] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTX_L_G);
//     APP_LOG_DEBUG("Now LSM6DSO_OUTX_L_G is %d\r\n", LSM6D_Data[0]);

//     LSM6D_Data[1] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTY_L_G);
//     APP_LOG_DEBUG("Now LSM6DSO_OUTY_L_G is %d\r\n", LSM6D_Data[1]);

//     LSM6D_Data[2] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTZ_L_G);
//     APP_LOG_DEBUG("Now LSM6DSO_OUTZ_L_G is %d\r\n", LSM6D_Data[2]);

//     LSM6D_Data[3] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTX_L_A);
//     APP_LOG_DEBUG("Now LSM6DSO_OUTX_L_A is %d\r\n", LSM6D_Data[3]);

//     LSM6D_Data[4] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTY_L_A);
//     APP_LOG_DEBUG("Now LSM6DSO_OUTY_L_A is %d\r\n", LSM6D_Data[4]);

//     LSM6D_Data[5] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTZ_L_A);
//     APP_LOG_DEBUG("Now LSM6DSO_OUTZ_L_A is %d\r\n", LSM6D_Data[5]);
// }
