#ifndef _I2C_CONFIG_H_
#define _I2C_CONFIG_H_

#include "stdint.h"



/** i2c scl/sda config define*/
#define I2C0_SCL_PIN			APP_IO_PIN_10
#define I2C0_SDA_PIN			APP_IO_PIN_11
#define I2C0_IO_MUX				APP_IO_MUX_4
#define I2C0_ID                 APP_I2C_ID_0

#define I2C1_SCL_PIN			APP_IO_PIN_30
#define I2C1_SDA_PIN			APP_IO_PIN_26
#define I2C1_IO_MUX				APP_IO_MUX_0
#define I2C1_ID                 APP_I2C_ID_1


#define I2C0_OWN_ADDR			0xA1
#define I2C1_OWN_ADDR			0xA2


#define Nst_I2C_ID				APP_I2C_ID_1
#define LSM_I2C_ID				APP_I2C_ID_0
//#define ST25DV_I2C_ID			APP_I2C_ID_0
void GT_I2C_Init(void);

#endif
