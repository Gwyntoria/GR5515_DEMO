#ifndef STNS01_H
#define STNS01_H

#include <stdint.h>
#include <string.h>

#define TEST_CONV_LENGTH (16UL)

#define STNS01_CHANNEL_BATTERY  (0) // VABAT
#define STNS01_CHANNEL_CHARGING (1) // BATMS

#define DEFAULT_IO_CONFIG                              \
    {                                                  \
        {APP_IO_TYPE_MSIO, APP_IO_MUX_7, MSIO_PIN_0},  \
        { APP_IO_TYPE_MSIO, APP_IO_MUX_7, MSIO_PIN_0 } \
    }
#define DEFAULT_MODE_CONFIG \
    { APP_ADC_TYPE_DMA, DMA_Channel0 }
#define DEFAULT_ADC_CONFIG \
    { ADC_INPUT_SRC_IO0, ADC_INPUT_SRC_IO0, ADC_INPUT_SINGLE, ADC_REF_SRC_BUF_INT, ADC_REF_VALUE_1P6, ADC_CLK_1P6M }
#define DEFAULT_PARAM_CONFIG \
    { DEFAULT_IO_CONFIG, DEFAULT_MODE_CONFIG, DEFAULT_ADC_CONFIG }

void   stns01_init(void);
double stns01_get_voltage(uint8_t Get_channel);

void stns01_test(void);

#endif
