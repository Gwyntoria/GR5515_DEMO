#ifndef _I2C_CONFIG_H_
#define _I2C_CONFIG_H_

#include "stdint.h"

/* i2c scl/sda config define*/

// 六轴
#define LSM_SCL_PIN APP_IO_PIN_10
#define LSM_SDA_PIN APP_IO_PIN_11
#define LSM_IO_MUX  APP_IO_MUX_4
#define LSM_I2C_ID  APP_I2C_ID_0

// 温度
#define NST_SCL_PIN APP_IO_PIN_30
#define NST_SDA_PIN APP_IO_PIN_26
#define NST_IO_MUX  APP_IO_MUX_0
#define NST_I2C_ID  APP_I2C_ID_1

#define LSM_OWN_ADDR 0xA1
#define NST_OWN_ADDR 0xA2

void GT_I2C_Init(void);

#endif
