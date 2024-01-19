/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_user.c
 * 
 * @brief   gh3x2x driver lib demo code that user defined
 * 
 * @author  
 * 
 */

/* includes */
#include "stdint.h"
#include "string.h"
#include "gh3x2x_drv.h"
#include "gh3x2x_demo_config.h"
#include "gh3x2x_demo_inner.h"
#include "gh3x2x_demo.h"

#include "gh3x2x_demo_io_config.h"

#include "app_error.h"
#include "app_log.h"
app_timer_id_t gh3x2x_soft_adt_timer_id;
app_timer_id_t gh3x2x_serial_timer_id;
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#include "gh3x2x_demo_algo_call.h"
#endif

#if (__GH3X2X_MP_MODE__)
#include "gh3x2x_mp_common.h"
#endif

#if ( __GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__ )

/* i2c interface */
/**
 * @fn     void hal_gh3x2x_i2c_init(void)
 * 
 * @brief  hal i2c init for gh3x2x
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_i2c_init(void)
{
	app_i2c_params_t params_t = DEFAULT_I2C_PARAM_CONFIG;

	params_t.use_mode.type = APP_I2C_TYPE_POLLING;
	if (app_i2c_init(&params_t, NULL) != 0)
	{
		EXAMPLE_LOG("i2c initial failed! Please check the input paraments.\r\n");
		return ;
    }
}

/**
 * @fn     uint8_t hal_gh3x2x_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length)
 * 
 * @brief  hal i2c write for gh3x2x
 *
 * @attention   device_id is 8bits, if platform i2c use 7bits addr, should use (device_id >> 1)
 *
 * @param[in]   device_id       device addr
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length)
{
    uint8_t ret = 1;
#if 0
    for(GU16 i = 0; i < length; i++)
    {
        EXAMPLE_LOG("%02X ", write_buffer[i]);
    }
    EXAMPLE_LOG("\r\n");
#endif
    /* code implement by user */
    ret = app_i2c_transmit_sync(GH3x2x_I2C_ID, device_id >> 1, (uint8_t *) write_buffer, length, 1000);
	if (ret)
	{
		EXAMPLE_LOG("i2c(%X) transmit failed(%d)! \r\n",device_id,ret);
		return 0;
	}
    return ret;
}

/**
 * @fn     uint8_t hal_gh3x2x_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length,
 *                            uint8_t read_buffer[], uint16_t read_length)
 * 
 * @brief  hal i2c read for gh3x2x
 *
 * @attention   device_id is 8bits, if platform i2c use 7bits addr, should use (device_id >> 1)
 *
 * @param[in]   device_id       device addr
 * @param[in]   write_buffer    write data buffer
 * @param[in]   write_length    write data len
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length)
{
    uint8_t ret = 1;

#if 0
    EXAMPLE_LOG("%s: ",__FUNCTION__);
    for(GU16 i = 0; i < write_length; i++)
    {
        EXAMPLE_LOG("%02X ", write_buffer[i]);
    }
//    EXAMPLE_LOG("\r\n");
#endif   
    /* code implement by user */
	uint16_t i2c_addr = device_id >> 1;
    ret = app_i2c_transmit_sync(GH3x2x_I2C_ID, i2c_addr, (uint8_t *) write_buffer, write_length, 1000);
	if (ret)
	{
		EXAMPLE_LOG("i2c(%X) transmit failed(%d)! \r\n",device_id,ret);
		return 0;
	}
    ret = app_i2c_receive_sync(GH3x2x_I2C_ID, i2c_addr, read_buffer, read_length, 100);
	if (ret)
	{
		EXAMPLE_LOG("i2c(%X) receive failed(%d)! \r\n",device_id,ret);
		return 0;
	}

#if 0
//    EXAMPLE_LOG("%s: ",__FUNCTION__);
    for(GU16 i = 0; i < read_length; i++)
    {
        EXAMPLE_LOG("%02X ", read_buffer[i]);
    }
    EXAMPLE_LOG("\r\n");
#endif  
    return ret;
}

#else // __GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__

/* spi interface */
volatile uint8_t spi_master_tdone = 0;
volatile uint8_t spi_master_rdone = 0;

    
void app_spi_callback(app_spi_evt_t *p_evt)
{
    if (p_evt->type == APP_SPI_EVT_TX_CPLT)
    {
        spi_master_tdone = 1;
    }
    if (p_evt->type == APP_SPI_EVT_RX_DATA)
    {
        spi_master_rdone = 1;
    }
    if (p_evt->type == APP_SPI_EVT_ERROR)
    {
        spi_master_tdone = 1;
        spi_master_rdone = 1;
    }
}
/**
 * @fn     void hal_gh3x2x_spi_init(void)
 * 
 * @brief  hal spi init for gh3x2x
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

void hal_gh3x2x_spi_init(void)
{
    /* init spi and cs pin */
    uint16_t ret;
    app_spi_params_t spi_params = DEFAULT_PARAM_CONFIG;
    spi_params.use_mode.type = APP_SPI_TYPE_DMA;
    
    ret = app_spi_init(&spi_params, app_spi_callback);
    if (ret != 0)
    {
        EXAMPLE_LOG("SPIM initial failed! Please check the input paraments. ret = %d.\r\n", ret);
}

#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__)
    app_io_init_t gpio_config = APP_IO_DEFAULT_CONFIG;
    gpio_config.pin = GH3x2x_CS_PIN;
    gpio_config.mode = APP_IO_MODE_OUT_PUT;
    gpio_config.pull = APP_IO_PULLUP;
    gpio_config.mux = APP_IO_MUX_7;
    app_io_init(GH3x2x_CS_PORT, &gpio_config);
    app_io_write_pin(GH3x2x_CS_PORT, GH3x2x_CS_PIN, APP_IO_PIN_SET);
#endif
}

/**
 * @fn     GU8 hal_gh3x2x_spi_write(GU8 write_buffer[], GU16 length)
 * 
 * @brief  hal spi write for gh3x2x
 *
 * @attention   if __GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__  , user need generate timming: write_buf[0](W) + write_buf[1](W) + ...
 * @attention   if __GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_HARDWARE_CS__  , user need generate timming: CS LOW  + write_buf[0](W) + write_buf[1](W) + ... + CS HIGH
 *
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_write(GU8 write_buffer[], GU16 length)
{
    spi_master_tdone = 0;
    GU16 err = app_spi_transmit_async(APP_SPI_ID_MASTER, write_buffer, length);
    if (err)
    {
        EXAMPLE_LOG("spi write error. ret = %d.\r\n", err);
        return 0;
    }
    while (!spi_master_tdone);
#if 0
    for(GU16 i = 0; i < length; i++)
    {
        EXAMPLE_LOG("%02X ", write_buffer[i]);
    }
    EXAMPLE_LOG("\r\n");
#endif
    return 1;
}


#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__) 
/**
 * @fn     GU8 hal_gh3x2x_spi_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi read for gh3x2x
 *
 * @attention   user need generate timming: read_buf[0](R) + write_buf[1](R) + ... 
 *
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_read(GU8 read_buffer[], GU16 length)
{
    spi_master_rdone = 0;

    GU16 err = app_spi_receive_async(APP_SPI_ID_MASTER, read_buffer, length);
    if (err) {
        EXAMPLE_LOG("spi read error, err = %d \r\n", err);
        return 0;
    }

    while (!spi_master_rdone)
        ;

    return 1;
}

#elif (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_HARDWARE_CS__)
/**
 * @fn     GU8 hal_gh3x2x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi write F1 and read for gh3x2x
 *
 * @attention    user need generate timming: CS LOW + F1(W) + read_buf[0](R) + read_buf[1](R) + ... + CS HIGH
 *
 * @param[in]   write_buf     write data
 * @param[in]   length     write data len
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
{
    GU8 write_buffer[1] = {0xF1};
    spi_master_rdone = 0;
    spi_master_tdone = 0;
    GU16 err = app_spi_transmit_async(APP_SPI_ID_MASTER, write_buffer, 1);
    err = app_spi_receive_async(APP_SPI_ID_MASTER, read_buffer, length);
    if (err){
        EXAMPLE_LOG("spi read error, err = %d \r\n", err);
        return 0;
    }
    while (!spi_master_tdone);
    while (!spi_master_rdone);
    return 1;
}
#endif

/**
 * @fn     void hal_gh3x2x_spi_cs_ctrl(GU8 cs_pin_level)
 * 
 * @brief  hal spi cs pin ctrl for gh3x2x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   cs_pin_level     spi cs pin level
 * @param[out]  None
 *
 * @return  None
 */
#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__) 
void hal_gh3x2x_spi_cs_ctrl(GU8 cs_pin_level)
{
    app_io_write_pin(GH3x2x_CS_PORT, GH3x2x_CS_PIN, (cs_pin_level) ? (APP_IO_PIN_SET) : (APP_IO_PIN_RESET));
}
#endif

#endif

#if __SUPPORT_HARD_RESET_CONFIG__

/**
 * @fn     void hal_gh3x2x_int_init(void)
 * 
 * @brief  gh3x2x int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */



void hal_gh3x2x_reset_pin_init(void)
{
    app_io_init_t gpio_config = APP_IO_DEFAULT_CONFIG;
#if USE_GH3x2x_VIO_PIN    
    gpio_config.pin = GH3x2x_VIO_PIN;
    gpio_config.mode = APP_IO_MODE_OUT_PUT;
    gpio_config.pull = APP_IO_PULLUP;
    gpio_config.mux = APP_IO_MUX_7;
    app_io_init(GH3x2x_VIO_PORT, &gpio_config);
    app_io_write_pin(GH3x2x_VIO_PORT, GH3x2x_VIO_PIN, GH3x2x_VIO_OPEN);
#endif    
    gpio_config.pin = GH3x2x_RST_PIN;
    gpio_config.mode = APP_IO_MODE_OUT_PUT;
    gpio_config.pull = APP_IO_PULLUP;
    gpio_config.mux = APP_IO_MUX_7;
    app_io_init(GH3x2x_RST_PORT, &gpio_config);

    app_io_write_pin(GH3x2x_RST_PORT, GH3x2x_RST_PIN, APP_IO_PIN_RESET);
    sys_delay_ms(5);
    app_io_write_pin(GH3x2x_RST_PORT, GH3x2x_RST_PIN, APP_IO_PIN_SET);
}
/**
 * @fn     void hal_gh3x2x_reset_pin_ctrl(GU8 pin_level)
 * 
 * @brief  hal reset pin ctrl for gh3x2x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   pin_level     reset pin level
 * @param[out]  None
 *
 * @return  None
 */

void hal_gh3x2x_reset_pin_ctrl(GU8 pin_level)
{
    app_io_write_pin(GH3x2x_RST_PORT, GH3x2x_RST_PIN, (pin_level) ? (APP_IO_PIN_SET) : (APP_IO_PIN_RESET));
}

#endif

void gh3x2x_int_callback(app_gpiote_evt_t *p_evt)
{
//    app_io_pin_state_t pin_level = APP_IO_PIN_RESET;

    switch (p_evt->type)
    {
    case GH3x2x_INT_PORT:
        if ((p_evt->pin & APP_IO_PIN_ALL) == GH3x2x_INT_PIN)
        {
            //printf("hal_gh3x2x_int_handler_call_back start\r\n");
//            pin_level = app_io_read_pin(GH3x2x_INT_PORT, GH3x2x_INT_PIN);
//            if (pin_level == APP_IO_PIN_SET)
            {
                
                hal_gh3x2x_int_handler_call_back();
                //printf("hal_gh3x2x_int_handler_call_back end\r\n");
            }
        }
        break;

    default:
        sys_delay_ms(1);
        break;
    }
}
/**
 * @fn     void hal_gh3x2x_int_init(void)
 * 
 * @brief  gh3x2x int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_int_init(void)
{
    const app_gpiote_param_t param[] =
        {
            {GH3x2x_INT_PORT, GH3x2x_INT_PIN, APP_IO_MODE_IT_RISING, APP_IO_PULLDOWN, GH3x2x_INT_WAKEUP, gh3x2x_int_callback},
        };
    app_gpiote_init(param, sizeof(param) / sizeof(app_gpiote_param_t));
}

#if (__NORMAL_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__ || __MIX_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__)
/**
 * @fn     void hal_gh3x2x_int_handler_call_back(void)
 * 
 * @brief  call back of gh3x2x int handler
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_int_handler_call_back(void)
{
    if (Gh3x2xGetInterruptMode() == __NORMAL_INT_PROCESS_MODE__)
    {
        GH3X2X_ClearChipSleepFlag(0);
        g_uchGh3x2xIntCallBackIsCalled = 1;
#if (__GH3X2X_MP_MODE__)
        GH3X2X_MP_SET_INT_FLAG();  //gh3x2x mp test must call it
#endif
        GOODIX_PLANFROM_INT_HANDLER_CALL_BACK_ENTITY();
    }
}
#endif

/**
 * @fn     void hal_gsensor_start_cache_data(void)
 * 
 * @brief  Start cache gsensor data for gh3x2x
 *
 * @attention   This function will be called when start gh3x2x sampling.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gsensor_start_cache_data(void)
{
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    GH3X2X_TimestampSyncAccInit();
#endif
    GOODIX_PLANFROM_INT_GS_START_CACHE_ENTITY();
}

/**
 * @fn     void hal_gsensor_stop_cache_data(void)
 * 
 * @brief  Stop cache gsensor data for gh3x2x
 *
 * @attention   This function will be called when stop gh3x2x sampling.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gsensor_stop_cache_data(void)
{
        
    GOODIX_PLANFROM_INT_GS_STOP_CACHE_ENTITY();
}

void hal_cap_start_cache_data(void)
{
    GOODIX_PLANFROM_INT_CAP_START_CACHE_ENTITY();
}

void hal_cap_stop_cache_data(void)
{
    GOODIX_PLANFROM_INT_CAP_STOP_CACHE_ENTITY();
}

void hal_temp_start_cache_data(void)
{
    GOODIX_PLANFROM_INT_TEMP_START_CACHE_ENTITY();
}

void hal_temp_stop_cache_data(void)
{
    GOODIX_PLANFROM_INT_TEMP_STOP_CACHE_ENTITY();
}

void hal_gh3x2x_write_cap_to_flash(GS32 WearCap1,GS32 UnwearCap1,GS32 WearCap2,GS32 UnwearCap2)
{
    GOODIX_PLANFROM_WRITE_CAP_TO_FLASH_ENTITY();
}
void hal_gh3x2x_read_cap_from_flash(GS32* WearCap1,GS32* UnwearCap1,GS32* WearCap2,GS32* UnwearCap2)
{
    GOODIX_PLANFROM_READ_CAP_FROM_FLASH_ENTITY();
}

/**
 * @fn     void hal_gsensor_drv_get_fifo_data(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index)
 * 
 * @brief  get gsensor fifo data
 *
 * @attention   When read fifo data of GH3x2x, will call this function to get corresponding cached gsensor data.
 *
 * @param[in]   None
 * @param[out]  gsensor_buffer          pointer to gsensor data buffer
 * @param[out]  gsensor_buffer_index    pointer to number of gsensor data(every gsensor data include x,y,z axis data)
 *
 * @return  None
 */
void hal_gsensor_drv_get_fifo_data(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index)
{
/**************************** WARNNING  START***************************************************/
/*  (*gsensor_buffer_index) can not be allowed bigger than __GSENSOR_DATA_BUFFER_SIZE__  ****************/
/* Be care for copying data to gsensor_buffer, length of gsensor_buffer is __GSENSOR_DATA_BUFFER_SIZE__ *****/
/**************************** WARNNING END*****************************************************/
    *gsensor_buffer_index = 25;
    for (int i = 0; i < *gsensor_buffer_index; i++)
    {
        extern GU8 WearEventState;
        if(0x00 == WearEventState)
        {
            gsensor_buffer[i].sXAxisVal = i > 20 ? 512 : 0;
            gsensor_buffer[i].sYAxisVal = 512;
            gsensor_buffer[i].sZAxisVal = i > 20 ? 512 : 0;
        }
        else
        {
            gsensor_buffer[i].sXAxisVal = 0;
            gsensor_buffer[i].sYAxisVal = 0;
            gsensor_buffer[i].sZAxisVal = 512;
        }
    }
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    for (int i = 0; i < *gsensor_buffer_index; i++)
    {
        GU32 unTimeStamp = 0;
        /* user set time stamp code here */
        GH3X2X_TimestampSyncFillAccSyncBuffer(unTimeStamp, gsensor_buffer[i].sXAxisVal, gsensor_buffer[i].sYAxisVal, gsensor_buffer[i].sZAxisVal);
    }
#endif

    GOODIX_PLANFROM_INT_GET_GS_DATA_ENTITY();


/**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---START***************************************************/
    if((*gsensor_buffer_index) > (__GSENSOR_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
/**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---END***************************************************/
}

void hal_cap_drv_get_fifo_data(STCapRawdata cap_data_buffer[], GU16 *cap_buffer_index)
{
    GOODIX_PLANFROM_INT_GET_CAP_DATA_ENTITY()  ;
    if((*cap_buffer_index) > (__CAP_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
}

void hal_temp_drv_get_fifo_data(STTempRawdata temp_data_buffer[], GU16 *temp_buffer_index)
{
    GOODIX_PLANFROM_INT_GET_TEMP_DATA_ENTITY();
    if((*temp_buffer_index) > (__TEMP_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
}

#if (__EXAMPLE_LOG_TYPE__)
/**
 * @fn     void GH3X2X_Log(char *log_string)
 * 
 * @brief  for debug version, log
 *
 * @attention   this function must define that use debug version lib
 *
 * @param[in]   log_string      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */

#if __EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_0__
void GH3X2X_Log(GCHAR *log_string)
{
    printf("%s", log_string);
     
}
#endif

#if __EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_1__
void GH3X2X_RegisterPrintf(int (**pPrintfUser)(const char *format, ...))
{
    //(*pPrintfUser) = printf;   //use printf in <stdio.h>  or use equivalent function in your platform
    GOODIX_PLANFROM_PRINTF_ENTITY();
}
#endif




#endif

/**
 * @fn     void Gh3x2x_BspDelayUs(GU16 usUsec)
 * 
 * @brief  Delay in us,user should register this function into driver lib
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

void Gh3x2x_BspDelayUs(GU16 usUsec)
{

    sys_delay_us(usUsec);
}

void GH3X2X_AdtFuncStartWithGsDetectHook(void)
{

}

/**
 * @fn     void Gh3x2x_BspDelayMs(GU16 usMsec)
 * 
 * @brief  Delay in ms
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_BspDelayMs(GU16 usMsec)
{

    sys_delay_ms(usMsec);
}

#if (__FUNC_TYPE_SOFT_ADT_ENABLE__)
/**
 * @fn     void Gh3x2x_CreateAdtConfirmTimer(void)
 * 
 * @brief  Create a timer for adt confirm which will read gsensor data periodically
 *
 * @attention   Period of timer can be set by 
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
#if (__USE_POLLING_TIMER_AS_ADT_TIMER__)&&\
    (__POLLING_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__ || __MIX_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__)
#else
void gh3x2x_soft_adt_timer_call(void* p_ctx)
{
    Gh3x2xDemoMoveDetectTimerHandler();
}

void Gh3x2xCreateAdtConfirmTimer(void)
{
    app_timer_create(&gh3x2x_soft_adt_timer_id, ATIMER_REPEAT, gh3x2x_soft_adt_timer_call);
}
#endif

/**
 * @fn     void Gh3x2x_StartAdtConfirmTimer(void)
 * 
 * @brief  Start time of adt confirm to get g sensor
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
#if __FUNC_TYPE_SOFT_ADT_ENABLE__
#if (__USE_POLLING_TIMER_AS_ADT_TIMER__)&&\
    (__POLLING_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__ || __MIX_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__)
#else
void Gh3x2x_StartAdtConfirmTimer(void)
{
    app_timer_start(gh3x2x_soft_adt_timer_id, 50, NULL);
}
#endif
#endif

/**
 * @fn     void Gh3x2x_StopAdtConfirmTimer(void)
 * 
 * @brief  Stop time of adt confirm
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
#if __FUNC_TYPE_SOFT_ADT_ENABLE__
#if (__USE_POLLING_TIMER_AS_ADT_TIMER__)&&\
    (__POLLING_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__ || __MIX_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__)
#else
void Gh3x2x_StopAdtConfirmTimer(void)
{
    app_timer_stop(gh3x2x_soft_adt_timer_id);
}
#endif
#endif
#endif




/**
 * @fn     void Gh3x2x_UserHandleCurrentInfo(void)
 * 
 * @brief  handle gh3x2x chip current information for user
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_UserHandleCurrentInfo(void)
{
    //read or write  slotx current

    //GH3X2X_GetSlotLedCurrent(0,0); //read slot 0  drv 0
    //GH3X2X_GetSlotLedCurrent(1,0); // read  slot 1  drv 0

    //GH3X2X_SlotLedCurrentConfig(0,0,50);  //set slot0 drv0 50 LSB
    //GH3X2X_SlotLedCurrentConfig(1,0,50);  //set slot1 drv0 50 LSB
}

#if (__SUPPORT_PROTOCOL_ANALYZE__)
/**
 * @fn     void Gh3x2x_HalSerialSendData(GU8* uchTxDataBuf, GU16 usBufLen)
 *
 * @brief  Serial send data
 *
 * @attention   None
 *
 * @param[in]   uchTxDataBuf        pointer to data buffer to be transmitted
 * @param[in]   usBufLen            data buffer length
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_HalSerialSendData(GU8* uchTxDataBuf, GU16 usBufLen)
{
//    sdk_err_t error_code;
    // extern sdk_err_t health_tx_data_send(uint8_t conn_idx, uint8_t *p_data, uint16_t length);
    // health_tx_data_send(0, uchTxDataBuf, usBufLen);
//    APP_ERROR_CHECK(error_code);
//    printf("send: ");
//    for(uint16_t i = 0;i < usBufLen; i ++)
//    {
//        printf("0x%02X\t",uchTxDataBuf[i]);
//    }
//    printf("\r\n");
}


void gh3x2x_serial_timer_call(void* p_ctx)
{
    Gh3x2xSerialSendTimerHandle();
}
GU8 SerialPeriodMs = 0;
/**
 * @fn      void Gh3x2xSerialSendTimerInit(GU8 uchPeriodMs)
 *
 * @brief  Gh3x2xSerialSendTimerInit
 *
 * @attention   None
 *
 * @param[in]   uchPeriodMs    timer period (ms)
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerInit(GU8 uchPeriodMs)
{
    SerialPeriodMs = uchPeriodMs;
    app_timer_create(&gh3x2x_serial_timer_id, ATIMER_REPEAT, gh3x2x_serial_timer_call);
//    EXAMPLE_LOG("[%s]: SerialPeriodMs = %d \r\n",__FUNCTION__, SerialPeriodMs);
}


/**
 * @fn     void Gh3x2xSerialSendTimerStop(void)
 *
 * @brief  Serial send timer stop
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerStop(void)
{
    app_timer_stop(gh3x2x_serial_timer_id);
}



/**
 * @fn     void Gh3x2xSerialSendTimerStart(void)
 *
 * @brief  Serial send timer start
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerStart(void)
{
//    EXAMPLE_LOG("[%s]: SerialPeriodMs = %d \r\n",__FUNCTION__, SerialPeriodMs);
    app_timer_start(gh3x2x_serial_timer_id, SerialPeriodMs, NULL);
}

#endif





void* Gh3x2xMallocUser(GU32 unMemSize)
{
#if (__USER_DYNAMIC_DRV_BUF_EN__)
#ifdef GOODIX_DEMO_PLANFORM
    GOODIX_PLANFROM_MALLOC_USER_ENTITY();
#else
    //return malloc(unMemSize);
    return 0;
#endif
#else
    return 0;
#endif
}

void Gh3x2xFreeUser(void* MemAddr)
{
#if (__USER_DYNAMIC_DRV_BUF_EN__)
    GOODIX_PLANFROM_FREE_USER_ENTITY();
    //free(MemAddr);
#endif
}









#if ( __FUNC_TYPE_BP_ENABLE__ && (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__))
/**
 * @fn     GS8 gh3x2x_write_pressure_parameters(GS32 *buffer)
 * 
 * @brief  gh3x2x get pressure value
 *
 * @attention   None
 *
 * @param[out]   buffer     buffer[0] = rawdata_a0,
 *                          buffer[1] = rawdata_a1,
 *                          buffer[2] = pressure g value of rawdata_a1,
 *
 * @return  error code
 */
GS8 gh3x2x_write_pressure_parameters(GS32 *buffer)
{
    return GOODIX_PLANFROM_PRESSURE_PARAS_WRITE_ENTITY();
}

/**
 * @fn     GS8 gh3x2x_read_pressure_parameters(GS32 *buffer)
 * 
 * @brief  gh3x2x get pressure value
 *
 * @attention   None
 *
 * @param[out]   buffer     buffer[0] = area,
 *                          buffer[1] = rawdata_a0,
 *                          buffer[2] = rawdata_a1,
 *                          buffer[3] = pressure g value,
 *
 * @return  error code
 */
GS8 gh3x2x_read_pressure_parameters(GS32 *buffer)
{
    return GOODIX_PLANFROM_PRESSURE_PARAS_READ_ENTITY();
}
#endif





/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
