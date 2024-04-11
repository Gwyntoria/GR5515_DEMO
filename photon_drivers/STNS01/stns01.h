#ifndef STNS01_H
#define STNS01_H

#include <stdint.h>
#include <string.h>

#define ADC_CONV_LENGTH (64UL)
#define ADC_CONV_IGNORE (5UL)

#define STNS01_CHANNEL_CHARGING (0) // BATMS
#define STNS01_CHANNEL_BATTERY  (1) // VABAT

void   stns01_init(void);
double stns01_get_voltage(uint8_t channel);

void stns01_test(void);

#endif
