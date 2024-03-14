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
#include "I2C_Config/I2C_Config.h"

int nst112x_array[30];
uint16_t nst112x_array_num = 0;

/*====================================================================================================*/
/*====================================================================================================*
** function : 		Nst112x_Transmit(uint8_t ads_addr,uint8_t point)
** description: 	Don't Kown
** Input: 			uint8_t ads_addr : Nst112 I2C  Address
					uint8_t Reg	 	 : Nst112 Register Address
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
void Nst112x_Transmit(uint8_t ads_addr,uint8_t Reg)
{
	app_i2c_transmit_async(NST_I2C_ID, ads_addr, &Reg, 1);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		Nst112x_Comparator_Threshold(uint8_t Nst_Addr, int16_t LowerThreshold, int16_t UpperThreshold)
** description: 	Setting thresholds for maximum and minimum temperatures
** Input: 			uint8_t Nst_Addr			Nst112x I2C address
					int16_t LowerThreshold		input LowerThreshold
					int16_t UpperThreshold		input UpperThreshold
** Output:			None
** explain: 		If the threshold alarm is turned on, exceeding the lower or upper threshold limit will generate a level jump in the AL bit
**====================================================================================================*/
/*====================================================================================================*/
void Nst112x_Comparator_Threshold(uint8_t Nst_Addr, int16_t LowerThreshold, int16_t UpperThreshold)
{
	uint8_t Data_send[2];
	
	Data_send[0] = (LowerThreshold >> 8) & 0x00FF;
	Data_send[1] = LowerThreshold & 0x00FF;
	//printf("Nst112x_Comparator_Threshold LowerThreshold ：Data_send[0] : %x -- Data_send[1] : %x \r\n",Data_send[0],Data_send[1]);
	app_i2c_mem_write_async(NST_I2C_ID, Nst_Addr, T_low_Reg, I2C_MEMADD_SIZE_8BIT, Data_send, 2);
	
	delay_ms(2);
	
	Data_send[0] = (UpperThreshold >> 8) & 0x00FF;
	Data_send[1] = UpperThreshold & 0x00FF;
	//printf("Nst112x_Comparator_Threshold UpperThreshold ：Data_send[0] : %x -- Data_send[1] : %x \r\n",Data_send[0],Data_send[1]);
	app_i2c_mem_write_async(NST_I2C_ID, Nst_Addr, T_high_Reg, I2C_MEMADD_SIZE_8BIT, Data_send, 2);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		Nst112x_ConfigRegister(uint8_t Nst_Addr)
** description: 	Configure the parameters of the NST112
** Input: 			uint8_t Nst_Addr			Nst112x I2C address
** Output:			None
** explain: 		Setting Nst112 Consultation Datasheet Page 18-19
**====================================================================================================*/
/*====================================================================================================*/
void Nst112x_ConfigRegister(uint8_t Nst_Addr)
{
	//Configuration register consortium
	union xnst112xConfigRegisterH nst112xConfigRegisterH;
	union xnst112xConfigRegisterL nst112xConfigRegisterL;
	
	uint8_t Data_send[2];
	//0x60 == 0b01100000
	nst112xConfigRegisterH.RegisterH.OS = Continuous_Conversion_Mode;			            // 0
	nst112xConfigRegisterH.RegisterH.R0R1 = Converter_Resolution_12bit;						// 11 ---- 0.625 resolution
	nst112xConfigRegisterH.RegisterH.F0F1 =Fault_Queue_1;								    // 00 ---- Fault_Queue = 1
	nst112xConfigRegisterH.RegisterH.POL = Polarity_low;	                                // 0  Now chip not used
	nst112xConfigRegisterH.RegisterH.NA = 0;												// 0
	nst112xConfigRegisterH.RegisterH.SD = Continuous_Conversion_Mode_SD;	                // 0  ---- Continuous_Conversion_Mode
	
	//0x40 == 0b01000000  0x64 = 0b01100100
	nst112xConfigRegisterL.RegisterL.CR1CR0 = Conversion_Rate_1Hz;							// 01 ---- Conversion_Rate = 1
	nst112xConfigRegisterL.RegisterL.AL = Active_high;		                              	// 1  Now chip not used
	nst112xConfigRegisterL.RegisterL.EM = normal_mode;							            // 0  ---- normal_mode
	nst112xConfigRegisterL.RegisterL.NA = 0;												// 0
	nst112xConfigRegisterL.RegisterL.HRES = High_Resolution_R1R0;						    // 0  ----	not high
	nst112xConfigRegisterL.RegisterL.NA1 = 0;												// 0

	//Send configured Register data
	Data_send[0] = nst112xConfigRegisterH.value;
	Data_send[1] = nst112xConfigRegisterL.value;
	app_i2c_mem_write_async(NST_I2C_ID, Nst_Addr, Config_Reg, I2C_MEMADD_SIZE_8BIT, Data_send, 2);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		Get_Nst112x_ChipID(uint8_t Nst_Addr)
** description: 	Get Nst112 ChipID to know Nst112 remain or not
** Input: 			Nst_Addr	I2C address
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
uint16_t Get_Nst112x_ChipID(uint8_t Nst_Addr)
{
	uint8_t Data_get[2];
	app_i2c_mem_read_async(NST_I2C_ID, Nst_Addr, ChipID_Reg, 1, Data_get, 2);
	delay_ms(10);
	return ((uint16_t)Data_get[0]<<8) | Data_get[1] ;
}
/*====================================================================================================*/
/*====================================================================================================*
** function : 		Nst112x_Init(uint8_t Nst_Addr)
** description: 	Initialization for  NST112 
** Input: 			Nst_Addr	I2C address
** Output:			None
** explain: 		want to Get IIC is OK
**====================================================================================================*/
/*====================================================================================================*/
void Nst112x_Init(uint8_t Nst_Addr)
{
	uint16_t Value_temp;
	uint8_t temp;
	Nst112x_ConfigRegister(Nst_Addr);
	delay_ms(10);
	Nst112x_Comparator_Threshold(Nst_Addr,0x10,0x30);
	delay_ms(10);
	Value_temp = Get_Nst112x_ChipID(Nst_Addr);
	if(Value_temp == 0xA3A3 )
	{
		temp = Nst_Addr;
		printf("nst112x IIC is OK : NST112 number - %d \r\n",temp-0x47);
	}
	else
	{
		temp = Nst_Addr;
		printf("nst112x IIC is Error : NST112 number - %d ,plese check ConfigRegister\r\n",temp-0x47);
	}
	delay_ms(10);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		Nst112_four_Init(uint8_t Nst_Addr)
** description: 	Initialization for All NST112 
** Input: 			Nst_Addr	I2C address
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
void Nst112_four_Init(void)
{
	Nst112x_Init(Nst112A_I2C_Addr);
	delay_ms(10);
	Nst112x_Init(Nst112B_I2C_Addr);
	delay_ms(10);
	Nst112x_Init(Nst112C_I2C_Addr);
	delay_ms(10);
	Nst112x_Init(Nst112D_I2C_Addr);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		Get_nst112x_temperature_value(uint8_t Nst_Addr)
** description: 	Get Nst112 temperature_value
** Input: 			Nst_Addr	I2C address
** Output:			Value_Get   12Bit Value
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
int16_t Get_nst112x_temperature_value(uint8_t Nst_addr)
{
	uint8_t Data_Get[2];
	uint16_t Value_Get;
	app_i2c_mem_read_async(NST_I2C_ID, Nst_addr, Temperature_Reg, 1, Data_Get, 2);
	delay_ms(1);
	Value_Get = ((int16_t)Data_Get[0]<<8) | Data_Get[1] ;
	return (Value_Get >> 4);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		Get_nst112x_temperature(uint8_t Nst_Addr)
** description: 	Get Nst112 	temperature
** Input: 			Nst_Addr	I2C address
** Output:			Get_Temperature	temperature
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
float Get_nst112x_temperature(uint8_t Nst_addr)
{
	int16_t Value_Get;
	float Get_Temperature = 0.0;
	Value_Get = Get_nst112x_temperature_value(Nst_addr);
	Get_Temperature = Value_Get * 0.0625;
	return Get_Temperature;
}



int16_t nst112x_Sliding_filter(int16_t values)						//
{
	nst112x_array[nst112x_array_num] = values;
	nst112x_array_num ++;
	nst112x_array_num = nst112x_array_num % 2;

	double nst112x_array_total = 0;
	for(uint8_t i=0;i<2;i++)
	{
		nst112x_array_total = nst112x_array_total + nst112x_array[i];
	}
	nst112x_array_total = (int16_t)(nst112x_array_total/2);
	
	return nst112x_array_total;
}




