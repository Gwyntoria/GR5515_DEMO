/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_accppg_sync.h
 *
 * @brief   gh3x2x algo call demo code version
 *
 * @version ref gh3x2x_demo_algo_call_version.h
 *
 */

#ifndef _GH3X2X_DEMO_ACCPPG_SYNC_H_
#define _GH3X2X_DEMO_ACCPPG_SYNC_H_

#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_config.h"

#if (__GS_TIMESTAMP_READ_EN__)

#define __SYNC_FUNC_MAX__ (GH3X2X_FUNC_OFFSET_MAX)
#define SYNC_ABS(x) (int)((x)>0?(x):(-(x)))

typedef struct
{
    GU32 unTimeStamp;
    GS16 usAccX;
    GS16 usAccY;
    GS16 usAccZ;
}STSyncAccStuct;

typedef struct
{
    GU32 unTimeStamp;
    GU32 unFrameCnt;
    GU32 unRawdata[__SYNC_RAWDATA_MAX_NUM__];
    GU32 unAgcinfo[__SYNC_AGCINFO_MAX_NUM__];
    GU32 unFlag[__SYNC_FLAG_MAX_NUM__];
}STSyncPpgStuct;

#endif

#endif
