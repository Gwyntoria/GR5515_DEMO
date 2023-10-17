/*************************************************************************************
 * Description  :
 * Version      : 1.0
 * Author       : XiaoPb
 * Date         : 2022-06-15 16:28:23
 * LastEditors  : XiaoPb
 * LastEditTime : 2022-06-15 16:48:45
 * FilePath     :
 * /GR551x_V1_7_0_d9e44f51/projects/ble/ble_peripheral/ble_app_gh3x2x_v4002_public/Src/gh3x2x_code/demo_kernel_code/inc/gh3x2x_demo_io_config.h
 * Copyright (C) 2022 XiaoPb. All rights reserved.
 *************************************************************************************/

#ifndef __GH3X2X_DEMO_IO_CONFIG_H__
#define __GH3X2X_DEMO_IO_CONFIG_H__

#include "gh3x2x_demo_config.h"
#include "gh3x2x_drv.h"
#include "stdint.h"
#include "string.h"

#include "app_gpiote.h"
#include "app_log.h"
#include "app_timer.h"
#include "gr55xx_sys.h"

#define USE_GH3x2x_VIO_PIN (0) // power io

#if __SUPPORT_HARD_RESET_CONFIG__
#define GH3x2x_RST_PIN  APP_IO_PIN_2
#define GH3x2x_RST_PORT APP_IO_TYPE_NORMAL
#endif

#define GH3x2x_INT_PIN  APP_IO_PIN_3
#define GH3x2x_INT_PORT APP_IO_TYPE_NORMAL

#if (GH3x2x_INT_PORT == APP_IO_TYPE_AON)
#define GH3x2x_INT_WAKEUP APP_IO_ENABLE_WAKEUP
#else
#define GH3x2x_INT_WAKEUP APP_IO_NONE_WAKEUP
#endif

#if USE_GH3x2x_VIO_PIN // power io
#define GH3x2x_VIO_PIN  APP_IO_PIN_7
#define GH3x2x_VIO_PORT APP_IO_TYPE_NORMAL

#define GH3x2x_VIO_OPEN  APP_IO_PIN_SET
#define GH3x2x_VIO_CLOSE APP_IO_PIN_RESET
#endif

#if (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__)
#include "app_spi.h"

#define GH3x2x_CS_PIN  APP_IO_PIN_8
#define GH3x2x_CS_PORT APP_IO_TYPE_NORMAL
#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__)
#define GH3x2x_CS_MUX APP_IO_MUX_7
#define GH3x2x_CS_EN  APP_SPI_PIN_DISABLE
#else
#define GH3x2x_CS_MUX APP_IO_MUX_1
#define GH3x2x_CS_EN  APP_SPI_PIN_ENABLE
#endif

#define GH3x2x_CLK_PIN  APP_IO_PIN_7
#define GH3x2x_CLK_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_CLK_MUX  APP_IO_MUX_4

#define GH3x2x_SI_PIN  APP_IO_PIN_6
#define GH3x2x_SI_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_SI_MUX  APP_IO_MUX_4

#define GH3x2x_SO_PIN  APP_IO_PIN_5
#define GH3x2x_SO_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_SO_MUX  APP_IO_MUX_4

#else // (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__)

#include "app_i2c.h"
#define MASTER_DEV_ADDR (0x38)
#define GH3x2x_I2C_ID   APP_I2C_ID_1

#define GH3x2x_SCL_PIN  APP_IO_PIN_9
#define GH3x2x_SCL_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_SCL_MUX  APP_IO_MUX_1

#define GH3x2x_SDA_PIN  APP_IO_PIN_8
#define GH3x2x_SDA_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_SDA_MUX  APP_IO_MUX_1
#endif

#if (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__)
/*
 * cs
 * clk
 * si
 * so
 */
#define DEFAULT_IO_CONFIG                                                                           \
    {                                                                                               \
        {GH3x2x_CS_PORT, GH3x2x_CS_MUX, GH3x2x_CS_PIN, APP_IO_NOPULL, GH3x2x_CS_EN},                \
        {GH3x2x_CLK_PORT, GH3x2x_CLK_MUX, GH3x2x_CLK_PIN, APP_IO_NOPULL, APP_SPI_PIN_ENABLE},       \
        {GH3x2x_SI_PORT, GH3x2x_SI_MUX, GH3x2x_SI_PIN, APP_IO_NOPULL, APP_SPI_PIN_ENABLE},          \
        {GH3x2x_SO_PORT, GH3x2x_SO_MUX, GH3x2x_SO_PIN, APP_IO_NOPULL, APP_SPI_PIN_ENABLE}                                                                                         \
    }

#define DEFAULT_MODE_CONFIG                          \
    {                                                \
        APP_SPI_TYPE_DMA, DMA_Channel0, DMA_Channel1 \
    }
#define DEFAULT_SPI_CONFIG                                                                                     \
    {                                                                                                          \
        SPI_DATASIZE_8BIT, SPI_POLARITY_LOW, SPI_PHASE_1EDGE, (SystemCoreClock / 4000000), SPI_TIMODE_DISABLE, \
            SPI_SLAVE_SELECT_0                                                                                 \
    }
#define DEFAULT_PARAM_CONFIG                                                          \
    {                                                                                 \
        APP_SPI_ID_MASTER, DEFAULT_IO_CONFIG, DEFAULT_MODE_CONFIG, DEFAULT_SPI_CONFIG \
    }
#else // (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__)

#define DEFAULT_I2C_IO_CONFIG                                              \
    {                                                                      \
        {GH3x2x_SCL_PORT, GH3x2x_SCL_MUX, GH3x2x_SCL_PIN, APP_IO_PULLUP},  \
        {                                                                  \
            GH3x2x_SDA_PORT, GH3x2x_SDA_MUX, GH3x2x_SDA_PIN, APP_IO_PULLUP \
        }                                                                  \
    }
#define DEFAULT_I2C_MODE_CONFIG                      \
    {                                                \
        APP_I2C_TYPE_DMA, DMA_Channel2, DMA_Channel3 \
    }
#define DEFAULT_I2C_CONFIG                                                                 \
    {                                                                                      \
        I2C_SPEED_1000K, MASTER_DEV_ADDR, I2C_ADDRESSINGMODE_7BIT, I2C_GENERALCALL_DISABLE \
    }
#define DEFAULT_I2C_PARAM_CONFIG                                                                               \
    {                                                                                                          \
        GH3x2x_I2C_ID, APP_I2C_ROLE_MASTER, DEFAULT_I2C_IO_CONFIG, DEFAULT_I2C_MODE_CONFIG, DEFAULT_I2C_CONFIG \
    }
#endif

#endif // !__GH3X2X_DEMO_IO_CONFIG_H__
