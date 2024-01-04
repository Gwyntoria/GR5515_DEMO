/**
 *****************************************************************************************
 *
 * @file main.c
 *
 * @brief main function Implementation.
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "app_io.h"
#include "app_log.h"
#include "app_timer.h"
#include "custom_config.h"
#include "flash_scatter_config.h"
#include "gr55xx_dfu.h"
#include "gr55xx_pwr.h"
#include "gr55xx_sys.h"
#include "patch.h"
#include "scatter_common.h"
#include "user_app.h"
#include "user_periph_setup.h"

#include "gh3x2x_demo.h"
#include "gh3x2x_demo_config.h"

#include "user_common.h"
#include "user_func_ctrl.h"
#include "user_rtc.h"


/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
extern gap_cb_fun_t         app_gap_callbacks;
extern gatt_common_cb_fun_t app_gatt_common_callback;
extern gattc_cb_fun_t       app_gattc_callback;
extern sec_cb_fun_t         app_sec_callback;

extern GU8 g_uchGh3x2xIntCallBackIsCalled;

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

/**@brief Stack global variables for Bluetooth protocol stack. */
STACK_HEAP_INIT(heaps_table);

static app_callback_t s_app_ble_callback = {
    .app_ble_init_cmp_callback = ble_init_cmp_callback,
    .app_gap_callbacks         = &app_gap_callbacks,
    .app_gatt_common_callback  = &app_gatt_common_callback,
    .app_gattc_callback        = &app_gattc_callback,
    .app_sec_callback          = &app_sec_callback,
};

#define VER_MAJOR 0
#define VER_MINOR 1
#define VER_BUILD 1

static char APP_VERSION[16];

int main(void)
{
    // Initialize user peripherals.
    app_periph_init();

    // Initialize ble stack.
    ble_stack_init(&s_app_ble_callback, &heaps_table); /*< init ble stack*/

    sprintf(APP_VERSION, "%d.%d.%d", VER_MAJOR, VER_MINOR, VER_BUILD);
    APP_LOG_INFO("App Version: %s\r\n", APP_VERSION);

    Gh3x2xDemoInit();
    rtc_init();
    // func_ctrl_init();

    // Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN);
    // Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);

    // rtc_set_tick_alarm(MIN_TO_MS * 1 + 500);

    while (1) {
        if (g_uchGh3x2xIntCallBackIsCalled) {
            Gh3x2xDemoInterruptProcess();
        }

        app_log_flush();     // 刷新log缓存
        pwr_mgmt_schedule(); // 电源管理调度，负责管理查询是否可以进入睡眠
        dfu_schedule();
    }
}
