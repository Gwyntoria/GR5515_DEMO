#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>
#include <string.h>


#define TEST_CONV_LENGTH                (16UL)


#define DEFAULT_IO_CONFIG               {{ APP_IO_TYPE_MSIO, APP_IO_MUX_7, MSIO_PIN_0 },\
                                         { APP_IO_TYPE_MSIO, APP_IO_MUX_7, MSIO_PIN_0 }}
#define DEFAULT_MODE_CONFIG             { APP_ADC_TYPE_DMA, DMA_Channel0 }
#define DEFAULT_ADC_CONFIG              { ADC_INPUT_SRC_IO0, ADC_INPUT_SRC_IO0, ADC_INPUT_SINGLE, ADC_REF_SRC_BUF_INT, ADC_REF_VALUE_1P6, ADC_CLK_1P6M }
#define DEFAULT_PARAM_CONFIG            { DEFAULT_IO_CONFIG, DEFAULT_MODE_CONFIG, DEFAULT_ADC_CONFIG }


void ADC_Config_Init(void);
double Get_adc_Voltage(uint8_t Get_channel);
#endif
