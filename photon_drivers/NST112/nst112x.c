/**
 * @file nst112x.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief 测温芯片，初始化和获取初始温度
 * @version 0.1
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "nst112x.h"
#include "app_i2c.h"
#include "app_log.h"
// #include "I2C_Config/I2C_Config.h"

// 温度
#define NST_SCL_PIN APP_IO_PIN_30
#define NST_SDA_PIN APP_IO_PIN_26
#define NST_IO_MUX  APP_IO_MUX_0
#define NST_I2C_ID  APP_I2C_ID_1

#define NST_OWN_ADDR 0xA2

int      nst112x_array[30];
uint16_t nst112x_array_num = 0;

void _nst112x_i2c_evt_handler(app_i2c_evt_t* p_evt) {
    switch (p_evt->type) {
        case APP_I2C_EVT_ERROR:
            // printf("APP_I2C_EVT_ERROR");
            break;

        case APP_I2C_EVT_TX_CPLT:
            // printf("APP_I2C_EVT_TX_CPLT");
            break;

        case APP_I2C_EVT_RX_DATA:
            // printf("APP_I2C_EVT_RX_DATA");
            break;
    }
}

int _nst112x_i2c_init(void) {
    int ret;

    app_i2c_params_t nst112x_params_t;

    nst112x_params_t.id   = NST_I2C_ID;
    nst112x_params_t.role = APP_I2C_ROLE_MASTER;

    nst112x_params_t.pin_cfg.scl.type = APP_IO_TYPE_NORMAL;
    nst112x_params_t.pin_cfg.scl.mux  = NST_IO_MUX;
    nst112x_params_t.pin_cfg.scl.pin  = NST_SCL_PIN;
    nst112x_params_t.pin_cfg.scl.pull = APP_IO_NOPULL;

    nst112x_params_t.pin_cfg.sda.type = APP_IO_TYPE_NORMAL;
    nst112x_params_t.pin_cfg.sda.mux  = NST_IO_MUX;
    nst112x_params_t.pin_cfg.sda.pin  = NST_SDA_PIN;
    nst112x_params_t.pin_cfg.sda.pull = APP_IO_NOPULL;

    nst112x_params_t.use_mode.type = APP_I2C_TYPE_INTERRUPT;
    // nst112x_params_t.use_mode.tx_dma_channel	= DMA_Channel0;
    // nst112x_params_t.use_mode.rx_dma_channel	= DMA_Channel1;

    nst112x_params_t.init.speed             = I2C_SPEED_400K;
    nst112x_params_t.init.own_address       = NST_OWN_ADDR;
    nst112x_params_t.init.addressing_mode   = I2C_ADDRESSINGMODE_7BIT;
    nst112x_params_t.init.general_call_mode = I2C_GENERALCALL_DISABLE;

    ret = app_i2c_init(&nst112x_params_t, _nst112x_i2c_evt_handler);
    if (ret != 0) {
        APP_LOG_ERROR("nst112x i2c init failed with 0x%04x", ret);
        return -1;
    }

    APP_LOG_INFO("nst112x i2c init success");

    return 0;
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		_nst112x_config_register(uint8_t Nst_Addr)
** description: 	Configure the parameters of the NST112
** Input: 			uint8_t Nst_Addr			Nst112x I2C address
** Output:			None
** explain: 		Setting Nst112 Consultation Datasheet Page 18-19
**====================================================================================================*/
/*====================================================================================================*/
void _nst112x_config_register(uint8_t Nst_Addr) {
    // Configuration register consortium
    union xnst112xConfigRegisterH nst112xConfigRegisterH;
    union xnst112xConfigRegisterL nst112xConfigRegisterL;

    uint8_t Data_send[2];
    // 0x60 == 0b01100000
    nst112xConfigRegisterH.RegisterH.OS   = Continuous_Conversion_Mode;    // 0
    nst112xConfigRegisterH.RegisterH.R0R1 = Converter_Resolution_12bit;    // 11 ---- 0.625 resolution
    nst112xConfigRegisterH.RegisterH.F0F1 = Fault_Queue_1;                 // 00 ---- Fault_Queue = 1
    nst112xConfigRegisterH.RegisterH.POL  = Polarity_low;                  // 0  Now chip not used
    nst112xConfigRegisterH.RegisterH.NA   = 0;                             // 0
    nst112xConfigRegisterH.RegisterH.SD   = Continuous_Conversion_Mode_SD; // 0  ---- Continuous_Conversion_Mode

    // 0x40 == 0b01000000  0x64 = 0b01100100
    nst112xConfigRegisterL.RegisterL.CR1CR0 = Conversion_Rate_1Hz;  // 01 ---- Conversion_Rate = 1
    nst112xConfigRegisterL.RegisterL.AL     = Active_high;          // 1  Now chip not used
    nst112xConfigRegisterL.RegisterL.EM     = normal_mode;          // 0  ---- normal_mode
    nst112xConfigRegisterL.RegisterL.NA     = 0;                    // 0
    nst112xConfigRegisterL.RegisterL.HRES   = High_Resolution_R1R0; // 0  ----	not high
    nst112xConfigRegisterL.RegisterL.NA1    = 0;                    // 0

    // Send configured Register data
    Data_send[0] = nst112xConfigRegisterH.value;
    Data_send[1] = nst112xConfigRegisterL.value;
    app_i2c_mem_write_sync(NST_I2C_ID, Nst_Addr, Config_Reg, I2C_MEMADD_SIZE_8BIT, Data_send, 2, 0x1000);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		_nst112x_comparator_threshold(uint8_t Nst_Addr, int16_t LowerThreshold, int16_t UpperThreshold)
** description: 	Setting thresholds for maximum and minimum temperatures
** Input: 			uint8_t Nst_Addr			Nst112x I2C address
                    int16_t LowerThreshold		input LowerThreshold
                    int16_t UpperThreshold		input UpperThreshold
** Output:			None
** explain: 		If the threshold alarm is turned on, exceeding the lower or upper threshold limit will generate a level jump in the AL bit
**====================================================================================================*/
/*====================================================================================================*/
void _nst112x_comparator_threshold(uint8_t Nst_Addr, int16_t LowerThreshold, int16_t UpperThreshold) {
    uint8_t Data_send[2];

    Data_send[0] = (LowerThreshold >> 8) & 0x00FF;
    Data_send[1] = LowerThreshold & 0x00FF;
    // printf("_nst112x_comparator_threshold LowerThreshold ：Data_send[0] : %x -- Data_send[1] : %x \r\n",Data_send[0],Data_send[1]);
    app_i2c_mem_write_sync(NST_I2C_ID, Nst_Addr, T_low_Reg, I2C_MEMADD_SIZE_8BIT, Data_send, 2, 0x1000);

    delay_ms(2);

    Data_send[0] = (UpperThreshold >> 8) & 0x00FF;
    Data_send[1] = UpperThreshold & 0x00FF;
    // printf("_nst112x_comparator_threshold UpperThreshold ：Data_send[0] : %x -- Data_send[1] : %x \r\n",Data_send[0],Data_send[1]);
    app_i2c_mem_write_sync(NST_I2C_ID, Nst_Addr, T_high_Reg, I2C_MEMADD_SIZE_8BIT, Data_send, 2, 0x1000);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		_get_nst112x_chip_id(uint8_t Nst_Addr)
** description: 	Get Nst112 ChipID to know Nst112 remain or not
** Input: 			Nst_Addr	I2C address
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
uint16_t _get_nst112x_chip_id(uint8_t Nst_Addr) {
    uint8_t Data_get[2];
    app_i2c_mem_read_sync(NST_I2C_ID, Nst_Addr, ChipID_Reg, 1, Data_get, 2, 0x1000);
    delay_ms(10);
    return ((uint16_t)Data_get[0] << 8) | Data_get[1];
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		_nst112x_init(uint8_t Nst_Addr)
** description: 	Initialization for  NST112
** Input: 			Nst_Addr	I2C address
** Output:			None
** explain: 		want to Get IIC is OK
**====================================================================================================*/
/*====================================================================================================*/
void _nst112x_init(uint8_t Nst_Addr) {
    uint16_t value_temp;
    uint8_t  temp;

    _nst112x_config_register(Nst_Addr);
    delay_ms(10);

    _nst112x_comparator_threshold(Nst_Addr, 0x10, 0x30);
    delay_ms(10);

    value_temp = _get_nst112x_chip_id(Nst_Addr);
    if (value_temp == 0xA3A3) {
        temp = Nst_Addr;
        APP_LOG_INFO("nst112x IIC is OK : NST112 number - %d", temp - 0x47);
    } else {
        temp = Nst_Addr;
        APP_LOG_INFO("nst112x IIC is Error : NST112 number - %d ,plese check ConfigRegister", temp - 0x47);
    }

    delay_ms(10);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		nst112_init(uint8_t Nst_Addr)
** description: 	Initialization for All NST112
** Input: 			Nst_Addr	I2C address
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
void nst112_init(void) {
	_nst112x_i2c_init();

    // _nst112x_init(Nst112A_I2C_Addr);
    // delay_ms(10);
    // _nst112x_init(Nst112B_I2C_Addr);
    // delay_ms(10);
    _nst112x_init(Nst112C_I2C_Addr);
    delay_ms(10);
    _nst112x_init(Nst112D_I2C_Addr);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		nst112x_transmit(uint8_t ads_addr,uint8_t point)
** description: 	Don't Kown
** Input: 			uint8_t ads_addr : Nst112 I2C  Address
                    uint8_t Reg	 	 : Nst112 Register Address
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
void nst112x_transmit(uint8_t ads_addr, uint8_t Reg) {
    app_i2c_transmit_sync(NST_I2C_ID, ads_addr, &Reg, 1, 0x1000);
}

/**
 * @brief Get Nst112 temperature_value
 *
 * @param Nst_addr I2C address
 *
 * @return Value_Get 12Bit Value
 */
int16_t get_nst112x_temperature_value(uint8_t Nst_addr) {
    uint8_t  Data_Get[2];
    uint16_t Value_Get;

    app_i2c_mem_read_sync(NST_I2C_ID, Nst_addr, Temperature_Reg, 1, Data_Get, 2, 0x1000);
    delay_ms(1);

    Value_Get = ((int16_t)Data_Get[0] << 8) | Data_Get[1];

    return (Value_Get >> 4);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		get_nst112x_temperature(uint8_t Nst_Addr)
** description: 	Get Nst112 	temperature
** Input: 			Nst_Addr	I2C address
** Output:			Get_Temperature	temperature
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
float get_nst112x_temperature(uint8_t Nst_addr) {
    int16_t Value_Get;
    float   Get_Temperature = 0.0;

    Value_Get = get_nst112x_temperature_value(Nst_addr);

    Get_Temperature = Value_Get * 0.0625;

    return Get_Temperature;
}

int16_t nst112x_sliding_filter(int16_t values) //
{
    nst112x_array[nst112x_array_num] = values;
    nst112x_array_num++;
    nst112x_array_num = nst112x_array_num % 2;

    double nst112x_array_total = 0;
    for (uint8_t i = 0; i < 2; i++) {
        nst112x_array_total += nst112x_array[i];
    }
    nst112x_array_total = (int16_t)(nst112x_array_total / 2);

    return nst112x_array_total;
}
