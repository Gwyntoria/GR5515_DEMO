/**
 * @file LSM6DSOWTR.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief 六轴初始化，和获取基础数据
 * @version 0.1
 * @date 2024-01-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "LSM6DSOWTR.h"
#include "app_i2c.h"
#include "I2C_Config/I2C_Config.h"
/*====================================================================================================*/
/*====================================================================================================*
** function : 		LSM6DSOWTR_Read_Byte(uint16_t address, uint8_t register)
** description: 	LSM6DSOWTR Read one Byte
** Input: 			None
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
uint8_t LSM6DSOWTR_Read_Byte(uint8_t address, uint8_t reg)
{
    uint8_t Read_temp;
    app_i2c_mem_read_sync(LSM_I2C_ID, address, reg, 1, &Read_temp, 1,0X010);
	
    return Read_temp;
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		LSM6DSOWTR_Read_Data(uint16_t address, uint8_t register)
** description: 	LSM6DSOWTR Read one Byte
** Input: 			None
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
uint16_t LSM6DSOWTR_Read_Data( uint8_t reg)
{
    uint16_t Read_temp;
    uint8_t Data_temp[2];
    app_i2c_mem_read_sync(LSM_I2C_ID, LSM6DSO_Address, reg, 1, &Data_temp[0], 1,0x1000);
    app_i2c_mem_read_sync(LSM_I2C_ID, LSM6DSO_Address, reg + 1, 1, &Data_temp[1], 1,0x1000);
    Read_temp = (uint16_t)Data_temp[1] << 8 | Data_temp[0];
    return Read_temp;
}
/*====================================================================================================*/
/*====================================================================================================*
** function : 		LSM6DSOWTR_Write_NByte(uint8_t address, uint8_t reg, uint8_t Data)
** description: 	LSM6DSOWTR Read one Byte
** Input: 			None
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/
void LSM6DSOWTR_Write_NByte(uint8_t address, uint8_t reg, uint8_t Data)
{
    app_i2c_mem_write_async(LSM_I2C_ID, address, reg, 1, &Data, 1);
}

/*====================================================================================================*/
/*====================================================================================================*
** function : 		LSM6DSOWTR_Init()
** description: 	LSM6DSOWTR Initialization
** Input: 			None
** Output:			None
** explain: 		None
**====================================================================================================*/
/*====================================================================================================*/

void LSM6DSOWTR_Init(void)
{
	uint8_t TEMP;
	TEMP = LSM6DSOWTR_Read_Byte(0x6A,LSM6DSO_WHO_AM_I);
    if(LSM6DSO_ID == TEMP)
    {
			//printf("TEMP IS : %d !\r\n",TEMP);
			printf("Device connected successfully !\r\n");
    }
    else
    {
			//printf("TEMP IS : %d !\r\n",TEMP);
            printf("Device error or not connected !\r\n");
    }	
	/* Close I3C   write this */							
																	//00111100 0x
	lsm6dso_reg_t ctrl1_xl_s;
	
	ctrl1_xl_s.ctrl1_xl.odr_xl 			= ODR_XL_208_Hz;		//0100
	ctrl1_xl_s.ctrl1_xl.fs_xl			= FS_XL_G_2;			//00
	ctrl1_xl_s.ctrl1_xl.lpf2_xl_en 		= 0;					//0
	ctrl1_xl_s.ctrl1_xl.not_used_01 	= 0;					//0
	TEMP = LSM6DSOWTR_Read_Byte(LSM6DSO_Address,LSM6DSO_CTRL1_XL);
	printf("LSM6DSO_CTRL1_XL start value is %x \r\n",TEMP);
    LSM6DSOWTR_Write_NByte(LSM6DSO_Address,LSM6DSO_CTRL1_XL,ctrl1_xl_s.byte);
	printf("LSM6DSO_CTRL1_XL stop value is %x \r\n",ctrl1_xl_s.byte);
	
	lsm6dso_reg_t ctrl2_g_s;
	ctrl2_g_s.ctrl2_g.odr_g				= ODR_G_104_Hz;			// 0100
	ctrl2_g_s.ctrl2_g.fs_g				= FS_G_250_DPS;			// 00
	ctrl2_g_s.ctrl2_g.fs_125			= 0;					// 0
	ctrl2_g_s.ctrl2_g.not_used_01 		= 0;					// 0
	TEMP = LSM6DSOWTR_Read_Byte(LSM6DSO_Address,LSM6DSO_CTRL2_G);
	printf("LSM6DSO_CTRL2_G start value is %x \r\n",TEMP);
    LSM6DSOWTR_Write_NByte(LSM6DSO_Address,LSM6DSO_CTRL2_G,ctrl2_g_s.byte);
	printf("LSM6DSO_CTRL2_G stop value is %x \r\n",ctrl2_g_s.byte);
}


/**
  * @defgroup  LSM6DSO_Sensitivity
  * @brief     These functions convert raw-data into engineering units.
  * @{
  *
  */
// float_t lsm6dso_from_fs2_to_mg(int16_t lsb)
// {
//   return ((float_t)lsb) * 0.061f;
// }

// float_t lsm6dso_from_fs4_to_mg(int16_t lsb)
// {
//   return ((float_t)lsb) * 0.122f;
// }

// float_t lsm6dso_from_fs8_to_mg(int16_t lsb)
// {
//   return ((float_t)lsb) * 0.244f;
// }

// float_t lsm6dso_from_fs16_to_mg(int16_t lsb)
// {
//   return ((float_t)lsb) * 0.488f;
// }

// float_t lsm6dso_from_fs125_to_mdps(int16_t lsb)
// {
//   return ((float_t)lsb) * 4.375f;
// }

// float_t lsm6dso_from_fs500_to_mdps(int16_t lsb)
// {
//   return ((float_t)lsb) * 17.50f;
// }

// float_t lsm6dso_from_fs250_to_mdps(int16_t lsb)
// {
//   return ((float_t)lsb) * 8.750f;
// }

// float_t lsm6dso_from_fs1000_to_mdps(int16_t lsb)
// {
//   return ((float_t)lsb) * 35.0f;
// }

// float_t lsm6dso_from_fs2000_to_mdps(int16_t lsb)
// {
//   return ((float_t)lsb) * 70.0f;
// }

// float_t lsm6dso_from_lsb_to_celsius(int16_t lsb)
// {
//   return (((float_t)lsb / 256.0f) + 25.0f);
// }

// float_t lsm6dso_from_lsb_to_nsec(int16_t lsb)
// {
//   return ((float_t)lsb * 25000.0f);
// }
