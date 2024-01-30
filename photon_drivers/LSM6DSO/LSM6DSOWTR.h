#ifndef _LSM6DSOWTR_H_
#define _LSM6DSOWTR_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <math.h>

/* Define ------------------------------------------------------------------- */

/*About LSM6DSO */
#define LSM_Display                     0                       // display 0: disable 1: enable
#define LSM6DSO_Address                 0x6A                    // LSM6DSO Adress
#define LSM6DSO_ID                      0x6C                    // LSM6DSO read-only register



/* LSM6DSO Register description -------------------------------------------- */
#define LSM6DSO_FUNC_CFG_ACCESS         0x01U
typedef struct
{
  uint8_t not_used                      : 6;
  uint8_t shub_reg_access               : 1;                    // Enable access to the sensor hub (I²C master) registers.
  uint8_t func_cfg_access               : 1;                    // Enable access to the embedded functions configuration registers.
} lsm6dso_func_cfg_access_t;



#define LSM6DSO_PIN_CTRL                0x02U
typedef struct
{
  uint8_t not_used                      : 6;
  uint8_t sdo_pu_en                     : 1;                    // Enable pull-up on SDO pin
  uint8_t ois_pu_dis                    : 1;                    // Disable pull-up on both OCS_Aux and SDO_Aux pins. Default value: 0
} lsm6dso_pin_ctrl_t;



#define LSM6DSO_FIFO_CTRL1              0x07U                   // don't know use
typedef struct
{
  uint8_t wtm                           : 8;
} lsm6dso_fifo_ctrl1_t;



#define LSM6DSO_FIFO_CTRL2              0x08U
typedef struct
{
  uint8_t wtm                           : 1;
  uint8_t uncoptr_rate                  : 2;
  uint8_t not_used_01                   : 1;                    // This field configures the compression algorithm to write non-compressed data at each rate.
  uint8_t odrchg_en                     : 1;                    //Enables ODR CHANGE virtual sensor to be batched in FIFO
  uint8_t not_used_02                   : 1;                    
  uint8_t fifo_compr_rt_en              : 1;                    //Enables/Disables compression algorithm runtime
  uint8_t stop_on_wtm                   : 1;                    //Sensing chain FIFO stop values memorization at threshold level
} lsm6dso_fifo_ctrl2_t;

enum UNCOPTR_RATE
{                                                               // This field configures the compression algorithm to write non-compressed data at each rate.
    UNCOPTR_RATE_DISABLE                = 0,                    // Non-compressed data writing is not forced
    Batch_data_8                        = 1,                    // Non-compressed data every 8 batch data rate
    Batch_data_16                       = 2,
    Batch_data_32
};



#define LSM6DSO_FIFO_CTRL3              0x09U
typedef struct
{
  uint8_t bdr_xl                        : 4;
  uint8_t bdr_gy                        : 4;
} lsm6dso_fifo_ctrl3_t;

enum BDR_GY
{                                                             	// Selects Batching Data Rate (writing frequency in FIFO) for gyroscope data.
    BDR_GY_DISABLE                      = 0,
    BDR_GY_HZ_12_5                      = 1,
    BDR_GY_HZ_26                        = 2,
    BDR_GY_HZ_52                        = 3,
    BDR_GY_HZ_104                       = 4,
    BDR_GY_HZ_208                       = 5,
    BDR_GY_HZ_417                       = 6,
    BDR_GY_HZ_833                       = 7,
    BDR_GY_HZ_1667                      = 8,
    BDR_GY_HZ_3333                      = 9,
    BDR_GY_HZ_6667                      = 10,
    BDR_GY_HZ_6_5                       = 11, 
    BDR_GY_NOT_ALLOWED
};

enum BDR_XL
{                                                         		// Selects Batching Data Rate (writing frequency in FIFO) for accelerometer data.
    BDR_XL_DISABLE                  	= 0,
    BDR_XL_HZ_12_5                   	= 1,
    BDR_XL_HZ_26                     	= 2,
    BDR_XL_HZ_52                   		= 3,
    BDR_XL_HZ_104                     	= 4,
    BDR_XL_HZ_208                      	= 5,
    BDR_XL_HZ_417                      	= 6,
    BDR_XL_HZ_833                     	= 7,
    BDR_XL_HZ_1667                    	= 8,
    BDR_XL_HZ_3333                    	= 9,
    BDR_XL_HZ_6667                     	= 10,
    BDR_XL_HZ_1_6                      	= 11, 
    BDR_XL_NOT_ALLOWED
};



#define LSM6DSO_FIFO_CTRL4             	0x0AU
typedef struct
{
  uint8_t fifo_mode                    	: 3;
  uint8_t not_used_01                  	: 1;
  uint8_t odr_t_batch                  	: 2;
  uint8_t dec_ts_batch                 	: 2;
} lsm6dso_fifo_ctrl4_t;

enum DEC_TS_BATCH
{       // Selects decimation for timestamp batching in FIFO. Writing rate will be the maximum rate between XL and GYRO BDR divided by decimation decoder.
    Decimation_0                      	= 0,
    Decimation_1                       	= 1,
    Decimation_8                      	= 2,
    Decimation_32
};
    
enum ODR_T_BATCH
{                                                             	// Selects batching data rate (writing frequency in FIFO) for temperature data
    Batched_0                         	= 0,
    Batched_1_6                       	= 1,
    Batched_12_5                       	= 2,
    Batched_52
};

enum FIFO_MODE
{                                                              	// FIFO mode selection
    Bypass_mode                       	= 0,                    // FIFO disabled
    FIFO_mode                          	= 1,                    // stops collecting data when FIFO is full;
    Reserved_1                         	= 2,                    // 
    Continuous_to_FIFO_mode            	= 3,                    // Continuous mode until trigger is deasserted, then FIFO mode;
    Bypass_to_Continuous_mode          	= 4,                    // Bypass mode until trigger is deasserted, then Continuous mode;
    Reserved_2                        	= 5,                    // 
    Continuous_mode                   	= 6,                    // if the FIFO is full, the new sample overwrites the older one;
    Bypass_to_FIFO_mode               	= 7,                    // Bypass mode until trigger is deasserted, then FIFO mode.
};



#define LSM6DSO_COUNTER_BDR_REG1       	0x0BU
typedef struct
{
  uint8_t cnt_bdr_th                  	: 3;                    // In conjunction with CNT_BDR_TH_[7:0]
  uint8_t not_used_01                  	: 2;
  uint8_t trig_counter_bdr             	: 1;                    // Selects the trigger for the internal counter of batching events between XL and gyro
  uint8_t rst_counter_bdr               : 1;                    // Resets the internal counter of batching events for a single sensor.
  uint8_t dataready_pulsed             	: 1;                    // Enables pulsed data-ready mode
} lsm6dso_counter_bdr_reg1_t;

enum TRIG_COUNTER_BDR
{
    XL_batching                       	= 0,                    // XL batching event
    GYRO_batching                                             	// GYRO batching event
};



#define LSM6DSO_COUNTER_BDR_REG2       	0x0CU
typedef struct
{
  uint8_t cnt_bdr_th                    : 8;
} lsm6dso_counter_bdr_reg2_t;



#define LSM6DSO_INT1_CTRL               0x0DU
typedef struct
{
  uint8_t int1_drdy_xl                 	: 1;                    // Enables accelerometer data-ready interrupt on INT1 pin. It can be also used to trigger an IBI when the MIPI I3CSM interface is used.
  uint8_t int1_drdy_g                 	: 1;                    // Enables gyroscope data-ready interrupt on INT1 pin. It can be also used to trigger an IBI when the MIPI I3CSM interface is used.
  uint8_t int1_boot                    	: 1;                    // Enables boot status on INT1 pin
  uint8_t int1_fifo_th                	: 1;                    // Enables FIFO threshold interrupt on INT1 pin. It can be also used to trigger an IBI when the MIPI I3CSM interface is used.
  uint8_t int1_fifo_ovr               	: 1;                    // Enables FIFO overrun interrupt on INT1 pin. It can be also used to trigger an IBI when the MIPI I3CSM interface is used.
  uint8_t int1_fifo_full               	: 1;                    // Enables FIFO full flag interrupt on INT1 pin. It can be also used to trigger an IBI when the MIPI I3CSM interface is used.
  uint8_t int1_cnt_bdr                 	: 1;                    // Enables COUNTER_BDR_IA interrupt on INT1
  uint8_t den_drdy_flag                 : 1;                    // Sends DEN_DRDY (DEN stamped on Sensor Data flag) to INT1 pin
} lsm6dso_int1_ctrl_t;



#define LSM6DSO_INT2_CTRL             	0x0EU
typedef struct
{
  uint8_t int2_drdy_xl                  : 1;					// Accelerometer data-ready interrupt on INT2 pin     
  uint8_t int2_drdy_g                   : 1;					// Gyroscope data-ready interrupt on INT2 pin
  uint8_t int2_drdy_temp              	: 1;					// Enables temperature sensor data-ready interrupt on INT2 pin
  uint8_t int2_fifo_th                 	: 1;					// Enables FIFO threshold interrupt on INT2 pin
  uint8_t int2_fifo_ovr                	: 1;					// Enables FIFO overrun interrupt on INT2 pin
  uint8_t int2_fifo_full               	: 1;					// Enables FIFO full flag interrupt on INT2 pin
  uint8_t int2_cnt_bdr                 	: 1;					// Enables COUNTER_BDR_IA interrupt on INT2
  uint8_t not_used_01                 	: 1;                    // 
} lsm6dso_int2_ctrl_t;



#define LSM6DSO_WHO_AM_I              	0x0FU					// WHO_AM_I register (r).This is a read-only register. Its value is fixed at 6Ch.



#define LSM6DSO_CTRL1_XL              	0x10U
typedef struct
{
  uint8_t not_used_01              		: 1;
  uint8_t lpf2_xl_en               		: 1;					// Accelerometer high-resolution selection
  uint8_t fs_xl                    		: 2;				
  uint8_t odr_xl                   		: 4;
} lsm6dso_ctrl1_xl_t;

enum ODR_XL
{	//XL_HM_MODE = 1 -> LOW POWER								//XL_HM_MODE = 0 -> HIGH POWER
	ODR_XL_Power_down 					= 0,					//Power_down
	ODR_XL_12_5_Hz						= 1,					//12.5 Hz
	ODR_XL_26_Hz						= 2,					//26 Hz
	ODR_XL_52_Hz						= 3,					//52 Hz
	//next all normal POWER
	ODR_XL_104_Hz						= 4,					//104 Hz
	ODR_XL_208_Hz						= 5,					//208 Hz
	//next all high POWER
	ODR_XL_416_Hz						= 6,					//416 Hz
	ODR_XL_833_Hz						= 7,					//833 Hz
	ODR_XL_1660_Hz						= 8,					//1.66 kHz
	ODR_XL_3330_Hz						= 9,					//3.33 kHz
	ODR_XL_6660_Hz						= 10,					//6.66 kHz
	ODR_XL_1_6_Hz						= 11,					//12.5 Hz
	ODR_XL_Not_allowed											//Not allowed
};

enum FS_XL
{	//XL_FS_MODE = 0											//	XL_FS_MODE = 1
	FS_XL_G_2							= 0,	//(defulat)		//	2g	
	FS_XL_G_16							= 1,					//  2g
	FS_XL_G_4							= 2,					//  4g
	FS_XL_G_8							= 3						//  8g
};



#define LSM6DSO_CTRL2_G               	0x11U
typedef struct
{
  uint8_t not_used_01              		: 1;
  uint8_t fs_125				   		: 1;					// Selects gyro UI chain full-scale 125 dps
  uint8_t fs_g                     		: 2; 					// Gyroscope UI chain full-scale selection
  uint8_t odr_g                    		: 4;
} lsm6dso_ctrl2_g_t;

enum ODR_G
{	//XL_HM_MODE = 1 -> LOW POWER								//XL_HM_MODE = 0 -> HIGH POWER
	ODR_G_Power_down 					= 0,					//Power_down
	ODR_G_12_5_Hz						= 1,					//12.5 Hz
	ODR_G_26_Hz							= 2,					//26 Hz
	ODR_G_52_Hz							= 3,					//52 Hz
	//next all normal POWER
	ODR_G_104_Hz						= 4,					//104 Hz
	ODR_G_208_Hz						= 5,					//208 Hz
	//next all high POWER
	ODR_G_416_Hz						= 6,					//416 Hz
	ODR_G_833_Hz						= 7,					//833 Hz
	ODR_G_1660_Hz						= 8,					//1.66 kHz
	ODR_G_3330_Hz						= 9,					//3.33 kHz
	ODR_G_6660_Hz						= 10,					//6.66 kHz
	ODR_G_1_6_Hz						= 11,					//12.5 Hz
	ODR_G_Not_allowed											//Not allowed
};

enum FS_G
{
	FS_G_250_DPS						= 0,					//
	FS_G_500_DPS						= 1,
	FS_G_1000_DPS						= 2,
	FS_G_2000_DPS						= 3
};



#define LSM6DSO_CTRL3_C               	0x12U
typedef struct
{
  uint8_t sw_reset                 		: 1;					// Software reset. Default value: 0
  uint8_t not_used_01              		: 1;					// 
  uint8_t if_inc                   		: 1;					// Register address automatically incremented during a multiple byte access with a serial interface (I²C or SPI). Default value: 1
  uint8_t sim                      		: 1;					// SPI Serial Interface Mode selection. Default value: 0
  uint8_t pp_od                   		: 1;					// Push-pull/open-drain selection on INT1 and INT2 pins. Default value: 0
  uint8_t h_lactive                		: 1;					// Interrupt activation level. Default value: 0
  uint8_t bdu                      		: 1;					// Block Data Update. Default value: 0
  uint8_t boot                     		: 1;					// Reboots memory content. Default value: 0
} lsm6dso_ctrl3_c_t;



#define LSM6DSO_CTRL4_C                	0x13U
typedef struct
{
  uint8_t not_used_01              		: 1;
  uint8_t lpf1_sel_g               		: 1;					// Enables gyroscope digital LPF1 if auxiliary SPI is disabled; the bandwidth can be selected through FTYPE[2:0] in CTRL6_C (15h).			
  uint8_t i2c_disable              		: 1;					// Disables I²C interface. Default value: 0
  uint8_t drdy_mask               	 	: 1;					// Enables data available
  uint8_t not_used_02              		: 1;					// 
  uint8_t int2_on_int1             		: 1;					// All interrupt signals available on INT1 pin enable. Default value: 0
  uint8_t sleep_g                  		: 1;					// Enables gyroscope Sleep mode. Default value:0
  uint8_t not_used_03              		: 1;					// 
} lsm6dso_ctrl4_c_t;



#define LSM6DSO_CTRL5_C               	0x14U
typedef struct
{
  uint8_t st_xl                    		: 2;					// Linear acceleration sensor self-test enable. Default value: 00
  uint8_t st_g                     		: 2;					// Angular rate sensor self-test enable. Default value: 00
  uint8_t not_used_01              		: 1;
  uint8_t rounding                 		: 2;					// Circular burst-mode (rounding) read from the output registers. Default value: 00
  uint8_t xl_ulp_en                		: 1;					// Accelerometer ultra-low-power mode enable. Default value: 0
} lsm6dso_ctrl5_c_t;

enum ROUNDING
{
	NO_rounding							= 0,
	ONLY_ACC							= 1,					// accelerometer only
	ONLY_GYR							= 2,					// gyroscope only
	ACC_GYR														// gyroscope + accelerometer
};

enum ST_G
{
	Normal_mode							= 0,					// Normal mode
	ST_G_Positive						= 1,					// Positive sign self-test
	ST_G_Not_allowed					= 2,					// Not allowed
	ST_G_Negative												// Negative sign self-test	
};

enum ST_XL
{
	ST_XL_Normal_mode					= 0,					// Normal mode
	ST_XL_Positive						= 1,					// Positive sign self-test
	ST_XL_Negative						= 2,					// Negative sign self-test
	ST_XL_Not_allowed											// Not allowed
};



#define LSM6DSO_CTRL6_C                 0x15U
typedef struct
{
  uint8_t ftype                    		: 3;					// Gyroscope's low-pass filter (LPF1) bandwidth selection Please found Datasheet Page 54
  uint8_t usr_off_w                		: 1;					// Weight of XL user offset bits of registers X_OFS_USR (73h), Y_OFS_USR (74h), Z_OFS_USR (75h)
  uint8_t xl_hm_mode               		: 1;					// High-performance operating mode disable for accelerometer. Default value: 0
  uint8_t den_mode                 		: 3;   					/* trig_en + lvl1_en + lvl2_en */
} lsm6dso_ctrl6_c_t;

enum DEN_MODE
{
	Level_sensitive_trigger				= 2,					// Level-sensitive trigger mode is selected
	Level_sensitive_latched				= 3,					// Level-sensitive latched mode is selected
	Edge_sensitive_trigger				= 4,					// Edge-sensitive trigger mode is selected
	Level_sensitive_FIFO				= 6						// Level-sensitive FIFO enable mode is selected
};



#define LSM6DSO_CTRL7_G                	0x16U
typedef struct
{
  uint8_t ois_on                   		: 1;					// Enables/disables the OIS chain from primary interface when the OIS_ON_EN bit is '1'.
  uint8_t usr_off_on_out           		: 1;					// Enables accelerometer user offset correction block
  uint8_t ois_on_en                		: 1;					// Selects how to enable and disable the OIS chain, after first configuration and enabling through SPI2.
  uint8_t not_used_01              		: 1;					// 
  uint8_t hpm_g                    		: 2;					// Gyroscope digital HP filter cutoff selection. Default: 00
  uint8_t hp_en_g                  		: 1;					// Enables gyroscope digital high-pass filter. The filter is enabled only if the gyro is in HP mode. Default value: 0
  uint8_t g_hm_mode                		: 1;					// Disables high-performance operating mode for gyroscope. Default value: 0		
} lsm6dso_ctrl7_g_t;

enum HPM_G
{
	HPM_G_16MHZ							= 0,
	HPM_G_65MHZ							= 1,
	HPM_G_260MHZ						= 2,
	HPM_G_1_04HZ
};



#define LSM6DSO_CTRL8_XL               	0x17U
typedef struct
{
  uint8_t low_pass_on_6d           		: 1;					// LPF2 on 6D function selection. 
  uint8_t xl_fs_mode               		: 1;					// Accelerometer full-scale management between UI chain and OIS chain
  uint8_t hp_slope_xl_en           		: 1;					// Accelerometer slope filter / high-pass filter selection.
  uint8_t fastsettl_mode_xl        		: 1;					// Enables accelerometer LPF2 and HPF fast-settling mode. The filter sets the second samples after writing this bit. Active only during device exit from power- down mode. Default value: 0
  uint8_t hp_ref_mode_xl           		: 1;					// Enables accelerometer high-pass filter reference mode (valid for high-pass path - HP_SLOPE_XL_EN bit must be ‘1’). Default value: 0(1)
  uint8_t hpcf_xl                  		: 3;					// Accelerometer LPF2 and HP filter configuration and cutoff setting. Please found Datasheet Page 56
} lsm6dso_ctrl8_xl_t;


#define LSM6DSO_CTRL9_XL              	0x18U
typedef struct
{
  uint8_t not_used_01              		: 1;
  uint8_t i3c_disable              		: 1;					// Disables MIPI I3CSM communication protocol(1)
  uint8_t den_lh                   		: 1;					// DEN active level configuration. Default value: 0
  uint8_t den_xl_en                 	: 1; 					// Extends DEN functionality to accelerometer sensor. Default value: 0
  uint8_t den_xl_g                 		: 1;   					// DEN stamping sensor selection. Default value: 0
  uint8_t den_z                    		: 1;					// DEN value stored in LSB of Z-axis. Default value: 1
  uint8_t den_y                    		: 1;					// DEN value stored in LSB of Y-axis. Default value: 1
  uint8_t den_x                    		: 1;					// DEN value stored in LSB of X-axis. Default value: 1
} lsm6dso_ctrl9_xl_t;



#define LSM6DSO_CTRL10_C                0x19U
typedef struct
{
  uint8_t not_used_01              		: 5;
  uint8_t timestamp_en             		: 1;					// Enables timestamp counter. default value: 0
  uint8_t not_used_02              		: 2;
} lsm6dso_ctrl10_c_t;



#define LSM6DSO_ALL_INT_SRC             0x1AU
typedef struct
{
  uint8_t ff_ia                    		: 1;					// Free-fall event status. Default value: 0
  uint8_t wu_ia                    		: 1;					// Wake-up event status. Default value: 0
  uint8_t single_tap               		: 1;					// Single-tap event status. Default value:0
  uint8_t double_tap               		: 1;					// Double-tap event status. Default value: 0
  uint8_t d6d_ia                   		: 1;					// Interrupt active for change in position of portrait, landscape, face-up, face-down. Default value: 0
  uint8_t sleep_change_ia          		: 1;					// Detects change event in activity/inactivity status. Default value: 0
  uint8_t not_used_01              		: 1;					// 
  uint8_t timestamp_endcount       		: 1;					// Alerts timestamp overflow within 6.4 ms
} lsm6dso_all_int_src_t;



#define LSM6DSO_WAKE_UP_SRC            	0x1BU
typedef struct
{
  uint8_t z_wu                     		: 1;					// Wakeup event detection status on Z-axis. Default value: 0
  uint8_t y_wu                     		: 1;					// Wakeup event detection status on Y-axis. Default value: 0
  uint8_t x_wu                     		: 1;					// Wakeup event detection status on X-axis. Default value: 0
  uint8_t wu_ia                    		: 1;					// Sleep status bit. Default value: 0
  uint8_t sleep_state              		: 1;					// Sleep status bit. Default value: 0
  uint8_t ff_ia                    		: 1;					// Free-fall event detection status. Default value: 0
  uint8_t sleep_change_ia          		: 1;					// Detects change event in activity/inactivity status. Default value: 0
  uint8_t not_used_01              		: 1;					// 
} lsm6dso_wake_up_src_t;



#define LSM6DSO_TAP_SRC                	0x1CU
typedef struct
{
  uint8_t z_tap                    		: 1;					// Tap event detection status on Z-axis. Default value: 0
  uint8_t y_tap                    		: 1;					// Tap event detection status on Y-axis. Default value: 0
  uint8_t x_tap                   		: 1;					// Tap event detection status on X-axis. Default value: 0
  uint8_t tap_sign                 		: 1;					// Sign of acceleration detected by tap event. Default: 0
  uint8_t double_tap               		: 1;					// Double-tap event detection status. Default value: 0
  uint8_t single_tap               		: 1;					// Single-tap event status. Default value: 0
  uint8_t tap_ia                   		: 1;					// Tap event detection status. Default: 0
  uint8_t not_used_02              		: 1;					// 
} lsm6dso_tap_src_t;



#define LSM6DSO_D6D_SRC                	0x1DU
typedef struct
{
  uint8_t xl                       		: 1;					// X-axis low event (under threshold). Default value: 0
  uint8_t xh                       		: 1;					// X-axis high event (over threshold). Default value: 0
  uint8_t yl                       		: 1;					// Y-axis low event (under threshold). Default value: 0
  uint8_t yh                       		: 1;					// Y-axis high event (over threshold). Default value: 0
  uint8_t zl                       		: 1;					// Z-axis low event (under threshold). Default value: 0
  uint8_t zh                       		: 1;					// Z-axis high event (over threshold). Default value: 0
  uint8_t d6d_ia                   		: 1;					// Interrupt active for change position portrait, landscape, face-up, face-down. Default value: 0
  uint8_t den_drdy                 		: 1;					// DEN data-ready signal. It is set high when data output is related to the data coming from a DEN active condition.
} lsm6dso_d6d_src_t;



#define LSM6DSO_STATUS_REG            	0x1EU
typedef struct
{
  uint8_t xlda                     		: 1;					// Accelerometer new data available. Default value: 0
  uint8_t gda                      		: 1;					// Gyroscope new data available. Default value: 0
  uint8_t tda                      		: 1;					// Temperature new data available. Default: 0
  uint8_t not_used_01              		: 5;					// 
} lsm6dso_status_reg_t;



#define LSM6DSO_STATUS_SPIAUX         	0x1EU
typedef struct
{
  uint8_t xlda                     		: 1;					// High when the gyroscope output is in the settling phase
  uint8_t gda                      		: 1;					// Gyroscope data available (reset when one of the high parts of the output data is read)
  uint8_t gyro_settling            		: 1;					// Accelerometer data available (reset when one of the high parts of the output data is read)
  uint8_t not_used_01              		: 5;
} lsm6dso_status_spiaux_t;


/* LSM6DSO Only Read Register ----------------------------------------------------------------------*/
#define LSM6DSO_OUT_TEMP_L              	 0x20U				// Temperature data output L register (r) 
#define LSM6DSO_OUT_TEMP_H              	 0x21U				// Temperature data output H register (r) 			
#define LSM6DSO_OUTX_L_G                	 0x22U				// Angular rate sensor pitch axis (X) angular rate output L register (r).
#define LSM6DSO_OUTX_H_G                	 0x23U				// Angular rate sensor pitch axis (X) angular rate output H register (r).
#define LSM6DSO_OUTY_L_G                	 0x24U				// Angular rate sensor pitch axis (Y) angular rate output L register (r).
#define LSM6DSO_OUTY_H_G                	 0x25U				// Angular rate sensor pitch axis (Y) angular rate output H register (r).
#define LSM6DSO_OUTZ_L_G                	 0x26U				// Angular rate sensor pitch axis (Z) angular rate output L register (r).
#define LSM6DSO_OUTZ_H_G                	 0x27U				// Angular rate sensor pitch axis (Z) angular rate output H register (r).
#define LSM6DSO_OUTX_L_A                	 0x28U				// Linear acceleration sensor X-axis output L register (r).
#define LSM6DSO_OUTX_H_A                	 0x29U				// Linear acceleration sensor X-axis output H register (r).
#define LSM6DSO_OUTY_L_A                	 0x2AU				// Linear acceleration sensor Y-axis output L register (r).
#define LSM6DSO_OUTY_H_A                	 0x2BU				// Linear acceleration sensor Y-axis output H register (r).
#define LSM6DSO_OUTZ_L_A                	 0x2CU				// Linear acceleration sensor Z-axis output L register (r).
#define LSM6DSO_OUTZ_H_A                	 0x2DU				// Linear acceleration sensor Z-axis output H register (r).



#define LSM6DSO_EMB_FUNC_STATUS_MAINPAGE     0x35U				// Embedded function status register (r) Please found Datasheet Page 67

typedef struct
{
  uint8_t not_used_01             		: 3;
  uint8_t is_step_det             	    : 1;					// Interrupt status bit for step detection
  uint8_t is_tilt                 	    : 1;					// Interrupt status bit for tilt detection
  uint8_t is_sigmot               	    : 1;					// Interrupt status bit for significant motion detection
  uint8_t not_used_02             	    : 1;					// 
  uint8_t is_fsm_lc               	    : 1;					// Interrupt status bit for FSM long counter timeout interrupt event.
} lsm6dso_emb_func_status_mainpage_t;



#define LSM6DSO_FSM_STATUS_A_MAINPAGE      	0x36U
typedef struct
{
  uint8_t is_fsm1                 		: 1;					// Interrupt status bit for FSM1 interrupt event.
  uint8_t is_fsm2                 		: 1;					// Interrupt status bit for FSM2 interrupt event.
  uint8_t is_fsm3                 		: 1;					// Interrupt status bit for FSM3 interrupt event.
  uint8_t is_fsm4                 		: 1;					// Interrupt status bit for FSM4 interrupt event.
  uint8_t is_fsm5                 		: 1;					// Interrupt status bit for FSM5 interrupt event.
  uint8_t is_fsm6                 		: 1;					// Interrupt status bit for FSM6 interrupt event.
  uint8_t is_fsm7                 		: 1;					// Interrupt status bit for FSM7 interrupt event.
  uint8_t is_fsm8                 		: 1;					// Interrupt status bit for FSM8 interrupt event.
} lsm6dso_fsm_status_a_mainpage_t;

#define LSM6DSO_FSM_STATUS_B_MAINPAGE        0x37U
typedef struct
{
  uint8_t is_fsm9                 		: 1;					// Interrupt status bit for FSM9 interrupt event.
  uint8_t is_fsm10                		: 1;					// Interrupt status bit for FSM10 interrupt event.
  uint8_t is_fsm11                		: 1;					// Interrupt status bit for FSM11 interrupt event.
  uint8_t is_fsm12                		: 1;					// Interrupt status bit for FSM12 interrupt event.
  uint8_t is_fsm13                		: 1;					// Interrupt status bit for FSM13 interrupt event.
  uint8_t is_fsm14                		: 1;					// Interrupt status bit for FSM14 interrupt event.
  uint8_t is_fsm15                		: 1;					// Interrupt status bit for FSM15 interrupt event.
  uint8_t is_fsm16                		: 1;					// Interrupt status bit for FSM16 interrupt event.
} lsm6dso_fsm_status_b_mainpage_t;



#define LSM6DSO_STATUS_MASTER_MAINPAGE     	0x39U
typedef struct
{
  uint8_t sens_hub_endop          		: 1;					// Sensor hub communication status. Default value: 0
  uint8_t not_used_01             		: 2;					// 
  uint8_t slave0_nack             		: 1;					// This bit is set to 1 if Not acknowledge occurs on slave 0 communication. Default value: 0
  uint8_t slave1_nack             		: 1;					// This bit is set to 1 if Not acknowledge occurs on slave 1 communication. Default value: 0
  uint8_t slave2_nack             		: 1;					// This bit is set to 1 if Not acknowledge occurs on slave 2 communication. Default value: 0
  uint8_t slave3_nack             		: 1;					// This bit is set to 1 if Not acknowledge occurs on slave 3 communication. Default value: 0
  uint8_t wr_once_done            		: 1;					// When the bit WRITE_ONCE in MASTER_CONFIG (14h) is configured as 1, this bit is set to 1 when the 
																// write operation on slave 0 has been performed and completed. Default value: 0
} lsm6dso_status_master_mainpage_t;



#define LSM6DSO_FIFO_STATUS1               	0x3AU
typedef struct
{
  uint8_t diff_fifo                		: 8;					// Number of unread sensor data (TAG + 6 bytes) stored in FIFO
																// In conjunction with DIFF_FIFO[9:8] in FIFO_STATUS2 (3Bh).
} lsm6dso_fifo_status1_t;



#define LSM6DSO_FIFO_STATUS2               	0x3B
typedef struct
{
  uint8_t diff_fifo                		: 2;        			// Number of unread sensor data (TAG + 6 bytes) stored in FIFO. Default value: 00
  uint8_t not_used_01              		: 1;          
  uint8_t over_run_latched         		: 1;        			// Latched FIFO overrun status. Default value: 0
  uint8_t counter_bdr_ia           		: 1;       			 	// Counter BDR reaches the CNT_BDR_TH_[10:0] threshold set in COUNTER_BDR_REG1 (0Bh) and COUNTER_BDR_REG2 (0Ch). Default value: 0
  uint8_t fifo_full_ia             		: 1;        			// Smart FIFO full status. Default value: 0
  uint8_t fifo_ovr_ia              		: 1;        			// FIFO overrun status. Default value: 0
  uint8_t fifo_wtm_ia              		: 1;        			// FIFO watermark status. Default value: 0
} lsm6dso_fifo_status2_t;



#define LSM6DSO_TIMESTAMP0                   0x40U  			// Timestamp first data output register (r). The value is expressed as a 32-bit word and the bit resolution is 25 µs.
#define LSM6DSO_TIMESTAMP1                   0x41U
#define LSM6DSO_TIMESTAMP2                   0x42U
#define LSM6DSO_TIMESTAMP3                   0x43U



#define LSM6DSO_TAP_CFG0                     0x56U  // 
typedef struct
{
  uint8_t lir                           : 1;        			// Latched Interrupt. Default value: 0
  uint8_t tap_z_en                      : 1;        			// Enable Z direction in tap recognition. Default value: 0
  uint8_t tap_y_en                      : 1;        			// Enable Y direction in tap recognition. Default value: 0
  uint8_t tap_x_en                      : 1;        			// Enable X direction in tap recognition. Default value: 0
  uint8_t slope_fds                     : 1;        			// HPF or SLOPE filter selection on wake-up and Activity/Inactivity functions. Default value: 0
  uint8_t sleep_status_on_int           : 1;        			// Activity/inactivity interrupt mode configuration.
  uint8_t int_clr_on_read               : 1;        			// This bit allows immediately clearing the latched interrupts of an event detection upon the read of
                                                    			// the corresponding status register. It must be set to 1 together with LIR. Default value: 0
  uint8_t not_used_01                   : 1;
} lsm6dso_tap_cfg0_t;

#define LSM6DSO_TAP_CFG1                     0x57U
typedef struct
{
  uint8_t tap_ths_x                     : 5;           			// X-axis tap recognition threshold. Default value: 0
  uint8_t tap_priority                  : 3;
} lsm6dso_tap_cfg1_t;

enum TAP_PRIORITY
{
  TAP_PRIORITY_XYZ                      = 0,
  TAP_PRIORITY_YXZ                      = 1,
  TAP_PRIORITY_XZY                      = 2,
  TAP_PRIORITY_ZYX                      = 3,
  TAP_PRIORITY_XYZ_1                    = 4,
  TAP_PRIORITY_YZX                      = 5,
  TAP_PRIORITY_ZXY                      = 6,
  TAP_PRIORITY_ZYX_1
};



#define LSM6DSO_TAP_CFG2                     0x58U  			// Enables interrupt and inactivity functions, and tap recognition functions (r/w).
typedef struct
{
  uint8_t tap_ths_y                     : 5;        			// Y-axis tap recognition threshold. Default value: 0
  uint8_t inact_en                      : 2;        			// Enable activity/inactivity (sleep) function. Default value: 00 please found Page 73
  uint8_t interrupts_enable             : 1;        			// Enable basic interrupts (6D/4D, free-fall, wake-up, tap, inactivity). Default value: 0
} lsm6dso_tap_cfg2_t;



#define LSM6DSO_TAP_THS_6D                   0x59U
typedef struct
{
  uint8_t tap_ths_z                     : 5;      				// Z-axis recognition threshold. Default value: 0
  uint8_t sixd_ths                      : 2;      				// Threshold for 4D/6D function. Default value: 00 please found Page 74
  uint8_t d4d_en                        : 1;      				// 4D orientation detection enable. Z-axis position detection is disabled. Default value: 0
} lsm6dso_tap_ths_6d_t;



#define LSM6DSO_INT_DUR2                     0x5AU
typedef struct
{
  uint8_t shock                         : 2;      				// Maximum duration of overthreshold event. Default value: 00
  uint8_t quiet                         : 2;      				// Expected quiet time after a tap detection. Default value: 00 
  uint8_t dur                           : 4;      				// Duration of maximum time gap for double tap recognition. Default: 0000     
} lsm6dso_int_dur2_t;



#define LSM6DSO_WAKE_UP_THS                  0x5BU
typedef struct
{
  uint8_t wk_ths                        : 6;      				// Threshold for wakeup: 1 LSB weight depends on WAKE_THS_W in WAKE_UP_DUR (5Ch). Default value: 000000
  uint8_t usr_off_on_wu                 : 1;      				// Drives the low-pass filtered data with user offset correction (instead of high-pass filtered data) to the wakeup function.
  uint8_t single_double_tap             : 1;      				// Single/double-tap event enable. Default: 0
} lsm6dso_wake_up_ths_t;



#define LSM6DSO_WAKE_UP_DUR                  0x5CU
typedef struct
{
  uint8_t sleep_dur                     : 4;      				// Duration to go in sleep mode. Default value: 0000 (this corresponds to 16 ODR)
  uint8_t wake_ths_w                    : 1;      				// Weight of 1 LSB of wakeup threshold. Default: 0
  uint8_t wake_dur                      : 2;      				// Wake up duration event. Default: 00
  uint8_t ff_dur                        : 1;      				// Free fall duration event. Default: 0
} lsm6dso_wake_up_dur_t;



#define LSM6DSO_FREE_FALL                    0x5DU
typedef struct
{
  uint8_t ff_ths                        : 3;      				// Free fall threshold setting. Default: 000
  uint8_t ff_dur                        : 5;      				// Free-fall duration event. Default: 0
} lsm6dso_free_fall_t;

enum FF_THS
{
  FF_THS_156mg                          = 0,
  FF_THS_219mg                          = 1,
  FF_THS_250mg                          = 2,
  FF_THS_312mg                          = 3,
  FF_THS_344mg                          = 4,
  FF_THS_406mg                          = 5,
  FF_THS_469mg                          = 6,
  FF_THS_500g
};



#define LSM6DSO_MD1_CFG                      0x5EU
typedef struct
{
  uint8_t int1_shub                     : 1;      				// Routing of sensor hub communication concluded event on INT1. Default value: 0
  uint8_t int1_emb_func                 : 1;      				// Routing of embedded functions event on INT1. Default value: 0
  uint8_t int1_6d                       : 1;      				// Routing of 6D event on INT1. Default value: 0
  uint8_t int1_double_tap               : 1;      				// Routing of tap event on INT1. Default value: 0
  uint8_t int1_ff                       : 1;      				// Routing of free-fall event on INT1. Default value: 0
  uint8_t int1_wu                       : 1;      				// Routing of wakeup event on INT1. Default value: 0
  uint8_t int1_single_tap               : 1;      				// Routing of single-tap recognition event on INT1. Default: 0
  uint8_t int1_sleep_change             : 1;      				// Routing of activity/inactivity recognition event on INT1. Default: 0
} lsm6dso_md1_cfg_t;



#define LSM6DSO_MD2_CFG                      0x5FU
typedef struct
{
  uint8_t int2_timestamp                : 1;      				// Enables routing on INT2 pin of the alert for timestamp overflow within
  uint8_t int2_emb_func                 : 1;      				// Routing of embedded functions event on INT2. Default value: 0
  uint8_t int2_6d                       : 1;      				// Routing of 6D event on INT2. Default value: 0
  uint8_t int2_double_tap               : 1;      				// Routing of tap event on INT2. Default value: 0
  uint8_t int2_ff                       : 1;      				// Routing of free-fall event on INT2. Default value: 0
  uint8_t int2_wu                       : 1;      				// Routing of wakeup event on INT2. Default value: 0
  uint8_t int2_single_tap               : 1;      				// Single-tap recognition routing on INT2. Default: 0
  uint8_t int2_sleep_change             : 1;      				// Routing of activity/inactivity recognition event on INT2. Default: 0
} lsm6dso_md2_cfg_t;



#define LSM6DSO_I3C_BUS_AVB                  0x62U
typedef struct
{
  uint8_t pd_dis_int1                   : 1;      				// This bit allows disabling the INT1 pull-down.
  uint8_t not_used_01                   : 2;
  uint8_t i3c_bus_avb_sel               : 2;      				// These bits are used to select the bus available time when I3C IBI is used.
  uint8_t not_used_02                   : 3;
} lsm6dso_i3c_bus_avb_t;

enum I3C_Bus_Avb_Sel
{
  I3C_Bus_Avb_Sel_50u                   = 0,      				// bus available time equal to 50 µsec (default);
  I3C_Bus_Avb_Sel_2u                    = 1,
  I3C_Bus_Avb_Sel_1m                    = 2,
  I3C_Bus_Avb_Sel_25m
};



#define LSM6DSO_INTERNAL_FREQ_FINE           0x63U
typedef struct
{
  uint8_t freq_fine                     : 8;      				// Difference in percentage of the effective ODR (and Timestamp Rate) with respect to the typical. Step: 0.15%. 8-bit format, 2's complement.
} lsm6dso_internal_freq_fine_t;



#define LSM6DSO_INT_OIS                      0x6FU
typedef struct
{
  uint8_t st_xl_ois                     : 2;      				// Selects accelerometer self-test – effective only if XL OIS chain is enabled. Default value: 00
  uint8_t not_used_01                   : 3;      				// 
  uint8_t den_lh_ois                    : 1;      				// ndicates polarity of DEN signal on OIS chain
  uint8_t lvl2_ois                      : 1;      				// Enables level-sensitive latched mode on the OIS chain. Default value: 0
  uint8_t int2_drdy_ois                 : 1;      				// Enables OIS chain DRDY on INT2 pin. This setting has priority over all other INT2 settings.
} lsm6dso_int_ois_t;

enum ST_XL_OIS
{
  ST_XL_OIS_NM                          = 0,      				// Normal mode
  ST_XL_OIS_P                           = 1,      				// Positive sign self-test
  ST_XL_OIS_N                           = 2,      				// Negative sign self-test
  ST_XL_OIS_NA                                    				// not allowed
};



#define LSM6DSO_CTRL1_OIS                    0x70U
typedef struct
{
  uint8_t ois_en_spi2                   : 1;      				// Enables OIS chain data processing for gyro in Mode 3 and Mode 4 (mode4_en = 1) and accelerometer data in and Mode 4 (mode4_en = 1).
  uint8_t fs_125_ois                    : 1;      				// Selects gyroscope OIS chain full-scale 125 dps
  uint8_t fs_g_ois                      : 2;      				// Selects gyroscope OIS chain full-scale
  uint8_t mode4_en                      : 1;      				// Enables accelerometer OIS chain. 
  uint8_t sim_ois                       : 1;      				// SPI2 3- or 4-wire interface. Default value: 0
  uint8_t lvl1_ois                      : 1;      				// Enables OIS data level-sensitive trigger  
  uint8_t not_used_01                   : 1;     			 	// 
} lsm6dso_ctrl1_ois_t;

enum FS_G_OIS
{
  FS_G_OIS_250                          = 0,
  FS_G_OIS_500                          = 1,
  FS_G_OIS_1000                         = 2,
  FS_G_OIS_2000
};



#define LSM6DSO_CTRL2_OIS                    0x71U
typedef struct
{
  uint8_t hp_en_ois                     : 1;      				// Enables gyroscope OIS chain digital high-pass filter
  uint8_t ftype_ois                     : 2;      				// Selects gyroscope digital LPF1 filter bandwidth. please look Page 83 Table 151
  uint8_t not_used_01                   : 1;      
  uint8_t hpm_ois                       : 2;      				// Selects gyroscope OIS chain digital high-pass filter cutoff. Default value: 00
  uint8_t not_used_02                   : 2;
} lsm6dso_ctrl2_ois_t;



#define LSM6DSO_CTRL3_OIS                    0x72U
typedef struct
{
  uint8_t st_ois_clampdis               : 1;      				// Disables OIS chain clamp
  uint8_t st_ois                        : 2;      				// Selects gyroscope OIS chain self-test. Default value: 00
  uint8_t filter_xl_conf_ois            : 3;      				// Selects accelerometer OIS channel bandwidth. See Table 155.
  uint8_t fs_xl_ois                     : 2;      				// Selects accelerometer OIS channel full-scale. See Table 154.
} lsm6dso_ctrl3_ois_t;

#define LSM6DSO_X_OFS_USR                    0x73U      		// Accelerometer X-axis user offset correction (r/w).
#define LSM6DSO_Y_OFS_USR                    0x74U      		// Accelerometer Y-axis user offset correction (r/w). 
#define LSM6DSO_Z_OFS_USR                    0x75U      		// Accelerometer Z-axis user offset correction (r/w).



#define LSM6DSO_FIFO_DATA_OUT_TAG            0x78U
typedef struct
{
  uint8_t tag_parity                    : 1;      				// Parity check of TAG content
  uint8_t tag_cnt                       : 2;      				// 2-bit counter which identifies sensor time slot
  uint8_t tag_sensor                    : 5;      				// FIFO tag: identifies the sensor in:
} lsm6dso_fifo_data_out_tag_t;

#define LSM6DSO_FIFO_DATA_OUT_X_L            0x79U
#define LSM6DSO_FIFO_DATA_OUT_X_H            0x7AU
#define LSM6DSO_FIFO_DATA_OUT_Y_L            0x7BU
#define LSM6DSO_FIFO_DATA_OUT_Y_H            0x7CU
#define LSM6DSO_FIFO_DATA_OUT_Z_L            0x7DU
#define LSM6DSO_FIFO_DATA_OUT_Z_H            0x7EU



/* The table given below provides a list of the registers for the embedded functions available in the device and the
corresponding addresses. Embedded functions registers are accessible when FUNC_CFG_EN is set to '1' in
FUNC_CFG_ACCESS (01h).*/
#define LSM6DSO_PAGE_SEL                     0x02U
typedef struct
{
  uint8_t not_used_01              		: 4;
  uint8_t page_sel                 		: 4;      				// Select the advanced features dedicated page
} lsm6dso_page_sel_t;

#define LSM6DSO_EMB_FUNC_EN_A                0x04U
typedef struct
{
  uint8_t not_used_01              		: 3;
  uint8_t pedo_en                  		: 1;					// Enable pedometer algorithm. Default value: 0
  uint8_t tilt_en                  		: 1;					// Enable tilt calculation. Default value: 0
  uint8_t sign_motion_en           		: 1;					// Enable significant motion detection function. Default value: 0
  uint8_t not_used_02              		: 2;
} lsm6dso_emb_func_en_a_t;

#define LSM6DSO_EMB_FUNC_EN_B                0x05U
typedef struct
{
  uint8_t fsm_en                   		: 1;					// Enable Finite State Machine (FSM) feature. Default value: 0
  uint8_t not_used_01              		: 2;
  uint8_t fifo_compr_en            		: 1;					// Enable FIFO compression feature. Default value: 0
  uint8_t pedo_adv_en              		: 1;					// Enable pedometer false-positive rejection block and advanced detection feature block. Default value: 0
  uint8_t not_used_02              		: 3;
} lsm6dso_emb_func_en_b_t;

#define LSM6DSO_PAGE_ADDRESS                 0x08U				// Page address register (r/w)
typedef struct
{
  uint8_t page_addr                		: 8;
} lsm6dso_page_address_t;

#define LSM6DSO_PAGE_VALUE                   0x09U				// Page value register (r/w)
typedef struct
{
  uint8_t page_value               		: 8;
} lsm6dso_page_value_t;

#define LSM6DSO_EMB_FUNC_INT1                0x0AU
typedef struct
{
  uint8_t not_used_01              		: 3;
  uint8_t int1_step_detector       		: 1;					// Routing of pedometer step recognition event on INT1. Default value: 0
  uint8_t int1_tilt                		: 1;					// Routing of tilt event on INT1. Default value: 0
  uint8_t int1_sig_mot             		: 1;					// Routing of significant motion event on INT1. Default value: 0
  uint8_t not_used_02              		: 1;			
  uint8_t int1_fsm_lc              		: 1;					// Routing of FSM long counter timeout interrupt event on INT1. Default value: 0
} lsm6dso_emb_func_int1_t;

#define LSM6DSO_FSM_INT1_A                   0x0BU
typedef struct
{
  uint8_t int1_fsm1                		: 1;					// Routing of FSM1 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm2                		: 1;					// Routing of FSM2 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm3                		: 1;					// Routing of FSM3 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm4                		: 1;					// Routing of FSM4 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm5                		: 1;					// Routing of FSM5 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm6                		: 1;					// Routing of FSM6 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm7                		: 1;					// Routing of FSM7 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm8                		: 1;					// Routing of FSM8 interrupt event on INT1. Default value: 0
} lsm6dso_fsm_int1_a_t;

#define LSM6DSO_FSM_INT1_B                   0x0CU
typedef struct
{
  uint8_t int1_fsm9                		: 1;					//  Routing of FSM9 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm10               		: 1;					//  Routing of FSM10 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm11               		: 1;					//  Routing of FSM11 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm12               		: 1;					//  Routing of FSM12 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm13               		: 1;					//  Routing of FSM13 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm14               		: 1;					//  Routing of FSM14 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm15               		: 1;					//  Routing of FSM15 interrupt event on INT1. Default value: 0
  uint8_t int1_fsm16               		: 1;					//  Routing of FSM16 interrupt event on INT1. Default value: 0
} lsm6dso_fsm_int1_b_t;

#define LSM6DSO_EMB_FUNC_INT2                0x0EU
typedef struct
{
  uint8_t not_used_01              		: 3;
  uint8_t int2_step_detector       		: 1;					// Routing of pedometer step recognition event on INT2. Default value: 0 
  uint8_t int2_tilt                		: 1;					// Routing of tilt event on INT2. Default value: 0
  uint8_t int2_sig_mot             		: 1;					// Routing of significant motion event on INT2. Default value: 0
  uint8_t not_used_02              		: 1;
  uint8_t int2_fsm_lc              		: 1;					// Routing of FSM long counter timeout interrupt event on INT2. Default value: 0
} lsm6dso_emb_func_int2_t;

#define LSM6DSO_FSM_INT2_A                   0x0FU
typedef struct
{
  uint8_t int2_fsm1                		: 1;					// Routing of FSM1 interrupt event on INT2. Default value: 0 
  uint8_t int2_fsm2                		: 1;					// Routing of FSM2 interrupt event on INT2. Default value: 0 
  uint8_t int2_fsm3                		: 1;					// Routing of FSM3 interrupt event on INT2. Default value: 0 
  uint8_t int2_fsm4               	 	: 1;					// Routing of FSM4 interrupt event on INT2. Default value: 0 
  uint8_t int2_fsm5                		: 1;					// Routing of FSM5 interrupt event on INT2. Default value: 0 
  uint8_t int2_fsm6                		: 1;					// Routing of FSM6 interrupt event on INT2. Default value: 0 
  uint8_t int2_fsm7                		: 1;					// Routing of FSM7 interrupt event on INT2. Default value: 0
  uint8_t int2_fsm8                		: 1;					// Routing of FSM8 interrupt event on INT2. Default value: 0
} lsm6dso_fsm_int2_a_t;

#define LSM6DSO_FSM_INT2_B                   0x10U
typedef struct
{
  uint8_t int2_fsm9                		: 1;					// Routing of FSM9 interrupt event on INT2. Default value: 0
  uint8_t int2_fsm10               		: 1;					// Routing of FSM10 interrupt event on INT2. Default value: 0
  uint8_t int2_fsm11               		: 1;					// Routing of FSM11 interrupt event on INT2. Default value: 0
  uint8_t int2_fsm12               		: 1;					// Routing of FSM12 interrupt event on INT2. Default value: 0
  uint8_t int2_fsm13               		: 1;					// Routing of FSM13 interrupt event on INT2. Default value: 0
  uint8_t int2_fsm14               		: 1;					// Routing of FSM14 interrupt event on INT2. Default value: 0
  uint8_t int2_fsm15               		: 1;					// Routing of FSM15 interrupt event on INT2. Default value: 0
  uint8_t int2_fsm16               		: 1;					// Routing of FSM16 interrupt event on INT2. Default value: 0
} lsm6dso_fsm_int2_b_t;

#define LSM6DSO_EMB_FUNC_STATUS              0x12U
typedef struct
{
  uint8_t not_used_01              		: 3;
  uint8_t is_step_det              		: 1;					// Interrupt status bit for step detection 
  uint8_t is_tilt                  		: 1;					// Interrupt status bit for tilt detection 
  uint8_t is_sigmot                		: 1;					// Interrupt status bit for significant motion detection 
  uint8_t not_used_02              		: 1;
  uint8_t is_fsm_lc                		: 1;					// Interrupt status bit for FSM long counter timeout interrupt event. 
} lsm6dso_emb_func_status_t;

#define LSM6DSO_FSM_STATUS_A                 0x13U
typedef struct
{
  uint8_t is_fsm1                  		: 1;					// Interrupt status bit for FSM1 interrupt event. 
  uint8_t is_fsm2                  		: 1;					// Interrupt status bit for FSM2 interrupt event. 
  uint8_t is_fsm3                  		: 1;					// Interrupt status bit for FSM3 interrupt event. 
  uint8_t is_fsm4                  		: 1;					// Interrupt status bit for FSM4 interrupt event. 
  uint8_t is_fsm5                  		: 1;					// Interrupt status bit for FSM5 interrupt event. 
  uint8_t is_fsm6                  		: 1;					// Interrupt status bit for FSM6 interrupt event. 
  uint8_t is_fsm7                  		: 1;					// Interrupt status bit for FSM7 interrupt event. 
  uint8_t is_fsm8                  		: 1;					// Interrupt status bit for FSM8 interrupt event. 
} lsm6dso_fsm_status_a_t;

#define LSM6DSO_FSM_STATUS_B                 0x14U
typedef struct
{
  uint8_t is_fsm9                  		: 1;					// Interrupt status bit for FSM9 interrupt event. 
  uint8_t is_fsm10                 		: 1;					// Interrupt status bit for FSM10 interrupt event.  
  uint8_t is_fsm11                 		: 1;					// Interrupt status bit for FSM11 interrupt event.  
  uint8_t is_fsm12                 		: 1;					// Interrupt status bit for FSM12 interrupt event.  
  uint8_t is_fsm13                 		: 1;					// Interrupt status bit for FSM13 interrupt event.  
  uint8_t is_fsm14                 		: 1;					// Interrupt status bit for FSM14 interrupt event.  
  uint8_t is_fsm15                 		: 1;					// Interrupt status bit for FSM15 interrupt event.  
  uint8_t is_fsm16                 		: 1;					// Interrupt status bit for FSM16 interrupt event.  
} lsm6dso_fsm_status_b_t;

#define LSM6DSO_PAGE_RW                      0x17U
typedef struct
{
  uint8_t not_used_01              		: 5;
  uint8_t page_read                     : 1;					// Enable reads from the selected advanced features dedicated page.
  uint8_t page_write                  	: 1;  					// Enable writes to the selected advanced features dedicated page.
  uint8_t emb_func_lir             		: 1;					// Latched Interrupt mode for embedded functions. Default value: 0
} lsm6dso_page_rw_t;

#define LSM6DSO_EMB_FUNC_FIFO_CFG             0x44U
typedef struct
{
  uint8_t not_used_00              		: 6;
  uint8_t pedo_fifo_en             		: 1;					// Enable FIFO batching of step counter values. Default value: 0
  uint8_t not_used_01              		: 1;
} lsm6dso_emb_func_fifo_cfg_t;

#define LSM6DSO_FSM_ENABLE_A                 0x46U
typedef struct
{
  uint8_t fsm1_en                  		: 1;					// FSM1 enable. Default value: 0 
  uint8_t fsm2_en                  		: 1;					// FSM2 enable. Default value: 0
  uint8_t fsm3_en                  		: 1;					// FSM3 enable. Default value: 0 
  uint8_t fsm4_en                  		: 1;					// FSM4 enable. Default value: 0
  uint8_t fsm5_en                 	 	: 1;					// FSM5 enable. Default value: 0
  uint8_t fsm6_en                  		: 1;					// FSM6 enable. Default value: 0
  uint8_t fsm7_en                  		: 1;					// FSM7 enable. Default value: 0
  uint8_t fsm8_en                  		: 1;					// FSM8 enable. Default value: 0
} lsm6dso_fsm_enable_a_t;

#define LSM6DSO_FSM_ENABLE_B                 0x47U
typedef struct
{
  uint8_t fsm9_en                  		: 1;					// FSM9 enable. Default value: 0
  uint8_t fsm10_en                 		: 1;					// FSM10 enable. Default value: 0 
  uint8_t fsm11_en                 		: 1;					// FSM11 enable. Default value: 0 
  uint8_t fsm12_en                 		: 1;					// FSM12 enable. Default value: 0 
  uint8_t fsm13_en                 		: 1;					// FSM13 enable. Default value: 0 
  uint8_t fsm14_en                 		: 1;					// FSM14 enable. Default value: 0 
  uint8_t fsm15_en                 		: 1;					// FSM15 enable. Default value: 0 
  uint8_t fsm16_en                 		: 1;					// FSM16 enable. Default value: 0 
} lsm6dso_fsm_enable_b_t;

#define LSM6DSO_FSM_LONG_COUNTER_L           0x48U				// FSM long counter status register (r/w)
#define LSM6DSO_FSM_LONG_COUNTER_H           0x49U

#define LSM6DSO_FSM_LONG_COUNTER_CLEAR       0x4AU				// FSM long counter reset register (r/w)
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t fsm_lc_clear               	: 1;					// Clear FSM long counter value. Default value: 0
  uint8_t fsm_lc_cleared               	: 1;					// This read-only bit is automatically set to 1 when the long counter reset is done. Default value: 0 
  uint8_t not_used_01              		: 6;
} lsm6dso_fsm_long_counter_clear_t;

#define LSM6DSO_FSM_OUTS1                    0x4CU				// FSM1 output register (r)
typedef struct
{
  uint8_t n_v                      		: 1;					// FSM1 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM1 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM1 output: negative event detected on the Z-axis.
  uint8_t p_z                      		: 1;					// FSM1 output: positive event detected on the Z-axis.
  uint8_t n_y                      		: 1;					// FSM1 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM1 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM1 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM1 output: positive event detected on the X-axis.
} lsm6dso_fsm_outs1_t;

#define LSM6DSO_FSM_OUTS2                    0x4DU
typedef struct
{
  uint8_t n_v                     		: 1;					// FSM2 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM2 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM2 output: negative event detected on the Z-axis
  uint8_t p_z                      		: 1;					// FSM2 output: positive event detected on the Z-axis.
  uint8_t n_y                     	 	: 1;					// FSM2 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM2 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM2 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM2 output: positive event detected on the X-axis.	
} lsm6dso_fsm_outs2_t;

#define LSM6DSO_FSM_OUTS3                    0x4EU
typedef struct
{
  uint8_t n_v                     		: 1;					// FSM3 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM3 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM3 output: negative event detected on the Z-axis
  uint8_t p_z                      		: 1;					// FSM3 output: positive event detected on the Z-axis.
  uint8_t n_y                     	 	: 1;					// FSM3 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM3 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM3 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM3 output: positive event detected on the X-axis.	
} lsm6dso_fsm_outs3_t;

#define LSM6DSO_FSM_OUTS4                    0x4FU
typedef struct
{
  uint8_t n_v                     		: 1;					// FSM4 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM4 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM4 output: negative event detected on the Z-axis
  uint8_t p_z                      		: 1;					// FSM4 output: positive event detected on the Z-axis.
  uint8_t n_y                     	 	: 1;					// FSM4 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM4 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM4 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM4 output: positive event detected on the X-axis.	
} lsm6dso_fsm_outs4_t;

#define LSM6DSO_FSM_OUTS5                    0x50U
typedef struct
{
  uint8_t n_v                     		: 1;					// FSM5 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM5 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM5 output: negative event detected on the Z-axis
  uint8_t p_z                      		: 1;					// FSM5 output: positive event detected on the Z-axis.
  uint8_t n_y                     	 	: 1;					// FSM5 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM5 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM5 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM5 output: positive event detected on the X-axis.	
} lsm6dso_fsm_outs5_t;

#define LSM6DSO_FSM_OUTS6                    0x51U
typedef struct
{
  uint8_t n_v                     		: 1;					// FSM6 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM6 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM6 output: negative event detected on the Z-axis
  uint8_t p_z                      		: 1;					// FSM6 output: positive event detected on the Z-axis.
  uint8_t n_y                     	 	: 1;					// FSM6 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM6 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM6 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM6 output: positive event detected on the X-axis.	
} lsm6dso_fsm_outs6_t;

#define LSM6DSO_FSM_OUTS7                    0x52U
typedef struct
{
  uint8_t n_v                     		: 1;					// FSM7 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM7 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM7 output: negative event detected on the Z-axis
  uint8_t p_z                      		: 1;					// FSM7 output: positive event detected on the Z-axis.
  uint8_t n_y                     	 	: 1;					// FSM7 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM7 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM7 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM7 output: positive event detected on the X-axis.	
} lsm6dso_fsm_outs7_t;

#define LSM6DSO_FSM_OUTS8                    0x53U
typedef struct
{
  uint8_t n_v                     		: 1;					// FSM8 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM8 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM8 output: negative event detected on the Z-axis
  uint8_t p_z                      		: 1;					// FSM8 output: positive event detected on the Z-axis.
  uint8_t n_y                     	 	: 1;					// FSM8 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM8 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM8 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM8 output: positive event detected on the X-axis.	
} lsm6dso_fsm_outs8_t;

#define LSM6DSO_FSM_OUTS9                    0x54U
typedef struct
{
  uint8_t n_v                     		: 1;					// FSM9 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM9 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM9 output: negative event detected on the Z-axis
  uint8_t p_z                      		: 1;					// FSM9 output: positive event detected on the Z-axis.
  uint8_t n_y                     	 	: 1;					// FSM9 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM9 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM9 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM9 output: positive event detected on the X-axis.	
} lsm6dso_fsm_outs9_t;

#define LSM6DSO_FSM_OUTS10                   0x55U
typedef struct
{
  uint8_t n_v                      		: 1;					// FSM10 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM10 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM10 output: negative event detected on the Z-axis.
  uint8_t p_z                      		: 1;					// FSM10 output: positive event detected on the Z-axis.
  uint8_t n_y                      		: 1;					// FSM10 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM10 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM10 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM10 output: positive event detected on the X-axis.
} lsm6dso_fsm_outs10_t;

#define LSM6DSO_FSM_OUTS11                   0x56U
typedef struct
{
  uint8_t n_v                      		: 1;					// FSM11 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM11 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM11 output: negative event detected on the Z-axis.
  uint8_t p_z                      		: 1;					// FSM11 output: positive event detected on the Z-axis.
  uint8_t n_y                      		: 1;					// FSM11 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM11 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM11 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM11 output: positive event detected on the X-axis.
} lsm6dso_fsm_outs11_t;

#define LSM6DSO_FSM_OUTS12                   0x57U
typedef struct
{
  uint8_t n_v                      		: 1;					// FSM12 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM12 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM12 output: negative event detected on the Z-axis.
  uint8_t p_z                      		: 1;					// FSM12 output: positive event detected on the Z-axis.
  uint8_t n_y                      		: 1;					// FSM12 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM12 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM12 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM12 output: positive event detected on the X-axis.
} lsm6dso_fsm_outs12_t;

#define LSM6DSO_FSM_OUTS13                   0x58U
typedef struct
{
  uint8_t n_v                      		: 1;					// FSM13 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM13 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM13 output: negative event detected on the Z-axis.
  uint8_t p_z                      		: 1;					// FSM13 output: positive event detected on the Z-axis.
  uint8_t n_y                      		: 1;					// FSM13 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM13 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM13 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM13 output: positive event detected on the X-axis.
} lsm6dso_fsm_outs13_t;

#define LSM6DSO_FSM_OUTS14                   0x59U
typedef struct
{
  uint8_t n_v                      		: 1;					// FSM14 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM14 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM14 output: negative event detected on the Z-axis.
  uint8_t p_z                      		: 1;					// FSM14 output: positive event detected on the Z-axis.
  uint8_t n_y                      		: 1;					// FSM14 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM14 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM14 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM14 output: positive event detected on the X-axis.
} lsm6dso_fsm_outs14_t;

#define LSM6DSO_FSM_OUTS15                   0x5AU
typedef struct
{
  uint8_t n_v                      		: 1;					// FSM15 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM15 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM15 output: negative event detected on the Z-axis.
  uint8_t p_z                      		: 1;					// FSM15 output: positive event detected on the Z-axis.
  uint8_t n_y                      		: 1;					// FSM15 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM15 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM15 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM15 output: positive event detected on the X-axis.
} lsm6dso_fsm_outs15_t;

#define LSM6DSO_FSM_OUTS16                   0x5BU
typedef struct
{
  uint8_t n_v                      		: 1;					// FSM16 output: negative event detected on the vector.
  uint8_t p_v                      		: 1;					// FSM16 output: positive event detected on the vector.
  uint8_t n_z                      		: 1;					// FSM16 output: negative event detected on the Z-axis.
  uint8_t p_z                      		: 1;					// FSM16 output: positive event detected on the Z-axis.
  uint8_t n_y                      		: 1;					// FSM16 output: negative event detected on the Y-axis.
  uint8_t p_y                      		: 1;					// FSM16 output: positive event detected on the Y-axis.
  uint8_t n_x                      		: 1;					// FSM16 output: negative event detected on the X-axis.
  uint8_t p_x                      		: 1;					// FSM16 output: positive event detected on the X-axis.
} lsm6dso_fsm_outs16_t;

#define LSM6DSO_EMB_FUNC_ODR_CFG_B           0x5FU
typedef struct
{
  uint8_t not_used_01              		: 3;
  uint8_t fsm_odr                  		: 2;					// Finite State Machine ODR configuration:
  uint8_t not_used_02              		: 3;
} lsm6dso_emb_func_odr_cfg_b_t;

enum FSM_ODR
{
	FSM_ODR12_5HZ						= 0,
	FSM_ODR26HZ							= 1,
	FSM_ODR52HZ							= 2,
	FSM_ODR104HZ
};

#define LSM6DSO_STEP_COUNTER_L               0x62U				// Step counter output register (r)
#define LSM6DSO_STEP_COUNTER_H               0x63U

#define LSM6DSO_EMB_FUNC_SRC                 0x64U				// Embedded function source register (r/w)
typedef struct
{
  uint8_t not_used_01              		: 2;
  uint8_t stepcounter_bit_set      		: 1;					// This bit is equal to 1 when the step count is increased.
  uint8_t step_overflow            		: 1;					// Step counter overflow status. Read-only bit.
  uint8_t step_count_delta_ia      		: 1;					// Pedometer step recognition on delta time status. Read-only bit.
  uint8_t step_detected            		: 1;					// Step detector event detection status. Read-only bit.
  uint8_t not_used_02              		: 1;
  uint8_t pedo_rst_step            		: 1;					// Reset pedometer step counter. Read/write bit.
} lsm6dso_emb_func_src_t;

#define LSM6DSO_EMB_FUNC_INIT_A              0x66U				// Embedded functions initialization register (r/w)
typedef struct
{
  uint8_t not_used_01               	: 3;
  uint8_t step_det_init             	: 1;					// Pedometer step counter/detector algorithm initialization request.
  uint8_t tilt_init                 	: 1;					// Tilt algorithm initialization request. Default value: 0
  uint8_t sig_mot_init              	: 1;					// Significant motion detection algorithm initialization request. Default value: 0
  uint8_t not_used_02               	: 2;
} lsm6dso_emb_func_init_a_t;

#define LSM6DSO_EMB_FUNC_INIT_B              0x67U				// Embedded functions initialization register (r/w)
typedef struct
{
  uint8_t fsm_init                 		: 1;					// FSM initialization request. Default value: 0s
  uint8_t not_used_01              		: 2;
  uint8_t fifo_compr_init          		: 1;					// FIFO compression feature initialization request. Default value: 0
  uint8_t not_used_02              		: 4;
} lsm6dso_emb_func_init_b_t;

#define LSM6DSO_MAG_SENSITIVITY_L            0xBAU				// External magnetometer sensitivity (LSbyte). Default value: 00100100
#define LSM6DSO_MAG_SENSITIVITY_H            0xBBU				// External magnetometer sensitivity (MSbyte). Default value: 00010110
#define LSM6DSO_MAG_OFFX_L                   0xC0U				// Offset for X-axis hard-iron compensation (LSbyte). Default value: 00000000
#define LSM6DSO_MAG_OFFX_H                   0xC1U				// Offset for X-axis hard-iron compensation (MSbyte). Default value: 00000000
#define LSM6DSO_MAG_OFFY_L                   0xC2U				// Offset for Y-axis hard-iron compensation (LSbyte). Default value: 00000000
#define LSM6DSO_MAG_OFFY_H                   0xC3U				// Offset for Y-axis hard-iron compensation (MSbyte). Default value: 00000000
#define LSM6DSO_MAG_OFFZ_L                   0xC4U				// Offset for Z-axis hard-iron compensation (LSbyte). Default value: 00000000
#define LSM6DSO_MAG_OFFZ_H                   0xC5U				// Offset for Z-axis hard-iron compensation (MSbyte). Default value: 00000000
#define LSM6DSO_MAG_SI_XX_L                  0xC6U				// Soft-iron correction row1 col1 coefficient (LSbyte). Default value: 00000000
#define LSM6DSO_MAG_SI_XX_H                  0xC7U				// Soft-iron correction row1 col1 coefficient (MSbyte). Default value: 00111100
#define LSM6DSO_MAG_SI_XY_L                  0xC8U				// Soft-iron correction row1 col2 (and row2 col1) coefficient (LSbyte). Default value: 00000000
#define LSM6DSO_MAG_SI_XY_H                  0xC9U				// Soft-iron correction row1 col2 (and row2 col1) coefficient (MSbyte). Default value: 00000000
#define LSM6DSO_MAG_SI_XZ_L                  0xCAU				// Soft-iron correction row1 col3 (and row3 col1) coefficient (LSbyte). Default value: 00000000
#define LSM6DSO_MAG_SI_XZ_H                  0xCBU				// Soft-iron correction row1 col3 (and row3 col1) coefficient (MSbyte). Default value: 00000000
#define LSM6DSO_MAG_SI_YY_L                  0xCCU				// Soft-iron correction row2 col2 coefficient (LSbyte). Default value: 00000000
#define LSM6DSO_MAG_SI_YY_H                  0xCDU				// Soft-iron correction row2 col2 coefficient (MSbyte). Default value: 00111100
#define LSM6DSO_MAG_SI_YZ_L                  0xCEU				// Soft-iron correction row2 col3 (and row3 col2) coefficient (LSbyte).
#define LSM6DSO_MAG_SI_YZ_H                  0xCFU				// Soft-iron correction row2 col3 (and row3 col2) coefficient (MSbyte).
#define LSM6DSO_MAG_SI_ZZ_L                  0xD0U				// Soft-iron correction row3 col3 coefficient (LSbyte). Default value: 00000000
#define LSM6DSO_MAG_SI_ZZ_H                  0xD1U				// Soft-iron correction row3 col3 coefficient (MSbyte). Default value: 00111100



#define LSM6DSO_MAG_CFG_A                    0xD4U				// External magnetometer coordinates (Y and Z axes) rotation register (r/w)
typedef struct
{
  uint8_t mag_z_axis               		: 3;					// Magnetometer Z-axis coordinates rotation (to be aligned to accelerometer/gyroscope axes orientation)
  uint8_t not_used_01              		: 1;					// Please look for datasheet page 131
  uint8_t mag_y_axis               		: 3;					// Magnetometer Y-axis coordinates rotation (to be aligned to accelerometer/gyroscope axes orientation)
  uint8_t not_used_02              		: 1;
} lsm6dso_mag_cfg_a_t;



#define LSM6DSO_MAG_CFG_B                    0xD5U				// External magnetometer coordinates (X-axis) rotation register (r/w).
typedef struct
{
  uint8_t mag_x_axis               		: 3;					// Magnetometer X-axis coordinates rotation (to be aligned to accelerometer/gyroscope axes orientation)
  uint8_t not_used_01              		: 5;
} lsm6dso_mag_cfg_b_t;


#define LSM6DSO_FSM_LC_TIMEOUT_L             0x17AU				// FSM long counter timeout value (LSbyte). Default value: 00000000
#define LSM6DSO_FSM_LC_TIMEOUT_H             0x17BU				// FSM long counter timeout value (MSbyte). Default value: 00000000
#define LSM6DSO_FSM_PROGRAMS                 0x17CU				// Number of FSM programs; must be less than or equal to 16.
#define LSM6DSO_FSM_START_ADD_L              0x17EU				// FSM start address register (r/w). First available address is 0x033C.
#define LSM6DSO_FSM_START_ADD_H              0x17FU				// 

#define LSM6DSO_PEDO_CMD_REG                 0x183U				// Pedometer configuration register (r/w)
typedef struct
{
  uint8_t ad_det_en                		: 1;					// Enables the advanced detection feature.
  uint8_t not_used_01              		: 1;
  uint8_t fp_rejection_en          		: 1;					// Enables the false-positive rejection feature.
  uint8_t carry_count_en           		: 1;					// Set when user wants to generate interrupt only on count overflow event.
  uint8_t not_used_02              		: 4;
} lsm6dso_pedo_cmd_reg_t;

#define LSM6DSO_PEDO_DEB_STEPS_CONF          0x184U				// Pedometer debounce configuration register (r/w)
#define LSM6DSO_PEDO_SC_DELTAT_L             0x1D0U				// Time period register for step detection on delta time (r/w)
#define LSM6DSO_PEDO_SC_DELTAT_H             0x1D1U				// 





/*The table given below provides a list of the registers for the sensor hub functions available in the device and the
corresponding addresses. The sensor hub registers are accessible when bit SHUB_REG_ACCESS is set to '1' in
FUNC_CFG_ACCESS (01h).*/
#define LSM6DSO_SENSOR_HUB_1                 0x02U				// Sensor hub1 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_1_t;

#define LSM6DSO_SENSOR_HUB_2                 0x03U				// Sensor hub2 output register (r)
typedef struct
{
  uint8_t bit0                   		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_2_t;

#define LSM6DSO_SENSOR_HUB_3                 0x04U				// Sensor hub3 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                   		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_3_t;

#define LSM6DSO_SENSOR_HUB_4                 0x05U				// Sensor hub4 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_4_t;

#define LSM6DSO_SENSOR_HUB_5                 0x06U				// Sensor hub5 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_5_t;

#define LSM6DSO_SENSOR_HUB_6                 0x07U				// Sensor hub6 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                   	 	: 1;
} lsm6dso_sensor_hub_6_t;

#define LSM6DSO_SENSOR_HUB_7                 0x08U				// Sensor hub7 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_7_t;

#define LSM6DSO_SENSOR_HUB_8                 0x09U				// Sensor hub8 output register (r)
typedef struct
{
  uint8_t bit0                   	 	: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_8_t;

#define LSM6DSO_SENSOR_HUB_9                 0x0AU				// Sensor hub9 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                   		: 1;
  uint8_t bit5                   	 	: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_9_t;

#define LSM6DSO_SENSOR_HUB_10                0x0BU				// Sensor hub10 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_10_t;

#define LSM6DSO_SENSOR_HUB_11                0x0CU				// Sensor hub11 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_11_t;

#define LSM6DSO_SENSOR_HUB_12                0x0DU				// Sensor hub12 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_12_t;

#define LSM6DSO_SENSOR_HUB_13                0x0EU				// Sensor hub13 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_13_t;

#define LSM6DSO_SENSOR_HUB_14                0x0FU				// Sensor hub14 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_14_t;

#define LSM6DSO_SENSOR_HUB_15                0x10U				// Sensor hub15 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_15_t;

#define LSM6DSO_SENSOR_HUB_16                0x11U				// Sensor hub16 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_16_t;

#define LSM6DSO_SENSOR_HUB_17                0x12U				// Sensor hub17 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_17_t;

#define LSM6DSO_SENSOR_HUB_18                0x13U				// Sensor hub18 output register (r)
typedef struct
{
  uint8_t bit0                    		: 1;
  uint8_t bit1                    		: 1;
  uint8_t bit2                    		: 1;
  uint8_t bit3                    		: 1;
  uint8_t bit4                    		: 1;
  uint8_t bit5                    		: 1;
  uint8_t bit6                    		: 1;
  uint8_t bit7                    		: 1;
} lsm6dso_sensor_hub_18_t;

#define LSM6DSO_MASTER_CONFIG                0x14U				// Master configuration register (r/w)
typedef struct
{
  uint8_t aux_sens_on              		: 2;					// Number of external sensors to be read by the sensor hub.
  uint8_t master_on                		: 1;					// Sensor hub I²C master enable. Default: 0
  uint8_t shub_pu_en               		: 1;					// Master I²C pull-up enable. Default value: 0
  uint8_t pass_through_mode        		: 1;					// I²C interface pass-through. Default value: 0
  uint8_t start_config             		: 1;					// Sensor hub trigger signal selection. Default value: 0
  uint8_t write_once              		: 1;					// Slave 0 write operation is performed only at the first sensor hub cycle. Default value: 0
  uint8_t rst_master_regs          		: 1;					// Reset Master logic and output registers. Must be set to ‘1’ and then set it to ‘0’. Default value: 0
} lsm6dso_master_config_t;

#define LSM6DSO_SLV0_ADD                     0x15U				// I²C slave address of the first external sensor (Sensor 1) register (r/w).
typedef struct
{
  uint8_t rw_0                     		: 1;					// Read/write operation on Sensor 1. Default value: 0	
  uint8_t slave0                   		: 7;					// I²C slave address of Sensor1 that can be read by the sensor hub.
} lsm6dso_slv0_add_t;

#define LSM6DSO_SLV0_SUBADD                  0x16U				// Address of register on the first external sensor (Sensor 1) register (r/w)
typedef struct
{
  uint8_t slave0_reg               		: 8;					// Address of register on Sensor1 that has to be read/written according to the rw_0 bit value in SLV0_ADD (15h). Default value: 00000000
} lsm6dso_slv0_subadd_t;

#define LSM6DSO_SLV0_CONFIG                  0x17U				// First external sensor (Sensor1) configuration and sensor hub settings register (r/w)
typedef struct	
{
  uint8_t slave0_numop             		: 3;					// Number of read operations on Sensor 1. Default value: 000
  uint8_t batch_ext_sens_0_en      		: 1;					// Enable FIFO batching data of first slave. Default value: 0
  uint8_t not_used_01              		: 2;					// 
  uint8_t shub_odr                 		: 2;					// Rate at which the master communicates. Default value: 00
} lsm6dso_slv0_config_t;

enum SHUB_ODR
{
	SHUB_ODR_104_HZ						= 0,					// 104 Hz (or at the maximum ODR between the accelerometer and gyro if it is less than 104 Hz)
	SHUB_ODR_52_HZ						= 1,					// 52 Hz (or at the maximum ODR between the accelerometer and gyro if it is less than 52 Hz);
	SHUB_ODR_26_HZ						= 2,					// 26 Hz (or at the maximum ODR between the accelerometer and gyro if it is less than 26 Hz);
	SHUB_ODR_12_5_HZ											// 12.5 Hz (or at the maximum ODR between the accelerometer and gyro if it is less than 12.5 Hz)
};


#define LSM6DSO_SLV1_ADD                     0x18U				// I²C slave address of the second external sensor (Sensor 2) register (r/w)
typedef struct
{
  uint8_t r_1                     		: 1;					// Read operation on Sensor 2 enable. Default value: 0
  uint8_t slave1_add               		: 7;					// I²C slave address of Sensor 2 that can be read by the sensor hub.
} lsm6dso_slv1_add_t;

#define LSM6DSO_SLV1_SUBADD                  0x19U				// Address of register on the second external sensor (Sensor 2) register (r/w)
typedef struct
{
  uint8_t slave1_reg               		: 8;
} lsm6dso_slv1_subadd_t;

#define LSM6DSO_SLV1_CONFIG                  0x1AU				// Second external sensor (Sensor 2) configuration register (r/w)
typedef struct
{
  uint8_t slave1_numop            	 	: 3;					// Number of read operations on Sensor 2. Default value: 000
  uint8_t batch_ext_sens_1_en      		: 1;					// Enable FIFO batching data of second slave. Default value: 0
  uint8_t not_used_01              		: 4;
} lsm6dso_slv1_config_t;

#define LSM6DSO_SLV2_ADD                     0x1BU				// I²C slave address of the third external sensor (Sensor 3) register (r/w)
typedef struct
{
  uint8_t r_2                      		: 1;					// Read operation on Sensor 3 enable. Default value: 0
  uint8_t slave2_add               		: 7;					// I²C slave address of Sensor 3 that can be read by the sensor hub.
} lsm6dso_slv2_add_t;

#define LSM6DSO_SLV2_SUBADD                  0x1CU				// Address of register on the third external sensor (Sensor 3) register (r/w)
typedef struct
{
  uint8_t slave2_reg               		: 8;
} lsm6dso_slv2_subadd_t;

#define LSM6DSO_SLV2_CONFIG                  0x1DU				// Third external sensor (Sensor 3) configuration register (r/w)
typedef struct
{
  uint8_t slave2_numop             		: 3;					// Number of read operations on Sensor 3. Default value: 000
  uint8_t batch_ext_sens_2_en      		: 1;					// Enable FIFO batching data of third slave. Default value: 0
  uint8_t not_used_01              		: 4;
} lsm6dso_slv2_config_t;

#define LSM6DSO_SLV3_ADD                     0x1EU				// I²C slave address of the fourth external sensor (Sensor 4) register (r/w)
typedef struct
{
  uint8_t r_3                      		: 1;					// Read operation on Sensor 4 enable. Default value: 0
  uint8_t slave3_add               		: 7;					// I²C slave address of Sensor 4 that can be read by the sensor hub.
} lsm6dso_slv3_add_t;

#define LSM6DSO_SLV3_SUBADD                  0x1FU				// Address of register on the fourth external sensor (Sensor 4) register (r/w)
typedef struct
{
  uint8_t slave3_reg               		: 8;
} lsm6dso_slv3_subadd_t;

#define LSM6DSO_SLV3_CONFIG                  0x20U				// Fourth external sensor (Sensor 4) configuration register (r/w)
typedef struct
{
  uint8_t slave3_numop             		: 3;					// Number of read operations on Sensor 4. Default value: 000
  uint8_t  batch_ext_sens_3_en     		: 1;					// Enable FIFO batching data of fourth slave. Default value: 0
  uint8_t not_used_01              		: 4;
} lsm6dso_slv3_config_t;

#define LSM6DSO_DATAWRITE_SLV0               0x21U				// Data to be written into the slave device register (r/w)
typedef struct
{
  uint8_t slave0_dataw             		: 8;
} lsm6dso_datawrite_src_mode_sub_slv0_t;

#define LSM6DSO_STATUS_MASTER                0x22U				// Sensor hub source register (r)
typedef struct
{
  uint8_t sens_hub_endop           		: 1;					// Sensor hub communication status. Default value: 0
  uint8_t not_used_01              		: 2;
  uint8_t slave0_nack              		: 1;					// This bit is set to 1 if Not acknowledge occurs on slave 0 communication. Default value: 0
  uint8_t slave1_nack              		: 1;					// This bit is set to 1 if Not acknowledge occurs on slave 1 communication. Default value: 0
  uint8_t slave2_nack              		: 1;					// This bit is set to 1 if Not acknowledge occurs on slave 2 communication. Default value: 0
  uint8_t slave3_nack              		: 1;					// This bit is set to 1 if Not acknowledge occurs on slave 3 communication. Default value: 0
  uint8_t wr_once_done             		: 1;					// When the bit WRITE_ONCE in MASTER_CONFIG (14h) is configured as 1, this bit is set to 1 when the
																// write operation on slave 0 has been performed and completed. Default value: 0
} lsm6dso_status_master_t;

#define LSM6DSO_START_FSM_ADD                0x0400U

/**
  * @defgroup LSM6DSO_Register_Union
  * @brief    This union group all the registers having a bit-field
  *           description.
  *           This union is useful but it's not needed by the driver.
  *
  *           REMOVING this union you are compliant with:
  *           MISRA-C 2012 [Rule 19.2] -> " Union are not allowed "
  *
  * @{
  *
  */
typedef union
{
  lsm6dso_func_cfg_access_t               	 func_cfg_access;
  lsm6dso_pin_ctrl_t                      	 pin_ctrl;
  lsm6dso_fifo_ctrl1_t                    	 fifo_ctrl1;
  lsm6dso_fifo_ctrl2_t                    	 fifo_ctrl2;
  lsm6dso_fifo_ctrl3_t                    	 fifo_ctrl3;
  lsm6dso_fifo_ctrl4_t                    	 fifo_ctrl4;
  lsm6dso_counter_bdr_reg1_t              	 counter_bdr_reg1;
  lsm6dso_counter_bdr_reg2_t              	 counter_bdr_reg2;
  lsm6dso_int1_ctrl_t                     	 int1_ctrl;
  lsm6dso_int2_ctrl_t                     	 int2_ctrl;
  lsm6dso_ctrl1_xl_t                     	 ctrl1_xl;
  lsm6dso_ctrl2_g_t                      	 ctrl2_g;
  lsm6dso_ctrl3_c_t                      	 ctrl3_c;
  lsm6dso_ctrl4_c_t                      	 ctrl4_c;
  lsm6dso_ctrl5_c_t                      	 ctrl5_c;
  lsm6dso_ctrl6_c_t                      	 ctrl6_c;
  lsm6dso_ctrl7_g_t                      	 ctrl7_g;
  lsm6dso_ctrl8_xl_t                     	 ctrl8_xl;
  lsm6dso_ctrl9_xl_t                     	 ctrl9_xl;
  lsm6dso_ctrl10_c_t                     	 ctrl10_c;
  lsm6dso_all_int_src_t                  	 all_int_src;
  lsm6dso_wake_up_src_t                  	 wake_up_src;
  lsm6dso_tap_src_t                      	 tap_src;
  lsm6dso_d6d_src_t                      	 d6d_src;
  lsm6dso_status_reg_t                   	 status_reg;
  lsm6dso_status_spiaux_t                	 status_spiaux;
  lsm6dso_fifo_status1_t                 	 fifo_status1;
  lsm6dso_fifo_status2_t                 	 fifo_status2;
  lsm6dso_tap_cfg0_t                     	 tap_cfg0;
  lsm6dso_tap_cfg1_t                     	 tap_cfg1;
  lsm6dso_tap_cfg2_t                     	 tap_cfg2;
  lsm6dso_tap_ths_6d_t                   	 tap_ths_6d;
  lsm6dso_int_dur2_t                     	 int_dur2;
  lsm6dso_wake_up_ths_t                  	 wake_up_ths;
  lsm6dso_wake_up_dur_t                  	 wake_up_dur;
  lsm6dso_free_fall_t                    	 free_fall;
  lsm6dso_md1_cfg_t                      	 md1_cfg;
  lsm6dso_md2_cfg_t                      	 md2_cfg;
  lsm6dso_i3c_bus_avb_t                  	 i3c_bus_avb;
  lsm6dso_internal_freq_fine_t           	 internal_freq_fine;
  lsm6dso_int_ois_t                      	 int_ois;
  lsm6dso_ctrl1_ois_t                    	 ctrl1_ois;
  lsm6dso_ctrl2_ois_t                    	 ctrl2_ois;
  lsm6dso_ctrl3_ois_t                    	 ctrl3_ois;
  lsm6dso_fifo_data_out_tag_t            	 fifo_data_out_tag;
  lsm6dso_page_sel_t                     	 page_sel;
  lsm6dso_emb_func_en_a_t                	 emb_func_en_a;
  lsm6dso_emb_func_en_b_t                	 emb_func_en_b;
  lsm6dso_page_address_t                 	 page_address;
  lsm6dso_page_value_t                   	 page_value;
  lsm6dso_emb_func_int1_t                 	 emb_func_int1;
  lsm6dso_fsm_int1_a_t                    	 fsm_int1_a;
  lsm6dso_fsm_int1_b_t                   	 fsm_int1_b;
  lsm6dso_emb_func_int2_t                	 emb_func_int2;
  lsm6dso_fsm_int2_a_t                   	 fsm_int2_a;
  lsm6dso_fsm_int2_b_t                   	 fsm_int2_b;
  lsm6dso_emb_func_status_t              	 emb_func_status;
  lsm6dso_fsm_status_a_t                 	 fsm_status_a;
  lsm6dso_fsm_status_b_t                 	 fsm_status_b;
  lsm6dso_page_rw_t                      	 page_rw;
  lsm6dso_emb_func_fifo_cfg_t             	 emb_func_fifo_cfg;
  lsm6dso_fsm_enable_a_t                 	 fsm_enable_a;
  lsm6dso_fsm_enable_b_t                 	 fsm_enable_b;
  lsm6dso_fsm_long_counter_clear_t       	 fsm_long_counter_clear;
  lsm6dso_fsm_outs1_t                    	 fsm_outs1;
  lsm6dso_fsm_outs2_t                    	 fsm_outs2;
  lsm6dso_fsm_outs3_t                    	 fsm_outs3;
  lsm6dso_fsm_outs4_t                    	 fsm_outs4;
  lsm6dso_fsm_outs5_t                    	 fsm_outs5;
  lsm6dso_fsm_outs6_t                    	 fsm_outs6;
  lsm6dso_fsm_outs7_t                    	 fsm_outs7;
  lsm6dso_fsm_outs8_t                    	 fsm_outs8;
  lsm6dso_fsm_outs9_t                    	 fsm_outs9;
  lsm6dso_fsm_outs10_t                   	 fsm_outs10;
  lsm6dso_fsm_outs11_t                   	 fsm_outs11;
  lsm6dso_fsm_outs12_t                   	 fsm_outs12;
  lsm6dso_fsm_outs13_t                   	 fsm_outs13;
  lsm6dso_fsm_outs14_t                   	 fsm_outs14;
  lsm6dso_fsm_outs15_t                   	 fsm_outs15;
  lsm6dso_fsm_outs16_t                   	 fsm_outs16;
  lsm6dso_emb_func_odr_cfg_b_t           	 emb_func_odr_cfg_b;
  lsm6dso_emb_func_src_t                 	 emb_func_src;
  lsm6dso_emb_func_init_a_t              	 emb_func_init_a;
  lsm6dso_emb_func_init_b_t              	 emb_func_init_b;
  lsm6dso_mag_cfg_a_t                    	 mag_cfg_a;
  lsm6dso_mag_cfg_b_t                    	 mag_cfg_b;
  lsm6dso_pedo_cmd_reg_t                 	 pedo_cmd_reg;
  lsm6dso_sensor_hub_1_t                 	 sensor_hub_1;
  lsm6dso_sensor_hub_2_t                 	 sensor_hub_2;
  lsm6dso_sensor_hub_3_t                 	 sensor_hub_3;
  lsm6dso_sensor_hub_4_t                 	 sensor_hub_4;
  lsm6dso_sensor_hub_5_t                 	 sensor_hub_5;
  lsm6dso_sensor_hub_6_t                 	 sensor_hub_6;
  lsm6dso_sensor_hub_7_t                 	 sensor_hub_7;
  lsm6dso_sensor_hub_8_t                 	 sensor_hub_8;
  lsm6dso_sensor_hub_9_t                 	 sensor_hub_9;
  lsm6dso_sensor_hub_10_t                	 sensor_hub_10;
  lsm6dso_sensor_hub_11_t                	 sensor_hub_11;
  lsm6dso_sensor_hub_12_t                	 sensor_hub_12;
  lsm6dso_sensor_hub_13_t                	 sensor_hub_13;
  lsm6dso_sensor_hub_14_t                	 sensor_hub_14;
  lsm6dso_sensor_hub_15_t                	 sensor_hub_15;
  lsm6dso_sensor_hub_16_t                	 sensor_hub_16;
  lsm6dso_sensor_hub_17_t                	 sensor_hub_17;
  lsm6dso_sensor_hub_18_t                	 sensor_hub_18;
  lsm6dso_master_config_t                	 master_config;
  lsm6dso_slv0_add_t                     	 slv0_add;
  lsm6dso_slv0_subadd_t                  	 slv0_subadd;
  lsm6dso_slv0_config_t                  	 slv0_config;
  lsm6dso_slv1_add_t                     	 slv1_add;
  lsm6dso_slv1_subadd_t                  	 slv1_subadd;
  lsm6dso_slv1_config_t                   	 slv1_config;
  lsm6dso_slv2_add_t                     	 slv2_add;
  lsm6dso_slv2_subadd_t                  	 slv2_subadd;
  lsm6dso_slv2_config_t                  	 slv2_config;
  lsm6dso_slv3_add_t                     	 slv3_add;
  lsm6dso_slv3_subadd_t                  	 slv3_subadd;
  lsm6dso_slv3_config_t                  	 slv3_config;
  lsm6dso_datawrite_src_mode_sub_slv0_t  	 datawrite_src_mode_sub_slv0;
  lsm6dso_status_master_t                	 status_master;
  uint8_t                                	 byte;
} lsm6dso_reg_t;

/**
  * @}
  *
  */

float_t lsm6dso_from_fs2_to_mg(int16_t lsb);
float_t lsm6dso_from_fs4_to_mg(int16_t lsb);
float_t lsm6dso_from_fs8_to_mg(int16_t lsb);
float_t lsm6dso_from_fs16_to_mg(int16_t lsb);

float_t lsm6dso_from_fs125_to_mdps(int16_t lsb);
float_t lsm6dso_from_fs500_to_mdps(int16_t lsb);
float_t lsm6dso_from_fs250_to_mdps(int16_t lsb);
float_t lsm6dso_from_fs1000_to_mdps(int16_t lsb);
float_t lsm6dso_from_fs2000_to_mdps(int16_t lsb);

float_t lsm6dso_from_lsb_to_celsius(int16_t lsb);

float_t lsm6dso_from_lsb_to_nsec(int16_t lsb);



typedef enum
{
  LSM6DSO_LSb_1mg  = 0,
  LSM6DSO_LSb_16mg = 1,
} lsm6dso_usr_off_w_t;

typedef enum
{
  LSM6DSO_HIGH_PERFORMANCE_MD  = 0,
  LSM6DSO_LOW_NORMAL_POWER_MD  = 1,
  LSM6DSO_ULTRA_LOW_POWER_MD   = 2,
} lsm6dso_xl_hm_mode_t;

typedef enum
{
  LSM6DSO_GY_HIGH_PERFORMANCE  = 0,
  LSM6DSO_GY_NORMAL            = 1,
} lsm6dso_g_hm_mode_t;

typedef enum
{
  LSM6DSO_NO_ROUND      = 0,
  LSM6DSO_ROUND_XL      = 1,
  LSM6DSO_ROUND_GY      = 2,
  LSM6DSO_ROUND_GY_XL   = 3,
} lsm6dso_rounding_t;

typedef enum
{
  LSM6DSO_USER_BANK           = 0,
  LSM6DSO_SENSOR_HUB_BANK     = 1,
  LSM6DSO_EMBEDDED_FUNC_BANK  = 2,
} lsm6dso_reg_access_t;

typedef enum
{
  LSM6DSO_DRDY_LATCHED = 0,
  LSM6DSO_DRDY_PULSED  = 1,
} lsm6dso_dataready_pulsed_t;

typedef enum
{
  LSM6DSO_XL_ST_DISABLE  = 0,
  LSM6DSO_XL_ST_POSITIVE = 1,
  LSM6DSO_XL_ST_NEGATIVE = 2,
} lsm6dso_st_xl_t;

typedef enum
{
  LSM6DSO_GY_ST_DISABLE  = 0,
  LSM6DSO_GY_ST_POSITIVE = 1,
  LSM6DSO_GY_ST_NEGATIVE = 3,
} lsm6dso_st_g_t;

typedef enum
{
  LSM6DSO_ULTRA_LIGHT  = 0,
  LSM6DSO_VERY_LIGHT   = 1,
  LSM6DSO_LIGHT        = 2,
  LSM6DSO_MEDIUM       = 3,
  LSM6DSO_STRONG       = 4, /* not available for data rate > 1k670Hz */
  LSM6DSO_VERY_STRONG  = 5, /* not available for data rate > 1k670Hz */
  LSM6DSO_AGGRESSIVE   = 6, /* not available for data rate > 1k670Hz */
  LSM6DSO_XTREME       = 7, /* not available for data rate > 1k670Hz */
} lsm6dso_ftype_t;

typedef enum
{
  LSM6DSO_HP_PATH_DISABLE_ON_OUT    = 0x00,
  LSM6DSO_SLOPE_ODR_DIV_4           = 0x10,
  LSM6DSO_HP_ODR_DIV_10             = 0x11,
  LSM6DSO_HP_ODR_DIV_20             = 0x12,
  LSM6DSO_HP_ODR_DIV_45             = 0x13,
  LSM6DSO_HP_ODR_DIV_100            = 0x14,
  LSM6DSO_HP_ODR_DIV_200            = 0x15,
  LSM6DSO_HP_ODR_DIV_400            = 0x16,
  LSM6DSO_HP_ODR_DIV_800            = 0x17,
  LSM6DSO_HP_REF_MD_ODR_DIV_10      = 0x31,
  LSM6DSO_HP_REF_MD_ODR_DIV_20      = 0x32,
  LSM6DSO_HP_REF_MD_ODR_DIV_45      = 0x33,
  LSM6DSO_HP_REF_MD_ODR_DIV_100     = 0x34,
  LSM6DSO_HP_REF_MD_ODR_DIV_200     = 0x35,
  LSM6DSO_HP_REF_MD_ODR_DIV_400     = 0x36,
  LSM6DSO_HP_REF_MD_ODR_DIV_800     = 0x37,
  LSM6DSO_LP_ODR_DIV_10             = 0x01,
  LSM6DSO_LP_ODR_DIV_20             = 0x02,
  LSM6DSO_LP_ODR_DIV_45             = 0x03,
  LSM6DSO_LP_ODR_DIV_100            = 0x04,
  LSM6DSO_LP_ODR_DIV_200            = 0x05,
  LSM6DSO_LP_ODR_DIV_400            = 0x06,
  LSM6DSO_LP_ODR_DIV_800            = 0x07,
} lsm6dso_hp_slope_xl_en_t;

typedef enum
{
  LSM6DSO_USE_SLOPE = 0,
  LSM6DSO_USE_HPF   = 1,
} lsm6dso_slope_fds_t;

typedef enum
{
  LSM6DSO_HP_FILTER_NONE     = 0x00,
  LSM6DSO_HP_FILTER_16mHz    = 0x80,
  LSM6DSO_HP_FILTER_65mHz    = 0x81,
  LSM6DSO_HP_FILTER_260mHz   = 0x82,
  LSM6DSO_HP_FILTER_1Hz04    = 0x83,
} lsm6dso_hpm_g_t;

typedef enum
{
  LSM6DSO_AUX_PULL_UP_DISC       = 0,
  LSM6DSO_AUX_PULL_UP_CONNECT    = 1,
} lsm6dso_ois_pu_dis_t;

typedef enum
{
  LSM6DSO_AUX_ON                    = 1,
  LSM6DSO_AUX_ON_BY_AUX_INTERFACE   = 0,
} lsm6dso_ois_on_t;

typedef enum
{
  LSM6DSO_USE_SAME_XL_FS        = 0,
  LSM6DSO_USE_DIFFERENT_XL_FS   = 1,
} lsm6dso_xl_fs_mode_t;

typedef enum
{
  LSM6DSO_AUX_XL_DISABLE = 0,
  LSM6DSO_AUX_XL_POS     = 1,
  LSM6DSO_AUX_XL_NEG     = 2,
} lsm6dso_st_xl_ois_t;

typedef enum
{
  LSM6DSO_AUX_DEN_ACTIVE_LOW     = 0,
  LSM6DSO_AUX_DEN_ACTIVE_HIGH    = 1,
} lsm6dso_den_lh_ois_t;

typedef enum
{
  LSM6DSO_AUX_DEN_DISABLE         = 0,
  LSM6DSO_AUX_DEN_LEVEL_LATCH     = 3,
  LSM6DSO_AUX_DEN_LEVEL_TRIG      = 2,
} lsm6dso_lvl2_ois_t;

typedef enum
{
  LSM6DSO_AUX_DISABLE  = 0,
  LSM6DSO_MODE_3_GY    = 1,
  LSM6DSO_MODE_4_GY_XL = 3,
} lsm6dso_ois_en_spi2_t;

typedef enum
{
  LSM6DSO_250dps_AUX  = 0,
  LSM6DSO_125dps_AUX  = 1,
  LSM6DSO_500dps_AUX  = 2,
  LSM6DSO_1000dps_AUX = 4,
  LSM6DSO_2000dps_AUX = 6,
} lsm6dso_fs_g_ois_t;

typedef enum
{
  LSM6DSO_AUX_SPI_4_WIRE = 0,
  LSM6DSO_AUX_SPI_3_WIRE = 1,
} lsm6dso_sim_ois_t;
typedef enum
{
  LSM6DSO_351Hz39 = 0,
  LSM6DSO_236Hz63 = 1,
  LSM6DSO_172Hz70 = 2,
  LSM6DSO_937Hz91 = 3,
} lsm6dso_ftype_ois_t;

typedef enum
{
  LSM6DSO_AUX_HP_DISABLE = 0x00,
  LSM6DSO_AUX_HP_Hz016   = 0x10,
  LSM6DSO_AUX_HP_Hz065   = 0x11,
  LSM6DSO_AUX_HP_Hz260   = 0x12,
  LSM6DSO_AUX_HP_1Hz040  = 0x13,
} lsm6dso_hpm_ois_t;

typedef enum
{
  LSM6DSO_ENABLE_CLAMP  = 0,
  LSM6DSO_DISABLE_CLAMP = 1,
} lsm6dso_st_ois_clampdis_t;

typedef enum
{
  LSM6DSO_AUX_GY_DISABLE = 0,
  LSM6DSO_AUX_GY_POS     = 1,
  LSM6DSO_AUX_GY_NEG     = 3,
} lsm6dso_st_ois_t;

typedef enum
{
  LSM6DSO_289Hz = 0,
  LSM6DSO_258Hz = 1,
  LSM6DSO_120Hz = 2,
  LSM6DSO_65Hz2 = 3,
  LSM6DSO_33Hz2 = 4,
  LSM6DSO_16Hz6 = 5,
  LSM6DSO_8Hz30 = 6,
  LSM6DSO_4Hz15 = 7,
} lsm6dso_filter_xl_conf_ois_t;

typedef enum
{
  LSM6DSO_AUX_2g  = 0,
  LSM6DSO_AUX_16g = 1,
  LSM6DSO_AUX_4g  = 2,
  LSM6DSO_AUX_8g  = 3,
} lsm6dso_fs_xl_ois_t;

typedef enum
{
  LSM6DSO_PULL_UP_DISC       = 0,
  LSM6DSO_PULL_UP_CONNECT    = 1,
} lsm6dso_sdo_pu_en_t;

typedef enum
{
  LSM6DSO_SPI_4_WIRE = 0,
  LSM6DSO_SPI_3_WIRE = 1,
} lsm6dso_sim_t;

typedef enum
{
  LSM6DSO_I2C_ENABLE  = 0,
  LSM6DSO_I2C_DISABLE = 1,
} lsm6dso_i2c_disable_t;

typedef enum
{
  LSM6DSO_I3C_DISABLE         = 0x80,
  LSM6DSO_I3C_ENABLE_T_50us   = 0x00,
  LSM6DSO_I3C_ENABLE_T_2us    = 0x01,
  LSM6DSO_I3C_ENABLE_T_1ms    = 0x02,
  LSM6DSO_I3C_ENABLE_T_25ms   = 0x03,
} lsm6dso_i3c_disable_t;

typedef enum
{
  LSM6DSO_PULL_DOWN_DISC       = 0,
  LSM6DSO_PULL_DOWN_CONNECT    = 1,
} lsm6dso_int1_pd_en_t;

typedef enum
{
  LSM6DSO_PUSH_PULL   = 0,
  LSM6DSO_OPEN_DRAIN  = 1,
} lsm6dso_pp_od_t;

typedef enum
{
  LSM6DSO_ACTIVE_HIGH = 0,
  LSM6DSO_ACTIVE_LOW  = 1,
} lsm6dso_h_lactive_t;


typedef enum
{
  LSM6DSO_ALL_INT_PULSED            = 0,
  LSM6DSO_BASE_LATCHED_EMB_PULSED   = 1,
  LSM6DSO_BASE_PULSED_EMB_LATCHED   = 2,
  LSM6DSO_ALL_INT_LATCHED           = 3,
} lsm6dso_lir_t;

typedef enum
{
  LSM6DSO_LSb_FS_DIV_64       = 0,
  LSM6DSO_LSb_FS_DIV_256      = 1,
} lsm6dso_wake_ths_w_t;


typedef enum
{
  LSM6DSO_DRIVE_SLEEP_CHG_EVENT = 0,
  LSM6DSO_DRIVE_SLEEP_STATUS    = 1,
} lsm6dso_sleep_status_on_int_t;


typedef enum
{
  LSM6DSO_XL_AND_GY_NOT_AFFECTED      = 0,
  LSM6DSO_XL_12Hz5_GY_NOT_AFFECTED    = 1,
  LSM6DSO_XL_12Hz5_GY_SLEEP           = 2,
  LSM6DSO_XL_12Hz5_GY_PD              = 3,
} lsm6dso_inact_en_t;


typedef enum
{
  LSM6DSO_XYZ = 0,
  LSM6DSO_YXZ = 1,
  LSM6DSO_XZY = 2,
  LSM6DSO_ZYX = 3,
  LSM6DSO_YZX = 5,
  LSM6DSO_ZXY = 6,
} lsm6dso_tap_priority_t;

typedef enum
{
  LSM6DSO_ONLY_SINGLE = 0,
  LSM6DSO_BOTH_SINGLE_DOUBLE = 1,
} lsm6dso_single_double_tap_t;

typedef enum
{
  LSM6DSO_DEG_80  = 0,
  LSM6DSO_DEG_70  = 1,
  LSM6DSO_DEG_60  = 2,
  LSM6DSO_DEG_50  = 3,
} lsm6dso_sixd_ths_t;

typedef enum
{
  LSM6DSO_FF_TSH_156mg = 0,
  LSM6DSO_FF_TSH_219mg = 1,
  LSM6DSO_FF_TSH_250mg = 2,
  LSM6DSO_FF_TSH_312mg = 3,
  LSM6DSO_FF_TSH_344mg = 4,
  LSM6DSO_FF_TSH_406mg = 5,
  LSM6DSO_FF_TSH_469mg = 6,
  LSM6DSO_FF_TSH_500mg = 7,
} lsm6dso_ff_ths_t;


typedef enum
{
  LSM6DSO_CMP_DISABLE  = 0x00,
  LSM6DSO_CMP_ALWAYS   = 0x04,
  LSM6DSO_CMP_8_TO_1   = 0x05,
  LSM6DSO_CMP_16_TO_1  = 0x06,
  LSM6DSO_CMP_32_TO_1  = 0x07,
} lsm6dso_uncoptr_rate_t;

typedef enum
{
  LSM6DSO_XL_NOT_BATCHED       =  0,
  LSM6DSO_XL_BATCHED_AT_12Hz5   =  1,
  LSM6DSO_XL_BATCHED_AT_26Hz    =  2,
  LSM6DSO_XL_BATCHED_AT_52Hz    =  3,
  LSM6DSO_XL_BATCHED_AT_104Hz   =  4,
  LSM6DSO_XL_BATCHED_AT_208Hz   =  5,
  LSM6DSO_XL_BATCHED_AT_417Hz   =  6,
  LSM6DSO_XL_BATCHED_AT_833Hz   =  7,
  LSM6DSO_XL_BATCHED_AT_1667Hz  =  8,
  LSM6DSO_XL_BATCHED_AT_3333Hz  =  9,
  LSM6DSO_XL_BATCHED_AT_6667Hz  = 10,
  LSM6DSO_XL_BATCHED_AT_6Hz5    = 11,
} lsm6dso_bdr_xl_t;


typedef enum
{
  LSM6DSO_GY_NOT_BATCHED         = 0,
  LSM6DSO_GY_BATCHED_AT_12Hz5    = 1,
  LSM6DSO_GY_BATCHED_AT_26Hz     = 2,
  LSM6DSO_GY_BATCHED_AT_52Hz     = 3,
  LSM6DSO_GY_BATCHED_AT_104Hz    = 4,
  LSM6DSO_GY_BATCHED_AT_208Hz    = 5,
  LSM6DSO_GY_BATCHED_AT_417Hz    = 6,
  LSM6DSO_GY_BATCHED_AT_833Hz    = 7,
  LSM6DSO_GY_BATCHED_AT_1667Hz   = 8,
  LSM6DSO_GY_BATCHED_AT_3333Hz   = 9,
  LSM6DSO_GY_BATCHED_AT_6667Hz   = 10,
  LSM6DSO_GY_BATCHED_AT_6Hz5     = 11,
} lsm6dso_bdr_gy_t;


typedef enum
{
  LSM6DSO_BYPASS_MODE             = 0,
  LSM6DSO_FIFO_MODE               = 1,
  LSM6DSO_STREAM_TO_FIFO_MODE     = 3,
  LSM6DSO_BYPASS_TO_STREAM_MODE   = 4,
  LSM6DSO_STREAM_MODE             = 6,
  LSM6DSO_BYPASS_TO_FIFO_MODE     = 7,
} lsm6dso_fifo_mode_t;


typedef enum
{
  LSM6DSO_TEMP_NOT_BATCHED        = 0,
  LSM6DSO_TEMP_BATCHED_AT_1Hz6    = 1,
  LSM6DSO_TEMP_BATCHED_AT_12Hz5   = 2,
  LSM6DSO_TEMP_BATCHED_AT_52Hz    = 3,
} lsm6dso_odr_t_batch_t;

typedef enum
{
  LSM6DSO_NO_DECIMATION = 0,
  LSM6DSO_DEC_1         = 1,
  LSM6DSO_DEC_8         = 2,
  LSM6DSO_DEC_32        = 3,
} lsm6dso_odr_ts_batch_t;

typedef enum
{
  LSM6DSO_XL_BATCH_EVENT   = 0,
  LSM6DSO_GYRO_BATCH_EVENT = 1,
} lsm6dso_trig_counter_bdr_t;

typedef enum
{
  LSM6DSO_GYRO_NC_TAG    = 1,
  LSM6DSO_XL_NC_TAG,
  LSM6DSO_TEMPERATURE_TAG,
  LSM6DSO_TIMESTAMP_TAG,
  LSM6DSO_CFG_CHANGE_TAG,
  LSM6DSO_XL_NC_T_2_TAG,
  LSM6DSO_XL_NC_T_1_TAG,
  LSM6DSO_XL_2XC_TAG,
  LSM6DSO_XL_3XC_TAG,
  LSM6DSO_GYRO_NC_T_2_TAG,
  LSM6DSO_GYRO_NC_T_1_TAG,
  LSM6DSO_GYRO_2XC_TAG,
  LSM6DSO_GYRO_3XC_TAG,
  LSM6DSO_SENSORHUB_SLAVE0_TAG,
  LSM6DSO_SENSORHUB_SLAVE1_TAG,
  LSM6DSO_SENSORHUB_SLAVE2_TAG,
  LSM6DSO_SENSORHUB_SLAVE3_TAG,
  LSM6DSO_STEP_COUNTER_TAG,
  LSM6DSO_GAME_ROTATION_TAG,
  LSM6DSO_GEOMAG_ROTATION_TAG,
  LSM6DSO_ROTATION_TAG,
  LSM6DSO_SENSORHUB_NACK_TAG  = 0x19,
} lsm6dso_fifo_tag_t;

typedef enum
{
  LSM6DSO_DEN_DISABLE    = 0,
  LSM6DSO_LEVEL_FIFO     = 6,
  LSM6DSO_LEVEL_LETCHED  = 3,
  LSM6DSO_LEVEL_TRIGGER  = 2,
  LSM6DSO_EDGE_TRIGGER   = 4,
} lsm6dso_den_mode_t;

typedef enum
{
  LSM6DSO_DEN_ACT_LOW  = 0,
  LSM6DSO_DEN_ACT_HIGH = 1,
} lsm6dso_den_lh_t;

typedef enum
{
  LSM6DSO_STAMP_IN_GY_DATA     = 0,
  LSM6DSO_STAMP_IN_XL_DATA     = 1,
  LSM6DSO_STAMP_IN_GY_XL_DATA  = 2,
} lsm6dso_den_xl_g_t;

typedef enum
{
  LSM6DSO_PEDO_BASE_MODE            = 0x00,
  LSM6DSO_FALSE_STEP_REJ            = 0x10,
  LSM6DSO_FALSE_STEP_REJ_ADV_MODE   = 0x30,
} lsm6dso_pedo_md_t;

typedef enum
{
  LSM6DSO_EVERY_STEP     = 0,
  LSM6DSO_COUNT_OVERFLOW = 1,
} lsm6dso_carry_count_en_t;

typedef enum
{
  LSM6DSO_Z_EQ_Y     = 0,
  LSM6DSO_Z_EQ_MIN_Y = 1,
  LSM6DSO_Z_EQ_X     = 2,
  LSM6DSO_Z_EQ_MIN_X = 3,
  LSM6DSO_Z_EQ_MIN_Z = 4,
  LSM6DSO_Z_EQ_Z     = 5,
} lsm6dso_mag_z_axis_t;

typedef enum
{
  LSM6DSO_Y_EQ_Y     = 0,
  LSM6DSO_Y_EQ_MIN_Y = 1,
  LSM6DSO_Y_EQ_X     = 2,
  LSM6DSO_Y_EQ_MIN_X = 3,
  LSM6DSO_Y_EQ_MIN_Z = 4,
  LSM6DSO_Y_EQ_Z     = 5,
} lsm6dso_mag_y_axis_t;

typedef enum
{
  LSM6DSO_X_EQ_Y     = 0,
  LSM6DSO_X_EQ_MIN_Y = 1,
  LSM6DSO_X_EQ_X     = 2,
  LSM6DSO_X_EQ_MIN_X = 3,
  LSM6DSO_X_EQ_MIN_Z = 4,
  LSM6DSO_X_EQ_Z     = 5,
} lsm6dso_mag_x_axis_t;

typedef struct
{
  lsm6dso_fsm_enable_a_t          fsm_enable_a;
  lsm6dso_fsm_enable_b_t          fsm_enable_b;
} lsm6dso_emb_fsm_enable_t;

typedef enum
{
  LSM6DSO_LC_NORMAL     = 0,
  LSM6DSO_LC_CLEAR      = 1,
  LSM6DSO_LC_CLEAR_DONE = 2,
} lsm6dso_fsm_lc_clr_t;

typedef struct
{
  lsm6dso_fsm_outs1_t    fsm_outs1;
  lsm6dso_fsm_outs2_t    fsm_outs2;
  lsm6dso_fsm_outs3_t    fsm_outs3;
  lsm6dso_fsm_outs4_t    fsm_outs4;
  lsm6dso_fsm_outs5_t    fsm_outs5;
  lsm6dso_fsm_outs6_t    fsm_outs6;
  lsm6dso_fsm_outs7_t    fsm_outs7;
  lsm6dso_fsm_outs8_t    fsm_outs8;
  lsm6dso_fsm_outs9_t    fsm_outs9;
  lsm6dso_fsm_outs10_t   fsm_outs10;
  lsm6dso_fsm_outs11_t   fsm_outs11;
  lsm6dso_fsm_outs12_t   fsm_outs12;
  lsm6dso_fsm_outs13_t   fsm_outs13;
  lsm6dso_fsm_outs14_t   fsm_outs14;
  lsm6dso_fsm_outs15_t   fsm_outs15;
  lsm6dso_fsm_outs16_t   fsm_outs16;
} lsm6dso_fsm_out_t;

typedef enum
{
  LSM6DSO_ODR_FSM_12Hz5 = 0,
  LSM6DSO_ODR_FSM_26Hz  = 1,
  LSM6DSO_ODR_FSM_52Hz  = 2,
  LSM6DSO_ODR_FSM_104Hz = 3,
} lsm6dso_fsm_odr_t;

typedef enum
{
  LSM6DSO_SLV_0       = 0,
  LSM6DSO_SLV_0_1     = 1,
  LSM6DSO_SLV_0_1_2   = 2,
  LSM6DSO_SLV_0_1_2_3 = 3,
} lsm6dso_aux_sens_on_t;

typedef enum
{
  LSM6DSO_EXT_PULL_UP      = 0,
  LSM6DSO_INTERNAL_PULL_UP = 1,
} lsm6dso_shub_pu_en_t;

typedef enum
{
  LSM6DSO_EXT_ON_INT2_PIN = 1,
  LSM6DSO_XL_GY_DRDY      = 0,
} lsm6dso_start_config_t;

typedef enum
{
  LSM6DSO_EACH_SH_CYCLE    = 0,
  LSM6DSO_ONLY_FIRST_CYCLE = 1,
} lsm6dso_write_once_t;

typedef enum
{
  LSM6DSO_SH_ODR_104Hz = 0,
  LSM6DSO_SH_ODR_52Hz  = 1,
  LSM6DSO_SH_ODR_26Hz  = 2,
  LSM6DSO_SH_ODR_13Hz  = 3,
} lsm6dso_shub_odr_t;


typedef struct
{
  uint8_t   slv0_add;
  uint8_t   slv0_subadd;
  uint8_t   slv0_data;
} lsm6dso_sh_cfg_write_t;

typedef struct
{
  uint8_t   slv_add;
  uint8_t   slv_subadd;
  uint8_t   slv_len;
} lsm6dso_sh_cfg_read_t;

typedef struct
{
  uint8_t ui;
  uint8_t aux;
} lsm6dso_id_t;

typedef enum
{
  LSM6DSO_SEL_BY_HW   = 0x00, /* bus mode select by HW (SPI 3W disable) */
  LSM6DSO_SPI_4W      = 0x06, /* Only SPI: SDO / SDI separated pins */
  LSM6DSO_SPI_3W      = 0x07, /* Only SPI: SDO / SDI share the same pin */
  LSM6DSO_I2C         = 0x04, /* Only I2C */
  LSM6DSO_I3C_T_50us  = 0x02, /* I3C: available time equal to 50 us */
  LSM6DSO_I3C_T_2us   = 0x12, /* I3C: available time equal to 2 us */
  LSM6DSO_I3C_T_1ms   = 0x22, /* I3C: available time equal to 1 ms */
  LSM6DSO_I3C_T_25ms  = 0x32, /* I3C: available time equal to 25 ms */
} lsm6dso_ui_bus_md_t;

typedef enum
{
  LSM6DSO_SPI_4W_AUX  = 0x00,
  LSM6DSO_SPI_3W_AUX  = 0x01,
} lsm6dso_aux_bus_md_t;

typedef struct
{
  lsm6dso_ui_bus_md_t ui_bus_md;
  lsm6dso_aux_bus_md_t aux_bus_md;
} lsm6dso_bus_mode_t;

typedef enum
{
  LSM6DSO_DRV_RDY   = 0x00, /* Initialize the device for driver usage */
  LSM6DSO_BOOT      = 0x01, /* Restore calib. param. ( it takes 10ms ) */
  LSM6DSO_RESET     = 0x02, /* Reset configuration registers */
  LSM6DSO_FIFO_COMP = 0x04, /* FIFO compression initialization request. */
  LSM6DSO_FSM       = 0x08, /* Finite State Machine initialization request */
  LSM6DSO_PEDO      = 0x20, /* Pedometer algo initialization request. */
  LSM6DSO_TILT      = 0x40, /* Tilt algo initialization request */
  LSM6DSO_SMOTION   = 0x80, /* Significant Motion initialization request */
} lsm6dso_init_t;

typedef struct
{
uint8_t sw_reset           :
  1; /* Restoring configuration registers */
  uint8_t boot               : 1; /* Restoring calibration parameters */
  uint8_t drdy_xl            : 1; /* Accelerometer data ready */
  uint8_t drdy_g             : 1; /* Gyroscope data ready */
  uint8_t drdy_temp          : 1; /* Temperature data ready */
  uint8_t ois_drdy_xl        : 1; /* Accelerometer data ready on OIS */
  uint8_t ois_drdy_g         : 1; /* Gyroscope data ready on OIS */
uint8_t ois_gyro_settling  :
  1; /* Gyroscope is in the settling phase */
} lsm6dso_status_t;

typedef struct
{
  uint8_t sdo_sa0_pull_up     : 1; /* 1 = pull-up on SDO/SA0 pin */
uint8_t aux_sdo_ocs_pull_up :
  1; /* 1 = pull-up on OCS_Aux/SDO_Aux pins */
  uint8_t int1_int2_push_pull : 1; /* 1 = push-pull / 0 = open-drain*/
uint8_t int1_pull_down      :
  1; /* 1 = pull-down always disabled (0=auto) */
} lsm6dso_pin_conf_t;

typedef struct
{
  uint8_t active_low   : 1; /* 1 = active low / 0 = active high */
uint8_t base_latched :
  1; /* base functions are: FF, WU, 6D, Tap, Act/Inac */
uint8_t emb_latched  :
  1; /* emb functions are: Pedo, Tilt, SMot, Timestamp */
} lsm6dso_int_mode_t;

typedef struct
{
  uint8_t drdy_xl       : 1; /* Accelerometer data ready */
  uint8_t drdy_g        : 1; /* Gyroscope data ready */
uint8_t drdy_temp     :
  1; /* Temperature data ready (1 = int2 pin disable) */
  uint8_t boot          : 1; /* Restoring calibration parameters */
  uint8_t fifo_th       : 1; /* FIFO threshold reached */
  uint8_t fifo_ovr      : 1; /* FIFO overrun */
  uint8_t fifo_full     : 1; /* FIFO full */
  uint8_t fifo_bdr      : 1; /* FIFO Batch counter threshold reached */
uint8_t den_flag      :
  1; /* external trigger level recognition (DEN) */
  uint8_t sh_endop      : 1; /* sensor hub end operation */
uint8_t timestamp     :
  1; /* timestamp overflow (1 = int2 pin disable) */
  uint8_t six_d         : 1; /* orientation change (6D/4D detection) */
  uint8_t double_tap    : 1; /* double-tap event */
  uint8_t free_fall     : 1; /* free fall event */
  uint8_t wake_up       : 1; /* wake up event */
  uint8_t single_tap    : 1; /* single-tap event */
uint8_t sleep_change  :
  1; /* Act/Inact (or Vice-versa) status changed */
  uint8_t step_detector : 1; /* Step detected */
  uint8_t tilt          : 1; /* Relative tilt event detected */
  uint8_t sig_mot       : 1; /* "significant motion" event detected */
uint8_t fsm_lc        :
  1; /* fsm long counter timeout interrupt event */
  uint8_t fsm1          : 1; /* fsm 1 interrupt event */
  uint8_t fsm2          : 1; /* fsm 2 interrupt event */
  uint8_t fsm3          : 1; /* fsm 3 interrupt event */
  uint8_t fsm4          : 1; /* fsm 4 interrupt event */
  uint8_t fsm5          : 1; /* fsm 5 interrupt event */
  uint8_t fsm6          : 1; /* fsm 6 interrupt event */
  uint8_t fsm7          : 1; /* fsm 7 interrupt event */
  uint8_t fsm8          : 1; /* fsm 8 interrupt event */
  uint8_t fsm9          : 1; /* fsm 9 interrupt event */
  uint8_t fsm10         : 1; /* fsm 10 interrupt event */
  uint8_t fsm11         : 1; /* fsm 11 interrupt event */
  uint8_t fsm12         : 1; /* fsm 12 interrupt event */
  uint8_t fsm13         : 1; /* fsm 13 interrupt event */
  uint8_t fsm14         : 1; /* fsm 14 interrupt event */
  uint8_t fsm15         : 1; /* fsm 15 interrupt event */
  uint8_t fsm16         : 1; /* fsm 16 interrupt event */
  uint8_t mlc1          : 1; /* mlc 1 interrupt event */
  uint8_t mlc2          : 1; /* mlc 2 interrupt event */
  uint8_t mlc3          : 1; /* mlc 3 interrupt event */
  uint8_t mlc4          : 1; /* mlc 4 interrupt event */
  uint8_t mlc5          : 1; /* mlc 5 interrupt event */
  uint8_t mlc6          : 1; /* mlc 6 interrupt event */
  uint8_t mlc7          : 1; /* mlc 7 interrupt event */
  uint8_t mlc8          : 1; /* mlc 8 interrupt event */
} lsm6dso_pin_int1_route_t;


typedef struct
{
  uint8_t drdy_ois      : 1; /* OIS chain data ready */
  uint8_t drdy_xl       : 1; /* Accelerometer data ready */
  uint8_t drdy_g        : 1; /* Gyroscope data ready */
  uint8_t drdy_temp     : 1; /* Temperature data ready */
  uint8_t fifo_th       : 1; /* FIFO threshold reached */
  uint8_t fifo_ovr      : 1; /* FIFO overrun */
  uint8_t fifo_full     : 1; /* FIFO full */
  uint8_t fifo_bdr      : 1; /* FIFO Batch counter threshold reached */
  uint8_t timestamp     : 1; /* timestamp overflow */
  uint8_t six_d         : 1; /* orientation change (6D/4D detection) */
  uint8_t double_tap    : 1; /* double-tap event */
  uint8_t free_fall     : 1; /* free fall event */
  uint8_t wake_up       : 1; /* wake up event */
  uint8_t single_tap    : 1; /* single-tap event */
uint8_t sleep_change  :
  1; /* Act/Inact (or Vice-versa) status changed */
  uint8_t step_detector : 1; /* Step detected */
  uint8_t tilt          : 1; /* Relative tilt event detected */
  uint8_t sig_mot       : 1; /* "significant motion" event detected */
uint8_t fsm_lc        :
  1; /* fsm long counter timeout interrupt event */
  uint8_t fsm1          : 1; /* fsm 1 interrupt event */
  uint8_t fsm2          : 1; /* fsm 2 interrupt event */
  uint8_t fsm3          : 1; /* fsm 3 interrupt event */
  uint8_t fsm4          : 1; /* fsm 4 interrupt event */
  uint8_t fsm5          : 1; /* fsm 5 interrupt event */
  uint8_t fsm6          : 1; /* fsm 6 interrupt event */
  uint8_t fsm7          : 1; /* fsm 7 interrupt event */
  uint8_t fsm8          : 1; /* fsm 8 interrupt event */
  uint8_t fsm9          : 1; /* fsm 9 interrupt event */
  uint8_t fsm10         : 1; /* fsm 10 interrupt event */
  uint8_t fsm11         : 1; /* fsm 11 interrupt event */
  uint8_t fsm12         : 1; /* fsm 12 interrupt event */
  uint8_t fsm13         : 1; /* fsm 13 interrupt event */
  uint8_t fsm14         : 1; /* fsm 14 interrupt event */
  uint8_t fsm15         : 1; /* fsm 15 interrupt event */
  uint8_t fsm16         : 1; /* fsm 16 interrupt event */
  uint8_t mlc1          : 1; /* mlc 1 interrupt event */
  uint8_t mlc2          : 1; /* mlc 2 interrupt event */
  uint8_t mlc3          : 1; /* mlc 3 interrupt event */
  uint8_t mlc4          : 1; /* mlc 4 interrupt event */
  uint8_t mlc5          : 1; /* mlc 5 interrupt event */
  uint8_t mlc6          : 1; /* mlc 6 interrupt event */
  uint8_t mlc7          : 1; /* mlc 7 interrupt event */
  uint8_t mlc8          : 1; /* mlc 8 interrupt event */
} lsm6dso_pin_int2_route_t;


typedef struct
{
  uint8_t drdy_xl          :  1; /* Accelerometer data ready */
  uint8_t drdy_g           :  1; /* Gyroscope data ready */
  uint8_t drdy_temp        :  1; /* Temperature data ready */
uint8_t den_flag         :
  1; /* external trigger level recognition (DEN) */
uint8_t timestamp        :
  1; /* timestamp overflow (1 = int2 pin disable) */
  uint8_t free_fall        :  1; /* free fall event */
  uint8_t wake_up          :  1; /* wake up event */
  uint8_t wake_up_z        :  1; /* wake up on Z axis event */
  uint8_t wake_up_y        :  1; /* wake up on Y axis event */
  uint8_t wake_up_x        :  1; /* wake up on X axis event */
  uint8_t single_tap       :  1; /* single-tap event */
  uint8_t double_tap       :  1; /* double-tap event */
  uint8_t tap_z            :  1; /* single-tap on Z axis event */
  uint8_t tap_y            :  1; /* single-tap on Y axis event */
  uint8_t tap_x            :  1; /* single-tap on X axis event */
  uint8_t tap_sign         :  1; /* sign of tap event (0-pos / 1-neg) */
uint8_t six_d            :
  1; /* orientation change (6D/4D detection) */
uint8_t six_d_xl         :
  1; /* X-axis low 6D/4D event (under threshold) */
uint8_t six_d_xh         :
  1; /* X-axis high 6D/4D event (over threshold) */
uint8_t six_d_yl         :
  1; /* Y-axis low 6D/4D event (under threshold) */
uint8_t six_d_yh         :
  1; /* Y-axis high 6D/4D event (over threshold) */
uint8_t six_d_zl         :
  1; /* Z-axis low 6D/4D event (under threshold) */
uint8_t six_d_zh         :
  1; /* Z-axis high 6D/4D event (over threshold) */
uint8_t sleep_change     :
  1; /* Act/Inact (or Vice-versa) status changed */
uint8_t sleep_state      :
  1; /* Act/Inact status flag (0-Act / 1-Inact) */
  uint8_t step_detector    :  1; /* Step detected */
  uint8_t tilt             :  1; /* Relative tilt event detected */
uint8_t sig_mot          :
  1; /* "significant motion" event detected */
uint8_t fsm_lc           :
  1; /* fsm long counter timeout interrupt event */
  uint8_t fsm1             :  1; /* fsm 1 interrupt event */
  uint8_t fsm2             :  1; /* fsm 2 interrupt event */
  uint8_t fsm3             :  1; /* fsm 3 interrupt event */
  uint8_t fsm4             :  1; /* fsm 4 interrupt event */
  uint8_t fsm5             :  1; /* fsm 5 interrupt event */
  uint8_t fsm6             :  1; /* fsm 6 interrupt event */
  uint8_t fsm7             :  1; /* fsm 7 interrupt event */
  uint8_t fsm8             :  1; /* fsm 8 interrupt event */
  uint8_t fsm9             :  1; /* fsm 9 interrupt event */
  uint8_t fsm10            :  1; /* fsm 10 interrupt event */
  uint8_t fsm11            :  1; /* fsm 11 interrupt event */
  uint8_t fsm12            :  1; /* fsm 12 interrupt event */
  uint8_t fsm13            :  1; /* fsm 13 interrupt event */
  uint8_t fsm14            :  1; /* fsm 14 interrupt event */
  uint8_t fsm15            :  1; /* fsm 15 interrupt event */
  uint8_t fsm16            :  1; /* fsm 16 interrupt event */
  uint8_t mlc1             :  1; /* mlc 1 interrupt event */
  uint8_t mlc2             :  1; /* mlc 2 interrupt event */
  uint8_t mlc3             :  1; /* mlc 3 interrupt event */
  uint8_t mlc4             :  1; /* mlc 4 interrupt event */
  uint8_t mlc5             :  1; /* mlc 5 interrupt event */
  uint8_t mlc6             :  1; /* mlc 6 interrupt event */
  uint8_t mlc7             :  1; /* mlc 7 interrupt event */
  uint8_t mlc8             :  1; /* mlc 8 interrupt event */
  uint8_t sh_endop         :  1; /* sensor hub end operation */
uint8_t sh_slave0_nack   :
  1; /* Not acknowledge on sensor hub slave 0 */
uint8_t sh_slave1_nack   :
  1; /* Not acknowledge on sensor hub slave 1 */
uint8_t sh_slave2_nack   :
  1; /* Not acknowledge on sensor hub slave 2 */
uint8_t sh_slave3_nack   :
  1; /* Not acknowledge on sensor hub slave 3 */
uint8_t sh_wr_once       :
  1; /* "WRITE_ONCE" end on sensor hub slave 0 */
uint16_t fifo_diff       :
  10; /* Number of unread sensor data in FIFO*/
  uint8_t fifo_ovr_latched :  1; /* Latched FIFO overrun status */
uint8_t fifo_bdr         :
  1; /* FIFO Batch counter threshold reached */
  uint8_t fifo_full        :  1; /* FIFO full */
  uint8_t fifo_ovr         :  1; /* FIFO overrun */
  uint8_t fifo_th          :  1; /* FIFO threshold reached */
} lsm6dso_all_sources_t;


typedef struct
{
  uint8_t odr_fine_tune;
} dev_cal_t;




typedef enum
{
  LSM6DSO_OIS_ONLY_AUX    = 0x00, /* Auxiliary SPI full control */
  LSM6DSO_OIS_MIXED       = 0x01, /* Enabling by UI / read-config by AUX */
} lsm6dso_ctrl_md_t;

typedef enum
{
  LSM6DSO_XL_OIS_OFF       = 0x00, /* in power down */
  LSM6DSO_XL_OIS_6667Hz_HP = 0x01, /* @6kHz OIS imu active/NO ULP on UI */
} lsm6dso_odr_xl_ois_noaux_t;

typedef enum
{
  LSM6DSO_XL_OIS_2g   = 0,
  LSM6DSO_XL_OIS_4g   = 2,
  LSM6DSO_XL_OIS_8g   = 3,
  LSM6DSO_XL_OIS_16g  = 1, /* UI full scale is also forced to be 16g */
} lsm6dso_fs_xl_ois_noaux_t;

typedef enum
{
  LSM6DSO_GY_OIS_OFF       = 0x00, /* in power down */
  LSM6DSO_GY_OIS_6667Hz_HP = 0x01, /* @6kHz No Ultra Low Power*/
} lsm6dso_odr_g_ois_noaux_t;

typedef enum
{
  LSM6DSO_GY_OIS_250dps   = 0,
  LSM6DSO_GY_OIS_125dps   = 1,
  LSM6DSO_GY_OIS_500dps   = 2,
  LSM6DSO_GY_OIS_1000dps  = 4,
  LSM6DSO_GY_OIS_2000dps  = 6,
} lsm6dso_fs_g_ois_noaux_t;

typedef enum
{
  LSM6DSO_FSM_DISABLE = 0x00,
  LSM6DSO_FSM_XL      = 0x01,
  LSM6DSO_FSM_GY      = 0x02,
  LSM6DSO_FSM_XL_GY   = 0x03,
} lsm6dso_sens_fsm_t;

typedef enum
{
  LSM6DSO_FSM_12Hz5 = 0x00,
  LSM6DSO_FSM_26Hz  = 0x01,
  LSM6DSO_FSM_52Hz  = 0x02,
  LSM6DSO_FSM_104Hz = 0x03,
} lsm6dso_odr_fsm_t;

typedef struct
{
  struct
  {
    struct
    {
      float_t mg[3];
      int16_t raw[3];
    } xl;
    struct
    {
      float_t mdps[3];
      int16_t raw[3];
    } gy;
    struct
    {
      float_t deg_c;
      int16_t raw;
    } heat;
  } ui;
  struct
  {
    struct
    {
      float_t mg[3];
      int16_t raw[3];
    } xl;
    struct
    {
      float_t mdps[3];
      int16_t raw[3];
    } gy;
  } ois;
} lsm6dso_data_t;


typedef struct
{
  uint8_t sig_mot      : 1; /* significant motion */
  uint8_t tilt         : 1; /* tilt detection  */
  uint8_t step         : 1; /* step counter/detector */
  uint8_t step_adv     : 1; /* step counter advanced mode */
  uint8_t fsm          : 1; /* finite state machine */
  uint8_t fifo_compr   : 1; /* FIFO compression */
} lsm6dso_emb_sens_t;

#endif /*LSM6DSO_DRIVER_H */

void LSM6DSOWTR_Init(void);
uint16_t LSM6DSOWTR_Read_Data( uint8_t reg);

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
