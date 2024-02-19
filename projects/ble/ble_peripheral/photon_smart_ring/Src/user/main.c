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
#include "user_timer.h"

#include "PowerSample/ADC.h"
#include "ST25DV04K/ST25DV.h"
#include "I2C_Config/I2C_Config.h"
#include "GD25LE128E/Flash_Spi.h"
#include "LSM6DSO/LSM6DSOWTR.h"
#include "NST112/nst112x.h"

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

static char APP_VERSION[16];

int main(void) {
    // NFC tag config
    uint8_t  text_arry[22] = {0xE1, 0x40, 0x40, 0x01, 0x03, 0x0F, 0xD1, 0x06,
                              0x06, 0x57, 0x4C, 0x43, 0x43, 0x41, 0x50, WLCCAP_NDEF_PROTOCOL_VERSION,
                              0x57, 0x07, 0x1E, 0x03, 0x0A, 0xFE};
    double   Power1 = 0.0, Power2 = 0.0;
    double   Temperature[4]           = {0.0, 0.0, 0.0, 0.0};
    uint16_t LSM6D_Temp               = 0;
    uint16_t LSM6D_Data[6]            = {0};
    uint32_t Flash_Address            = 0x0;
    uint8_t  Flash_Write_Data_Arry[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t  Flash_Read_Data_Arry[8]  = {0};
    uint8_t  i                        = 0;

    APP_LOG_INFO("App Version: %s\r\n", APP_VERSION);
    sprintf(APP_VERSION, "%d.%d.%d", VER_MAJOR, VER_MINOR, VER_BUILD);

    // Initialize user peripherals.
    app_periph_init();

    // Initialize ble stack.
    ble_stack_init(&s_app_ble_callback, &heaps_table); /*< init ble stack*/

    GT_I2C_Init();      // I2C init （必须最早初始化）
    Nst112_four_Init(); // 测温芯片初始化
    LSM6DSOWTR_Init();  // 六轴初始化
    ADC_Config_Init();  // 电池测量初始化
    Flash_Config();     // FLASH Init
    ST25DV_ReadUid();   // NFC 读取 Uid
    
    Gh3x2xDemoInit();   // Gh3x2x init
    user_rtc_init();    // RTC init
    func_ctrl_init();   // 功能控制模块初始化

    // // flash sample
    // Flash_Read_Data(Flash_Address, Flash_Read_Data_Arry, 8);
    // printf("Flash init Read Data is:\r\n");
    // for (i = 0; i < 8; i++) {
    //     printf("Flash_Read_Data_Arry[%d] = %d\r\n", i, Flash_Read_Data_Arry[i]);
    // }

    // Flash_Write_Data(Flash_Address, Flash_Write_Data_Arry, 8);
    // Flash_Read_Data(Flash_Address, Flash_Read_Data_Arry, 8);
    // printf("Flash Write after Read Data is:\r\n");
    // for (i = 0; i < 8; i++) {
    //     printf("Flash_Read_Data_Arry[%d] = %d\r\n", i, Flash_Read_Data_Arry[i]);
    // }

    // Flash_Sector_Erase(0);
    // Flash_Read_Data(Flash_Address, Flash_Read_Data_Arry, 8);
    // printf("Flash Erase after Read Data is:\r\n");
    // for (i = 0; i < 8; i++) {
    //     printf("Flash_Read_Data_Arry[%d] = %d\r\n", i, Flash_Read_Data_Arry[i]);
    // }

    // //	ST25DV_ReadUid();
    // ST25DV_WriteData(0, text_arry, sizeof(text_arry)); // NFC write data
    // delay_ms(250);

    // func_ctrl_start(kFuncOptAdt);


    while (1) {

        if (g_uchGh3x2xIntCallBackIsCalled) {
            Gh3x2xDemoInterruptProcess();
        }

        func_ctrl_handler();

        // Temperature[0] = Get_nst112x_temperature(Nst112A_I2C_Addr);
        // Temperature[1] = Get_nst112x_temperature(Nst112B_I2C_Addr);
        // Temperature[2] = Get_nst112x_temperature(Nst112C_I2C_Addr);
        // Temperature[3] = Get_nst112x_temperature(Nst112D_I2C_Addr);
        // printf("Temperature0 = %.2f  Temperature1 = %.2f  Temperature2 = %.2f  Temperature3 = %.2f\r\n",
        //        Temperature[0],
        //        Temperature[1],
        //        Temperature[2],
        //        Temperature[3]);

        // // 电池 ADC
        // Power1 = Get_adc_Voltage(0); // 获取电池电压
        // printf("Now VABAT Power is %.2f\r\n", Power1);
        // Power2 = Get_adc_Voltage(1); // 获取充电时的电压
        // printf("Now BATMS Power is %.2f\r\n", Power2);

        // // 六轴
        // LSM6D_Temp = LSM6DSOWTR_Read_Data(LSM6DSO_OUT_TEMP_L); // 获取温度
        // printf("Now LSM6D_Temp is %d\r\n", LSM6D_Temp);
        // LSM6D_Data[0] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTX_L_G);
        // printf("Now LSM6DSO_OUTX_L_G is %d\r\n", LSM6D_Data[0]);
        // LSM6D_Data[1] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTY_L_G);
        // printf("Now LSM6DSO_OUTY_L_G is %d\r\n", LSM6D_Data[1]);
        // LSM6D_Data[2] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTZ_L_G);
        // printf("Now LSM6DSO_OUTZ_L_G is %d\r\n", LSM6D_Data[2]);
        // LSM6D_Data[3] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTX_L_A);
        // printf("Now LSM6DSO_OUTX_L_A is %d\r\n", LSM6D_Data[3]);
        // LSM6D_Data[4] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTY_L_A);
        // printf("Now LSM6DSO_OUTY_L_A is %d\r\n", LSM6D_Data[4]);
        // LSM6D_Data[5] = LSM6DSOWTR_Read_Data(LSM6DSO_OUTZ_L_A);
        // printf("Now LSM6DSO_OUTZ_L_A is %d\r\n", LSM6D_Data[5]);



        app_log_flush();     // 刷新log缓存
        pwr_mgmt_schedule(); // 电源管理调度，负责管理查询是否可以进入睡眠
        dfu_schedule();

        delay_ms(100);
    }
}
