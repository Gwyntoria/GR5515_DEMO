/**
 * @file ADC.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief 测量电池电压和充电电压
 * @version 0.1
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "stns01.h"

#include "app_adc.h"
#include "app_log.h"

uint8_t  covn_done = 0;
uint16_t conversion[TEST_CONV_LENGTH];
uint16_t adc_channel0_buf[TEST_CONV_LENGTH];
uint16_t adc_channel1_buf[TEST_CONV_LENGTH];
double   voltage[TEST_CONV_LENGTH];

app_adc_samle_node_t sample_node0;
app_adc_samle_node_t sample_node1;

void stns01_adc_evt_handler(app_adc_evt_t* p_evt) {
    if (p_evt->type == APP_ADC_EVT_CONV_CPLT) {
        covn_done = 1;
    }
}

void stns01_init(void) {
    app_adc_params_t stns01_vbat_struct /* = DEFAULT_PARAM_CONFIG*/;
    app_adc_params_t stns01_vin_struct;

    stns01_vbat_struct.pin_cfg.channel_n.type = APP_IO_TYPE_MSIO;
    stns01_vbat_struct.pin_cfg.channel_n.pin  = MSIO_PIN_0;
    stns01_vbat_struct.pin_cfg.channel_n.mux  = APP_IO_MUX_7;
    //	stns01_vbat_struct.pin_cfg.channel_p.type 	= APP_IO_TYPE_MSIO;
    //	stns01_vbat_struct.pin_cfg.channel_p.pin  	= MSIO_PIN_0;
    //	stns01_vbat_struct.pin_cfg.channel_p.mux  	= APP_IO_MUX_7;
    stns01_vbat_struct.use_mode.dma_channel   = DMA_Channel0;
    stns01_vbat_struct.use_mode.type          = APP_ADC_TYPE_DMA;
    stns01_vbat_struct.init.channel_n         = ADC_INPUT_SRC_IO0;
    //	stns01_vbat_struct.init.channel_p 			= ADC_INPUT_SRC_IO0;
    stns01_vbat_struct.init.input_mode        = ADC_INPUT_SINGLE;
    stns01_vbat_struct.init.ref_source        = ADC_REF_SRC_BUF_INT;
    stns01_vbat_struct.init.ref_value         = LL_ADC_REF_VALUE_1P6;
    stns01_vbat_struct.init.clock             = LL_ADC_CLK_1P6;
    app_adc_init(&stns01_vbat_struct, stns01_adc_evt_handler);

    stns01_vin_struct.pin_cfg.channel_n.type = APP_IO_TYPE_MSIO;
    stns01_vin_struct.pin_cfg.channel_n.pin  = MSIO_PIN_1;
    stns01_vin_struct.pin_cfg.channel_n.mux  = APP_IO_MUX_7;
    //	stns01_vin_struct.pin_cfg.channel_p.type 	= APP_IO_TYPE_MSIO;
    //	stns01_vin_struct.pin_cfg.channel_p.pin  	= MSIO_PIN_1;
    //	stns01_vin_struct.pin_cfg.channel_p.mux  	= APP_IO_MUX_7;
    stns01_vin_struct.use_mode.dma_channel   = DMA_Channel0;
    stns01_vin_struct.use_mode.type          = APP_ADC_TYPE_DMA;
    stns01_vin_struct.init.channel_n         = ADC_INPUT_SRC_IO1;
    //	stns01_vin_struct.init.channel_p 			= ADC_INPUT_SRC_IO1;
    stns01_vin_struct.init.input_mode        = ADC_INPUT_SINGLE;
    stns01_vin_struct.init.ref_source        = ADC_REF_SRC_BUF_INT;
    stns01_vin_struct.init.ref_value         = LL_ADC_REF_VALUE_1P6;
    stns01_vin_struct.init.clock             = LL_ADC_CLK_1P6;
    app_adc_init(&stns01_vin_struct, stns01_adc_evt_handler);

    sample_node0.channel = ADC_INPUT_SRC_IO0;
    sample_node0.p_buf   = &adc_channel0_buf[0];
    sample_node0.len     = TEST_CONV_LENGTH;

    sample_node1.channel = ADC_INPUT_SRC_IO1;
    sample_node1.p_buf   = &adc_channel1_buf[0];
    sample_node1.len     = TEST_CONV_LENGTH;

    sample_node0.next = &sample_node1;
}

double stns01_get_voltage(uint8_t channel) {
    double get_voltage  = 0.0;
    double send_voltage = 0.0;

    memset(adc_channel0_buf, 0, sizeof(adc_channel0_buf));
    memset(adc_channel1_buf, 0, sizeof(adc_channel1_buf));
    covn_done = 0;

    app_adc_multi_channel_conversion_async(&sample_node0, 2);
    while (covn_done == 0) {
        delay_us(1);
    }

    if (channel == STNS01_CHANNEL_BATTERY) {
        app_adc_voltage_intern(adc_channel0_buf, voltage, TEST_CONV_LENGTH);
        for (uint32_t i = 0; i < TEST_CONV_LENGTH; i++) // ignore the first 3 abnormal values
        {
            if (i > 5 && i < 11) {
                get_voltage = get_voltage + voltage[i];
            }
            delay_us(1000);
        }
        send_voltage = get_voltage / 5 * 2;

        return send_voltage;

    } else if (channel == STNS01_CHANNEL_CHARGING) {
        app_adc_voltage_intern(adc_channel1_buf, voltage, TEST_CONV_LENGTH);
        for (uint32_t i = 0; i < TEST_CONV_LENGTH; i++) // ignore the first 3 abnormal values
        {
            if (i > 5 && i < 11) {
                get_voltage = get_voltage + voltage[i];
            }
            delay_us(1000);
        }
        send_voltage = get_voltage / 5;

        return send_voltage;
    } else {
        APP_LOG_ERROR("Unknown channel[%d]", channel);
        return -1;
    }
}

void stns01_test(void) {
    double Power1 = 0.0, Power2 = 0.0;

    Power1 = stns01_get_voltage(STNS01_CHANNEL_BATTERY); // 获取电池电压
    APP_LOG_DEBUG("Now VABAT Power is %.2f", Power1);

    Power2 = stns01_get_voltage(STNS01_CHANNEL_CHARGING); // 获取充电时的电压
    APP_LOG_DEBUG("Now BATMS Power is %.2f", Power2);
}
