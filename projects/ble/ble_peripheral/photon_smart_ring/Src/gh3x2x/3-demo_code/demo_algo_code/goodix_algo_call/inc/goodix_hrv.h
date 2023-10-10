#ifndef __GOODIX_HRV_H
#define __GOODIX_HRV_H


#include "goodix_type.h"
#include "goodix_algo.h"
#ifdef __cplusplus
extern "C" {
#endif
#define HRV_CHNL_MAX_NUM 6
typedef struct
{
	GS32 ppg_green[HRV_CHNL_MAX_NUM];
	GS32 green_cur_adj_flag[HRV_CHNL_MAX_NUM];
	GS32 green_gain_adj_flag[HRV_CHNL_MAX_NUM];
	GS32 acc_x;
	GS32 acc_y;
	GS32 acc_z;
	GS32 frame_id;
	GS32 hr;
}ST_HRV_INPUT_INFO;
DRVDLL_API GU32 HRVAlgoInit (GS32 needIpl, GS32 fs, GS32 accThr[]);
DRVDLL_API GU32 HRVAlgoDeInit (void);
DRVDLL_API GU8* HRVAlgoVersion (void);
DRVDLL_API GS32 HRVCalculateAlgo (ST_HRV_INPUT_INFO *rawdata, GS32 lResult[]);

#ifdef __cplusplus
}
#endif

#endif
