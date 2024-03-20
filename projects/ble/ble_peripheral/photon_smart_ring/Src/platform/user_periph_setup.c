/**
 *****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief  User Periph Init Function Implementation.
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
#include "user_periph_setup.h"

#include "app_assert.h"
#include "app_log.h"
#include "app_log_store.h"
#include "app_pwr_mgmt.h"
#include "app_uart.h"
#include "bsp.h"
#include "custom_config.h"
#include "fault_trace.h"
#include "GR5515_SK.h"
#include "gr55xx_delay.h"
#include "gr55xx_pwr.h"
#include "gr55xx_sys.h"
#include "hal_flash.h"

#include "gh3x2x_demo.h"
#include "gh3x2x_demo_config.h"

#include "user_rtc.h"

// static const uint8_t  s_bd_addr[SYS_BD_ADDR_LEN] = {0x0f, 0x00, 0xcf, 0x3e, 0xcb, 0xea};

static void app_log_assert_init(void)
{
    bsp_log_init();
    fault_trace_db_init();

    app_log_store_info_t store_info;
    app_log_store_op_t   op_func;

    store_info.nv_tag   = 0x40ff;
    store_info.db_addr  = 0x01096000;
    store_info.db_size  = 0x20000; // 128 KB
    store_info.blk_size = 0x1000;

    op_func.flash_init  = hal_flash_init;
    op_func.flash_erase = hal_flash_erase;
    op_func.flash_write = hal_flash_write;
    op_func.flash_read  = hal_flash_read;
    // op_func.time_get    = rtc_get_log_real_time;
    op_func.time_get    = NULL;
    op_func.sem_give    = NULL;
    op_func.sem_take    = NULL;

    app_log_store_init(&store_info, &op_func);
}

uint8_t r_data[50];
void app_uart_evt_handler(app_uart_evt_t *p_evt)
{
    switch(p_evt->type)
    {
        case APP_UART_EVT_ERROR:
            
            app_uart_receive_async(APP_UART_ID, r_data, 50);    
            break;
        case APP_UART_EVT_RX_DATA:

            if (memcmp(r_data, "[SPO2]", 6) == 0) {
                Gh3x2xDemoStopSampling(0xFFFFFFFF);
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SPO2);
                APP_LOG_INFO("GH3X2X_FUNCTION_SPO2");
            } else if (memcmp(r_data, "[HR]", 4) == 0) {
                Gh3x2xDemoStopSampling(0xFFFFFFFF);
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
                APP_LOG_INFO("GH3X2X_FUNCTION_HR");
            } else if (memcmp(r_data, "[ECG]", 5) == 0) {
                Gh3x2xDemoStopSampling(0xFFFFFFFF);
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_LEAD_DET);
                APP_LOG_INFO("GH3X2X_FUNCTION_ECG");
            } else if (memcmp(r_data, "[ADT]", 5) == 0) {
                Gh3x2xDemoStopSampling(0xFFFFFFFF);
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ADT);
                APP_LOG_INFO("GH3X2X_FUNCTION_ADT");
            } else if (memcmp(r_data, "[HRV]", 5) == 0) {
                Gh3x2xDemoStopSampling(0xFFFFFFFF);
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HRV);
                APP_LOG_INFO("GH3X2X_FUNCTION_HRV");
            }

            app_uart_receive_async(APP_UART_ID, r_data, 50);    
            break;
        default:
            break;
    }
}

void app_periph_init()
{
    // SYS_SET_BD_ADDR(s_bd_addr);

    app_log_assert_init(); // log初始化
    app_uart_receive_async(APP_UART_ID, r_data, 50);
    pwr_mgmt_mode_set(PMR_MGMT_ACTIVE_MODE); // 设置睡眠模式
}
