#ifndef __GOODIX_ECG_H
#define __GOODIX_ECG_H

#include "stdint.h"
#include "goodix_type.h"

// ECG 采样率
#ifndef ECG_FS
#define ECG_FS 500
#endif
#define ECG_CHANNEL_NUM 2


/**
* @brief ECG 算法函数返回状态值
*/
typedef enum
{
	GX_ALGO_SUCCESS = 0x00000000,			/**< 成功     */

	GX_ALGO_NO_MEMORY = 0x00000001,			/**< 内存空间不够     */

}goodix_ecg_ret;

// ADC 数据 与 电压值 mv 之间的转换，后续可能修改，因此开放到外部
#define VREF		(1.8)
#define GAIN		(20)		
#define VREF_ZERO	(1 << 23)

#if 0
#define GOODIX_ECG_ADC_2_MV(adc)	(float)(1000.0f * VREF * (((adc * 1.0f)) / VREF_ZERO) / GAIN)  //6导联EVK保存数据已经减过偏置
#define GOODIX_ECG_MV_2_ADC(mv)		(int)(int(((mv)*VREF_ZERO*GAIN)/(1000.0f*VREF))
#else
#define GOODIX_ECG_ADC_2_MV(adc)	(float)(1000.0f * VREF * (((adc * 1.0f) - VREF_ZERO) / VREF_ZERO) / GAIN)
#define GOODIX_ECG_MV_2_ADC(mv)		(int)(VREF_ZERO + int(((mv)*VREF_ZERO*GAIN)/(1000.0f*VREF))
#endif



typedef struct 
{
	uint32_t fs;									// 采样率
	uint32_t valid_channel_num;						// 有效通道数
	uint32_t qrs_flag;                              // QRS检测flag，0：不做QRS及房颤

}goodix_ecg_config;


typedef struct
{
	//uint32_t timestamp;         //时间戳
	uint32_t frameid;			//帧序号
	int32_t ecg_rawdata[ECG_CHANNEL_NUM];        //原始数据
	int32_t acc_x;              //加速度计x轴
	int32_t acc_y;              //加速度计y轴
	int32_t acc_z;              //加速度计z轴
	int32_t ecg_qc_flag;        //快速恢复标记
}goodix_ecg_input_rawdata;

typedef struct
{
	float32_t ecg_out[ECG_CHANNEL_NUM];               //心电值
	goodix_stage ecg_stage[ECG_CHANNEL_NUM];            //P-QRS-J-T 点状态
	float32_t ecg_bpm;
	
	float32_t ecg_snr[ECG_CHANNEL_NUM];               //信噪比
	float32_t ecg_powerline_out[ECG_CHANNEL_NUM];	// 滤除工频干扰 输出
	float32_t ecg_blw_out[ECG_CHANNEL_NUM];			// 滤除基线漂移 输出
	float32_t ecg_bpm_ch[ECG_CHANNEL_NUM];               //心率值
}goodix_ecg_result;


#ifdef __cplusplus
extern "C" {
#endif


/**
* @brief ECG 处理算法版本号
* @param[in]  version     外部储存版本号空间，分配20字节
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_version(uint8_t version[100]);


/**
* @brief ECG 处理算法初始化
* @param[in]  fs         ECG 和 ACC 的采样率【注：两者需要同频】
* @param[in]  memaddr    用于算法使用的外部内存首地址
* @param[in]  memsize	 用于算法使用的外部内存大小
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_init(goodix_ecg_config *cfg);


/**
* @brief ECG 算法更新
* @param[in] raw    ECG 输入的相关信息
* @param[out] res    ECG 返回的结果
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_update(goodix_ecg_input_rawdata* raw ,goodix_ecg_result *res);

/**
*@brief ECG 病例分类
* @param[in] 无
* @param[out] res    ECG 分类结果
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_classification_update(goodix_classification_flag *res);

/**
* @brief ECG 算法销毁，释放内部的内存空间，调用其他模块算法时使用
* @param[out] 无 
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
