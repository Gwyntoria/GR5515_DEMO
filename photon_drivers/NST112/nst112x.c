/**
 * @file nst112x.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief 测温芯片，初始化和获取初始温度
 * @version 0.1
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "nst112x.h"

#include "app_i2c.h"
#include "app_log.h"

// 温度
#define NST_I2C_ID_0 APP_I2C_ID_0
#define NST_I2C_ID_1 APP_I2C_ID_1
#define NST_IO_MUX   APP_IO_MUX_0
#define NST_SCL_PIN  APP_IO_PIN_30
#define NST_SDA_PIN  APP_IO_PIN_26

#define NST_OWN_ADDR 0xA2

/**
 * @brief Event handler for NST112x I2C events.
 *
 * This function is called when an I2C event occurs for the NST112x device.
 * It handles different types of I2C events and performs corresponding actions.
 *
 * @param p_evt Pointer to the I2C event structure.
 */
void _nst112x_i2c_evt_handler(app_i2c_evt_t* p_evt) {
    switch (p_evt->type) {
        case APP_I2C_EVT_ERROR:
            // printf("APP_I2C_EVT_ERROR");
            break;

        case APP_I2C_EVT_TX_CPLT:
            // printf("APP_I2C_EVT_TX_CPLT");
            break;

        case APP_I2C_EVT_RX_DATA:
            // printf("APP_I2C_EVT_RX_DATA");
            break;
    }
}

/**
 * @brief Initializes the NST112X I2C interface.
 *
 * This function initializes the NST112X I2C interface with the specified parameters.
 *
 * @return 0 if the initialization is successful, -1 otherwise.
 */
int _nst112x_i2c_init(void) {
    // int ret;

    // app_i2c_params_t nst112x_params_t;

    // nst112x_params_t.id   = NST_I2C_ID_1;
    // nst112x_params_t.role = APP_I2C_ROLE_MASTER;

    // nst112x_params_t.pin_cfg.scl.type = APP_IO_TYPE_NORMAL;
    // nst112x_params_t.pin_cfg.scl.mux  = NST_IO_MUX;
    // nst112x_params_t.pin_cfg.scl.pin  = NST_SCL_PIN;
    // nst112x_params_t.pin_cfg.scl.pull = APP_IO_NOPULL;

    // nst112x_params_t.pin_cfg.sda.type = APP_IO_TYPE_NORMAL;
    // nst112x_params_t.pin_cfg.sda.mux  = NST_IO_MUX;
    // nst112x_params_t.pin_cfg.sda.pin  = NST_SDA_PIN;
    // nst112x_params_t.pin_cfg.sda.pull = APP_IO_NOPULL;

    // nst112x_params_t.use_mode.type = APP_I2C_TYPE_INTERRUPT;
    // // nst112x_params_t.use_mode.tx_dma_channel	= DMA_Channel0;
    // // nst112x_params_t.use_mode.rx_dma_channel	= DMA_Channel1;

    // nst112x_params_t.init.speed             = I2C_SPEED_400K;
    // nst112x_params_t.init.own_address       = NST_OWN_ADDR;
    // nst112x_params_t.init.addressing_mode   = I2C_ADDRESSINGMODE_7BIT;
    // nst112x_params_t.init.general_call_mode = I2C_GENERALCALL_DISABLE;

    // ret = app_i2c_init(&nst112x_params_t, _nst112x_i2c_evt_handler);
    // if (ret != 0) {
    //     APP_LOG_ERROR("nst112x i2c init failed with %#.4x", ret);
    //     return -1;
    // }

    // APP_LOG_INFO("nst112x i2c init success");

    return 0;
}

/**
 * @brief Configure the parameters of the NST112.
 *
 * Setting Nst112 Consultation Datasheet Page 18-19.
 *
 * @param nst_addr The address of the NST112X device.
 */
void _nst112x_config_register(app_i2c_id_t i2c_id, uint8_t nst_addr) {
    int ret;

    // Configuration register consortium
    union xnst112xConfigRegisterH nst112xConfigRegisterH;
    union xnst112xConfigRegisterL nst112xConfigRegisterL;

    uint8_t data[2];
    // 0x60 == 0b01100000
    nst112xConfigRegisterH.RegisterH.OS   = Continuous_Conversion_Mode;    // 0
    nst112xConfigRegisterH.RegisterH.R0R1 = Converter_Resolution_12bit;    // 11 ---- 0.625 resolution
    nst112xConfigRegisterH.RegisterH.F0F1 = Fault_Queue_1;                 // 00 ---- Fault_Queue = 1
    nst112xConfigRegisterH.RegisterH.POL  = Polarity_low;                  // 0  Now chip not used
    nst112xConfigRegisterH.RegisterH.NA   = 0;                             // 0
    nst112xConfigRegisterH.RegisterH.SD   = Continuous_Conversion_Mode_SD; // 0  ---- Continuous_Conversion_Mode

    // 0x40 == 0b01000000  0x64 = 0b01100100
    nst112xConfigRegisterL.RegisterL.CR1CR0 = Conversion_Rate_1Hz;  // 01 ---- Conversion_Rate = 1
    nst112xConfigRegisterL.RegisterL.AL     = Active_high;          // 1  Now chip not used
    nst112xConfigRegisterL.RegisterL.EM     = normal_mode;          // 0  ---- normal_mode
    nst112xConfigRegisterL.RegisterL.NA     = 0;                    // 0
    nst112xConfigRegisterL.RegisterL.HRES   = High_Resolution_R1R0; // 0  ----	not high
    nst112xConfigRegisterL.RegisterL.NA1    = 0;                    // 0

    // Send configured Register data
    data[0] = nst112xConfigRegisterH.value;
    data[1] = nst112xConfigRegisterL.value;

    ret = app_i2c_mem_write_sync(i2c_id, nst_addr, CONFIG_REG, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
    if (ret != 0) {
        APP_LOG_ERROR("nst112x: i2c_id[%d]-addr[%#.2x] config register failed with %#.4x", i2c_id, nst_addr, ret);
    }
}

/**
 * Sets the comparator threshold value for the NST112x device.
 *
 * @param nst_addr The address of the NST112x device.
 * @param LowerThreshold The lower threshold value to be set.
 * @param UpperThreshold The upper threshold value to be set.
 */
void _nst112x_comparator_threshold(app_i2c_id_t i2c_id, uint8_t nst_addr, int16_t LowerThreshold, int16_t UpperThreshold) {
    int ret = 0;
    uint8_t data[2];

    data[0] = (LowerThreshold >> 8) & 0x00FF;
    data[1] = LowerThreshold & 0x00FF;
    ret = app_i2c_mem_write_sync(i2c_id, nst_addr, T_LOW_REG, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
    if (ret != 0) {
        APP_LOG_ERROR("nst112x: i2c_id[%d]-addr[%#.2x] LowerThreshold failed with %#.4x", i2c_id, nst_addr, ret);
    }

    data[0] = (UpperThreshold >> 8) & 0x00FF;
    data[1] = UpperThreshold & 0x00FF;
    ret = app_i2c_mem_write_sync(i2c_id, nst_addr, T_HIGH_REG, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
    if (ret != 0) {
        APP_LOG_ERROR("nst112x: i2c_id[%d]-addr[%#.2x] UpperThreshold failed with %#.4x", i2c_id, nst_addr, ret);
    }
}

/**
 * @brief Get the chip ID of NST112x.
 *
 * This function reads the chip ID register of NST112x device at the specified address.
 *
 * @param nst_addr The I2C address of the NST112x device.
 * @return The chip ID of NST112x as a 16-bit unsigned integer.
 */
uint16_t _nst112x_get_chip_id(app_i2c_id_t i2c_id, uint8_t nst_addr) {
    int ret = 0;
    uint8_t buffer[2];

    ret = app_i2c_mem_read_sync(i2c_id, nst_addr, CHIP_ID_REG, 1, buffer, 2, 1000);
    if (ret != 0) {
        APP_LOG_ERROR("nst112x: i2c_id[%d]-addr[%#.2x] get chip id failed with %#.4x", i2c_id, nst_addr, ret);
    }

    delay_ms(10);
    return ((uint16_t)buffer[0] << 8) | buffer[1];
}

/**
 * @brief Initializes the NST112x device at the specified address.
 *
 * This function initializes the NST112x device at the specified I2C address.
 *
 * @param nst_addr The I2C address of the NST112x device.
 */
void _nst112x_init(app_i2c_id_t i2c_id, uint8_t nst_addr) {
    uint16_t id = 0;

    id = _nst112x_get_chip_id(i2c_id, nst_addr);
    if (id == 0xA3A3) {
        APP_LOG_INFO("nst112x: Init Success, i2c_id[%d], addr[%#.2x]", i2c_id, nst_addr);
    } else {
        APP_LOG_INFO("nst112x: Init Error, i2c_id[%d], addr[%#.2x], chip id: [%#.4x]", i2c_id, nst_addr, id);
    }

    _nst112x_config_register(i2c_id, nst_addr);

    _nst112x_comparator_threshold(i2c_id, nst_addr, 0x10, 0x30);
}

/**
 * @brief Initializes the NST112x device.
 *
 * This function initializes the NST112x device by calling the _nst112x_init function for each device.
 */
void nst112_init(void) {
    _nst112x_i2c_init();
    delay_ms(10);

#if NST_I2C_CHN_0
    _nst112x_init(NST_I2C_ID_0, NST112C_I2C_ADDR);
    _nst112x_init(NST_I2C_ID_0, NST112D_I2C_ADDR);
#endif

#if NST_I2C_CHN_1
    _nst112x_init(NST_I2C_ID_1, NST112C_I2C_ADDR);
    _nst112x_init(NST_I2C_ID_1, NST112D_I2C_ADDR);
#endif
}

/**
 * @brief Transmit data to the NST112x device.
 *
 * This function transmits data to the NST112x device at the specified I2C address.
 *
 * @param ads_addr The I2C address of the NST112x device.
 * @param Reg The register address to which the data is to be written.
 */
void nst112x_transmit(uint8_t ads_addr, uint8_t Reg) {
    app_i2c_transmit_sync(NST_I2C_ID_1, ads_addr, &Reg, 1, 0x1000);
}

/**
 * @brief Get Nst112 sensor_value
 *
 * @param i2c_id I2C ID
 * @param nst_addr I2C address
 *
 * @return value 12Bit Value
 */
int16_t _nst112x_get_sensor_value(app_i2c_id_t i2c_id, uint8_t nst_addr) {
    uint8_t buffer[2];
    int16_t value;

    app_i2c_mem_read_sync(i2c_id, nst_addr, TEMPERATURE_REG, 1, buffer, 2, 1000);
    delay_ms(1);

    value = ((int16_t)buffer[0] << 8) | buffer[1];

    return (value >> 4);
}

static int32_t  nst112x_array[NST_STATS_DATA_NUM * 2]   = {0};
static uint16_t nst112x_array_num   = 0;
static uint16_t nst112x_array_index = 0;

void nst112x_fifo_add_sensor_value() {
    int16_t value = 0;

#if NST_I2C_CHN_0
    for (int i = 0; i < 2; i++) {
        value = _nst112x_get_sensor_value(NST_I2C_ID_0, NST112C_I2C_ADDR + i);
        // APP_LOG_DEBUG("NST112C i2c[%d]-sensor[%#.2x] value: %d", NST_I2C_ID_0 , NST112C_I2C_ADDR + i, value);

        nst112x_array[nst112x_array_index] = value;
        nst112x_array_num++;
        nst112x_array_index = nst112x_array_num % NST_STATS_DATA_NUM;
    }
#endif

#if NST_I2C_CHN_1
    for (int i = 0; i < 2; i++) {
        value = _nst112x_get_sensor_value(NST_I2C_ID_1, NST112C_I2C_ADDR + i);
        // APP_LOG_DEBUG("NST112C i2c[%d]-sensor[%#.2x] value: %d", NST_I2C_ID_1 , NST112C_I2C_ADDR + i, value);

        nst112x_array[nst112x_array_index] = value;
        nst112x_array_num++;
        nst112x_array_index = nst112x_array_num % NST_STATS_DATA_NUM;
    }
}
#endif

int16_t _nst112x_fifo_get_average_sensor_value(void) {
    if (nst112x_array_num < NST_STATS_DATA_NUM) {
        return NST112X_VALUE_ERROR;
    }

    int32_t sum = 0;
    for (int i = 0; i < nst112x_array_num; i++) {
        sum += nst112x_array[i];
    }

    int16_t average = (int16_t)(sum / nst112x_array_num);

    nst112x_array_num   = 0;
    nst112x_array_index = 0;
    memset(nst112x_array, 0, sizeof(nst112x_array));

    if (average < (int16_t)0xFCE0 || average > (int16_t)0x0800) {
        APP_LOG_ERROR("NST112X_VALUE_ERROR");
        return NST112X_VALUE_ERROR;
    }

    return average;
}

float nst112x_get_temperature() {
    int16_t value       = 0;
    float   temperature = 0.0;
    uint8_t cnt         = 0;

    while (cnt < NST_STATS_SET_SIZE) {
        nst112x_fifo_add_sensor_value();
        cnt++;
        delay_ms(10);
    }

    value = _nst112x_fifo_get_average_sensor_value();
    if (value == NST112X_VALUE_ERROR) {
        APP_LOG_ERROR("NST112X_VALUE_ERROR");
        return -256.0;
    }

    temperature = value * 0.0625;

    return temperature;
}

void nst112x_test(void) {
    int count = 0;

    while (count < 10) {
        nst112x_fifo_add_sensor_value();

        count++;

        delay_ms(100);
    }

    float temperature = nst112x_get_temperature(NST112C_I2C_ADDR);
    APP_LOG_INFO("NST112C temperature: %.2f", temperature);
}
