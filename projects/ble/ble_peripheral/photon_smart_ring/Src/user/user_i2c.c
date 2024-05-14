#include "user_i2c.h"

#include "app_log.h"
#include "app_i2c.h"

#include "user_common.h"

#define I2C_0_OWN_ADDR (0xA1)
#define I2C_1_OWN_ADDR (0xA2)

void _i2c_evt_handler(app_i2c_evt_t* p_evt) {
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

uint16_t user_i2c_init(void) {
    uint16_t  ret = 0;

    // IIC0
    app_i2c_params_t i2c0_params_t;
    memset(&i2c0_params_t, 0, sizeof(app_i2c_params_t));

    i2c0_params_t.id   = APP_I2C_ID_0;
    i2c0_params_t.role = APP_I2C_ROLE_MASTER;

    i2c0_params_t.pin_cfg.scl.type = APP_IO_TYPE_NORMAL;
    i2c0_params_t.pin_cfg.scl.mux  = APP_IO_MUX_4;
    i2c0_params_t.pin_cfg.scl.pin  = APP_IO_PIN_10;
    i2c0_params_t.pin_cfg.scl.pull = APP_IO_NOPULL;

    i2c0_params_t.pin_cfg.sda.type = APP_IO_TYPE_NORMAL;
    i2c0_params_t.pin_cfg.sda.mux  = APP_IO_MUX_4;
    i2c0_params_t.pin_cfg.sda.pin  = APP_IO_PIN_11;
    i2c0_params_t.pin_cfg.sda.pull = APP_IO_NOPULL;

    i2c0_params_t.use_mode.type = APP_I2C_TYPE_INTERRUPT;
    // i2c0_params_t.use_mode.tx_dma_channel	= DMA_Channel0;
    // i2c0_params_t.use_mode.rx_dma_channel	= DMA_Channel1;

    i2c0_params_t.init.speed             = I2C_SPEED_400K;
    i2c0_params_t.init.own_address       = I2C_0_OWN_ADDR;
    i2c0_params_t.init.addressing_mode   = I2C_ADDRESSINGMODE_7BIT;
    i2c0_params_t.init.general_call_mode = I2C_GENERALCALL_DISABLE;

    ret = app_i2c_init(&i2c0_params_t, _i2c_evt_handler);
    if (ret != 0) {
        APP_LOG_ERROR("i2c_0 init failed with %#.4x", ret);
        return GUNTER_FAILURE;
    }

    // IIC1
    app_i2c_params_t i2c1_params_t;
    memset(&i2c1_params_t, 0, sizeof(app_i2c_params_t));

    i2c1_params_t.id   = APP_I2C_ID_1;
    i2c1_params_t.role = APP_I2C_ROLE_MASTER;

    i2c1_params_t.pin_cfg.scl.type = APP_IO_TYPE_NORMAL;
    i2c1_params_t.pin_cfg.scl.mux  = APP_IO_MUX_0;
    i2c1_params_t.pin_cfg.scl.pin  = APP_IO_PIN_30;
    i2c1_params_t.pin_cfg.scl.pull = APP_IO_NOPULL;

    i2c1_params_t.pin_cfg.sda.type = APP_IO_TYPE_NORMAL;
    i2c1_params_t.pin_cfg.sda.mux  = APP_IO_MUX_0;
    i2c1_params_t.pin_cfg.sda.pin  = APP_IO_PIN_26;
    i2c1_params_t.pin_cfg.sda.pull = APP_IO_NOPULL;

    i2c1_params_t.use_mode.type = APP_I2C_TYPE_INTERRUPT;
    // i2c1_params_t.use_mode.tx_dma_channel	= DMA_Channel0;
    // i2c1_params_t.use_mode.rx_dma_channel	= DMA_Channel1;

    i2c1_params_t.init.speed             = I2C_SPEED_400K;
    i2c1_params_t.init.own_address       = I2C_1_OWN_ADDR;
    i2c1_params_t.init.addressing_mode   = I2C_ADDRESSINGMODE_7BIT;
    i2c1_params_t.init.general_call_mode = I2C_GENERALCALL_DISABLE;

    ret = app_i2c_init(&i2c1_params_t, _i2c_evt_handler);
    if (ret != 0) {
        APP_LOG_ERROR("i2c_1 init failed with %#.4x", ret);
        return GUNTER_FAILURE;
    }

    APP_LOG_INFO("i2c init success");

    return GUNTER_SUCCESS;
}