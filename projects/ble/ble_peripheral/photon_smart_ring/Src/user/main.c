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
#include "gh3x2x_demo.h"
#include "gh3x2x_demo_config.h"
#include "gr55xx_dfu.h"
#include "gr55xx_pwr.h"
#include "gr55xx_sys.h"
#include "patch.h"
#include "scatter_common.h"
#include "user_app.h"
#include "user_periph_setup.h"

#include "GD25LE128E/Flash_Spi.h"
#include "NST112/nst112x.h"
#include "STNS01/stns01.h"

#include "user_common.h"
#include "user_data_center.h"
#include "user_file_sys.h"
#include "user_func_ctrl.h"
#include "user_hrv.h"
#include "user_i2c.h"
#include "user_lsm6dso.h"
#include "user_nfc.h"
#include "user_rtc.h"
#include "user_timer.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
extern gap_cb_fun_t         app_gap_callbacks;
extern gatt_common_cb_fun_t app_gatt_common_callback;
extern gattc_cb_fun_t       app_gattc_callback;
extern sec_cb_fun_t         app_sec_callback;

extern GU8 g_uchGh3x2xIntCallBackIsCalled;

char APP_VERSION[32];

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

int main(void) {
    memset(APP_VERSION, 0, sizeof(APP_VERSION));
    sprintf(APP_VERSION, "%u.%u.%u.%lu", VER_MAJOR, VER_MINOR, VER_BUILD, VER_EXTEN);

    user_rtc_init();   // RTC init
    app_periph_init(); // Initialize user peripherals.

    APP_LOG_INFO("Firmware Version: %s", APP_VERSION);

    ble_stack_init(&s_app_ble_callback, &heaps_table); // Initialize ble stack.

    user_i2c_init(); // I2C init

    //* i2c
    lsm6dso_init(); // 六轴 LSM6DSO
    nst112_init();  // 测温芯片初始化
    user_nfc_init(); // NFC tag 初始化

    //* spi
    // ufs_reinit();
    ufs_init();       // 文件系统初始化
    Gh3x2xDemoInit(); // Gh3x2x init

    //* adc
    stns01_init(); // 电池测量初始化

    func_ctrl_init(); // 功能控制模块初始化
    udc_init();       // 数据管理模块初始化
    user_hrv_init();  // HRV模块初始化

    // ufs_test(); // 文件系统测试
    // udc_test(); // 数据管理模块测试
    // nst112x_test(); // 温度读取test
    // lsm6dso_test(); // 6轴传感器读取test
    // stns01_test();  // 电量读取test
    // user_nfc_test(); // NFC测试

    // delay_ms(500);

    // flash_func_test();

    lsm6dso_start_timer(); // 启动6轴传感器定时器

    while (1) {

        if (g_uchGh3x2xIntCallBackIsCalled) {
            Gh3x2xDemoInterruptProcess();
        }

        // calculate_interval_from_previous("main");

        func_ctrl_handler(); // 功能控制模块处理
        // func_ctrl_test(); // 功能控制模块测试

        // lsm6dso_get_event_status();

        app_log_flush(); // 刷新log缓存
        app_log_store_schedule();
        pwr_mgmt_schedule(); // 电源管理调度，负责管理查询是否可以进入睡眠
        dfu_schedule();

        delay_ms(1);
    }
}
