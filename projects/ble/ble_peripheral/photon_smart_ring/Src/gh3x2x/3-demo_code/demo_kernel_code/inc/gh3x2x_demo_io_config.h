/**
 * @file gh3x2x_demo_io_config.h
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef GH3X2X_DEMO_IO_CONFIG_H
#define GH3X2X_DEMO_IO_CONFIG_H

#include "stdint.h"
#include "string.h"

#include "app_gpiote.h"
#include "app_log.h"
#include "app_spi.h"
#include "app_timer.h"
#include "gr55xx_sys.h"

#include "gh3x2x_demo_config.h"
#include "gh3x2x_drv.h"

#define USE_GH3x2x_VIO_PIN (0) // power io

#if __SUPPORT_HARD_RESET_CONFIG__
#define GH3x2x_RST_PIN  APP_IO_PIN_2
#define GH3x2x_RST_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_RST_MUX  APP_IO_MUX_7
#endif

#define GH3x2x_INT_PIN  APP_IO_PIN_3
#define GH3x2x_INT_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_INT_MUX  APP_IO_MUX_7

#define GH3x2x_CS_PIN  APP_IO_PIN_8
#define GH3x2x_CS_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_CS_MUX  APP_IO_MUX_7

#define GH3x2x_CLK_PIN  APP_IO_PIN_7
#define GH3x2x_CLK_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_CLK_MUX  APP_IO_MUX_4

#define GH3x2x_SI_PIN  APP_IO_PIN_6
#define GH3x2x_SI_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_SI_MUX  APP_IO_MUX_4

#define GH3x2x_SO_PIN  APP_IO_PIN_5
#define GH3x2x_SO_PORT APP_IO_TYPE_NORMAL
#define GH3x2x_SO_MUX  APP_IO_MUX_4

#endif // *GH3X2X_DEMO_IO_CONFIG_H