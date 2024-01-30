/**
 * @file I2C_Config.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief I2C config
 * @version 0.1
 * @date 2024-01-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "app_i2c.h"
#include "I2C_Config.h"




/*====================================================================================================*/
/*====================================================================================================*
** function : 		GTADG_i2c_evt_handler(app_i2c_evt_t * p_evt)
** description: 	GuangTe ALL DEBUG I2C Handler function
** Input: 			None
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
void GTADG_i2c_evt_handler(app_i2c_evt_t * p_evt)
{
    switch (p_evt->type)
    {
        case APP_I2C_EVT_ERROR:
            //printf("APP_I2C_EVT_ERROR");
            break;

        case APP_I2C_EVT_TX_CPLT:
            //printf("APP_I2C_EVT_TX_CPLT");
            break;

        case APP_I2C_EVT_RX_DATA:
            //printf("APP_I2C_EVT_RX_DATA");
            break;
    }
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		GT_I2C_Init()
** description: 	GR5515 -> ALL I2C Initialization
** Input: 			None
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
void GT_I2C_Init(void)
{
	int ret;
	app_i2c_params_t I2C0_params_t;
	app_i2c_params_t I2C1_params_t;
	
	I2C0_params_t.id 						= I2C0_ID;
	I2C0_params_t.role 						= APP_I2C_ROLE_MASTER;
	
	I2C0_params_t.pin_cfg.scl.type 			= APP_IO_TYPE_NORMAL;
	I2C0_params_t.pin_cfg.scl.mux			= I2C0_IO_MUX;
	I2C0_params_t.pin_cfg.scl.pin			= I2C0_SCL_PIN;
	I2C0_params_t.pin_cfg.scl.pull			= APP_IO_NOPULL;
	
	I2C0_params_t.pin_cfg.sda.type 			= APP_IO_TYPE_NORMAL;
	I2C0_params_t.pin_cfg.sda.mux			= I2C0_IO_MUX;
	I2C0_params_t.pin_cfg.sda.pin			= I2C0_SDA_PIN;
	I2C0_params_t.pin_cfg.sda.pull			= APP_IO_NOPULL;
	
	I2C0_params_t.use_mode.type				= APP_I2C_TYPE_INTERRUPT;
	// I2C0_params_t.use_mode.tx_dma_channel	= DMA_Channel0;
	// I2C0_params_t.use_mode.rx_dma_channel	= DMA_Channel1;
	
	I2C0_params_t.init.speed				= I2C_SPEED_400K;
	I2C0_params_t.init.own_address			= I2C0_OWN_ADDR;
	I2C0_params_t.init.addressing_mode		= I2C_ADDRESSINGMODE_7BIT;
	I2C0_params_t.init.general_call_mode 	= I2C_GENERALCALL_DISABLE;
	
	ret = app_i2c_init(&I2C0_params_t, GTADG_i2c_evt_handler);
	printf("app_i2c_init0 = %d\r\n",ret);
	
	I2C1_params_t.id 						= I2C1_ID;
	I2C1_params_t.role 						= APP_I2C_ROLE_MASTER;
	
	I2C1_params_t.pin_cfg.scl.type 			= APP_IO_TYPE_NORMAL;
	I2C1_params_t.pin_cfg.scl.mux			= I2C1_IO_MUX;
	I2C1_params_t.pin_cfg.scl.pin			= I2C1_SCL_PIN;
	I2C1_params_t.pin_cfg.scl.pull			= APP_IO_NOPULL;
	
	I2C1_params_t.pin_cfg.sda.type 			= APP_IO_TYPE_NORMAL;
	I2C1_params_t.pin_cfg.sda.mux			= I2C1_IO_MUX;
	I2C1_params_t.pin_cfg.sda.pin			= I2C1_SDA_PIN;
	I2C1_params_t.pin_cfg.sda.pull			= APP_IO_NOPULL;
	
	I2C1_params_t.use_mode.type				= APP_I2C_TYPE_INTERRUPT;
	// I2C1_params_t.use_mode.tx_dma_channel	= DMA_Channel0;
	// I2C1_params_t.use_mode.rx_dma_channel	= DMA_Channel1;
	
	I2C1_params_t.init.speed				= I2C_SPEED_400K;
	I2C1_params_t.init.own_address			= I2C1_OWN_ADDR;
	I2C1_params_t.init.addressing_mode		= I2C_ADDRESSINGMODE_7BIT;
	I2C1_params_t.init.general_call_mode 	= I2C_GENERALCALL_DISABLE;
	
	ret = app_i2c_init(&I2C1_params_t, GTADG_i2c_evt_handler);
	printf("app_i2c_init1 = %d\r\n",ret);
}


