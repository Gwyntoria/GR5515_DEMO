/**
 * @file nst112x.h
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief Temperature sensor
 * @version 0.1
 * @date 2024-01-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef __nst112x_H
#define __nst112x_H


//#include "gr55xx_hal.h"

#include "stdint.h"



/*I2C address for different models of Nst112*/
#define Nst112A_I2C_Addr				0x48
#define Nst112B_I2C_Addr				0x49
#define Nst112C_I2C_Addr				0x4A
#define Nst112D_I2C_Addr				0x4B
 
  /* nst112 internal register address*/
#define Temperature_Reg					0x00	//Get temperature register, defualt 0x0000 16-bit data, Read
#define Config_Reg						0x01	//Config register Read/Write
#define T_low_Reg					    0x02	//Temperature Threshold Lower Limit
#define T_high_Reg					    0x03	//temperature threshold Toper Limit
#define ChipID_Reg						0x07	//Chip ID

/* Configuring Registers Using Unions & Bit Fields */
// You can refer to the website: http://www.stmcu.org.cn/module/forum/forum.php?mod=viewthread&tid=610197
// Because the memory accumulation of bitfields starts at the low end,
// place the registers in the high end of the manual at the end of the bitfield definition
union xnst112xConfigRegisterH
{
	uint8_t  value;
	struct{
		uint8_t SD:1;
		uint8_t NA:1;
		uint8_t POL:1;
		uint8_t F0F1:2;
		uint8_t R0R1:2;
		uint8_t OS:1;
	}RegisterH;
};
 
union xnst112xConfigRegisterL
{
	uint8_t  value;
	struct{	
		uint8_t NA1:1;
		uint8_t HRES:1;
		uint8_t NA:2;
		uint8_t EM:1;
		uint8_t AL:1;
		uint8_t CR1CR0:2;
	}RegisterL;
};
 
/************************************************
nst112x Internal Register Function Definition
************************************************/ 
enum OS		// If want to Know particular for : Page 11
{
	Continuous_Conversion_Mode = 0,			// 0
	One_shot_Mode	 						// 1
};

enum R0R1	// Converter Resolution
{
	Converter_Resolution_9bit = 0,			// 00
	Converter_Resolution_10bit,				// 01
	Converter_Resolution_11bit,				// 10
	Converter_Resolution_12bit				// 11
};

enum F0F1	// Consecutive faults number
{
	Fault_Queue_1 = 0,						// 00 
	Fault_Queue_2,							// 01 
	Fault_Queue_4,							// 10 
	Fault_Queue_6							// 11
};

enum POL	//AI Setting 
{
	Polarity_low = 0,	            		// 
	Polarity_high							// 
};

enum SD		// Sampling mode setting
{
   
	Continuous_Conversion_Mode_SD = 0,  	//
  	Shutdown_Mode_SD,						//	
};

enum CR1CR0	// Conversion Rate
{
	Conversion_Rate_025Hz = 0,				// 00
	Conversion_Rate_1Hz,					// 01
	Conversion_Rate_4Hz,         			// 10
	Conversion_Rate_8Hz         			// 11
};

enum AL		// Effective polarity depends on Bit10
{
	Active_low = 0,							//Active Low
	Active_high								//Active High
};

enum EM		// Extended Mode
{
	normal_mode = 0,						//
	Extended_mode	    					//
};

enum HRES	// High_Resolution
{
	High_Resolution_R1R0=0,					//Resolution set by Bits 14:13 (default)
	High_Resolution_14bit					//14Bit
};

void Nst112x_Init(uint8_t Nst_Addr);		// Nst112 Init

void Nst112_four_Init(void);				// All Nst112 Init
int16_t Get_nst112x_temperature_value(uint8_t Nst_addr);	// Get_nst112x_temperature_value
float Get_nst112x_temperature(uint8_t Nst_addr);	// Get_nst112x_temperature


extern int nst112x_array[30];
extern uint16_t nst112x_array_num;

int16_t nst112x_Sliding_filter(int16_t values);	

#endif /* __nst112x_H */
