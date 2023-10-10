#include <stdio.h>
//#include "gh3x2x_demo_inner.h"
#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_accppg_sync.h"
#include "gh3x2x_demo_common.h"

#if (__GS_TIMESTAMP_READ_EN__)

#if __USE_GOODIX_ADT_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_ADT[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_HR_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_HR[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_HRV_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_HRV[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_HSM_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_HSM[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_BP_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_FPBP[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_BP_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_PWA[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_SPO2_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_SPO2[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_ECG_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_ECG[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_SOFT_ADT[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_BT_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_BT[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_RESP_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_RESP[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_AF_ALGORITHM__
STSyncPpgStuct g_stPpgSyncStuct_AF[__SYNC_PPG_FRAME_NUM__] = {0};
#endif
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
//STSyncPpgStuct g_stPpgSyncStuct_SOFT_ADT_IR[__SYNC_PPG_FRAME_NUM__] = {0};
#endif

STSyncPpgStuct *g_pstPpgSyncStuct[__SYNC_FUNC_MAX__] = 
{
//adt
#if __USE_GOODIX_ADT_ALGORITHM__
    g_stPpgSyncStuct_ADT,
#else
    0,
#endif
//hr
#if __USE_GOODIX_HR_ALGORITHM__
    g_stPpgSyncStuct_HR,
#else
    0,
#endif
//hrv
#if __USE_GOODIX_HRV_ALGORITHM__
    g_stPpgSyncStuct_HRV,
#else
    0,
#endif
//hsm
#if __USE_GOODIX_HSM_ALGORITHM__
    g_stPpgSyncStuct_HSM,
#else
    0,
#endif
//fpbp
//pwa
#if __USE_GOODIX_BP_ALGORITHM__
    g_stPpgSyncStuct_FPBP,
    g_stPpgSyncStuct_PWA,
#else
    0,
    0,
#endif
//spo2
#if __USE_GOODIX_SPO2_ALGORITHM__
    g_stPpgSyncStuct_SPO2,
#else
    0,
#endif
//ecg
#if __USE_GOODIX_ECG_ALGORITHM__
    g_stPpgSyncStuct_ECG,
#else
    0,
#endif
//pwtt
    0,
//soft adt green
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    g_stPpgSyncStuct_SOFT_ADT,
#else
    0,
#endif
//bt
#if __USE_GOODIX_BT_ALGORITHM__
    g_stPpgSyncStuct_BT,
#else
    0,
#endif
//resp
#if __USE_GOODIX_RESP_ALGORITHM__
    g_stPpgSyncStuct_RESP,
#else
    0,
#endif
//af
#if __USE_GOODIX_AF_ALGORITHM__
    g_stPpgSyncStuct_AF,
#else
    0,
#endif
//test
    0,
    0,
//soft adt ir
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    g_stPpgSyncStuct_SOFT_ADT,
#else
    0,
#endif
};

static GU16 g_usAccSyncCnt = 0;
static STSyncAccStuct g_stAccSyncStuct[__SYNC_ACC_FRAME_NUM__];
static GU16 g_usPpgSyncCnt[__SYNC_FUNC_MAX__] = {0};
static GU8 g_uchAlgoHandleFlag[__SYNC_FUNC_MAX__] = {0};
static GU32 g_unPpgIntFlag = 0;
static GU8 g_uchSyncGetFrameDataFlag = 0;

extern void gh3x2x_algorithm_get_io_data_hook_func(const STGh3x2xFrameInfo * const pstFrameInfo);

#endif

GU8 GH3X2X_TimestampSyncGetFrameDataFlag(void)
{
#if (__GS_TIMESTAMP_READ_EN__)
    return g_uchSyncGetFrameDataFlag;
#else
    return 1;
#endif
}

void GH3X2X_TimestampSyncSetPpgIntFlag(GU8 uchPpgIntFlag)
{
#if (__GS_TIMESTAMP_READ_EN__)
    if (uchPpgIntFlag)
    {
        g_unPpgIntFlag = ~(0);
    }
#endif
}

void GH3X2X_TimestampSyncAccInit(void)
{
#if (__GS_TIMESTAMP_READ_EN__)
    g_uchSyncGetFrameDataFlag = 0;
    g_usAccSyncCnt = 0;
    memset(g_stAccSyncStuct, 0, __SYNC_ACC_BUFFER_SECOND__ * __SYNC_ACC_SAMPLERATE__ * sizeof(STSyncAccStuct));
#endif
}

void GH3X2X_TimestampSyncPpgInit(GU32 unFunctionID)
{
#if (__GS_TIMESTAMP_READ_EN__)
    for(GU8 uchFunCnt = 0; uchFunCnt < __SYNC_FUNC_MAX__; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            if (g_pstPpgSyncStuct[uchFunCnt] != GH3X2X_PTR_NULL)
            {
                memset(g_pstPpgSyncStuct[uchFunCnt], 0, __SYNC_PPG_FRAME_NUM__ * sizeof(STSyncPpgStuct));
                g_usPpgSyncCnt[uchFunCnt] = 0;
                g_uchAlgoHandleFlag[uchFunCnt] = 0;
                g_unPpgIntFlag = g_unPpgIntFlag & ~(0x1 << uchFunCnt);
            }
        }
    }
#endif
}

void GH3X2X_TimestampSyncSend2AlgoCall(GU8 uchFuncOffsetID)
{
#if (__GS_TIMESTAMP_READ_EN__)
    const STGh3x2xFrameInfo * const pstFrameInfo = g_pstGh3x2xFrameInfo[uchFuncOffsetID];
    GU8 uchAcccntRecord = 0;
    GS32 nAccPpgTsDiff = 0;
    GU8 ppg_cnt_record = 0;

    g_uchSyncGetFrameDataFlag = 1;

    GH3X2X_ALGO_LOG_PARAM("[%s]:func=%d,ppgcnt=%d,acccnt=%d,time=%d,%d.\n",\
                                    __FUNCTION__,\
                                    uchFuncOffsetID,\
                                    g_usPpgSyncCnt[uchFuncOffsetID],\
                                    g_usAccSyncCnt,\
                                    g_stAccSyncStuct[0].unTimeStamp,\
                                    g_pstPpgSyncStuct[uchFuncOffsetID][0].unTimeStamp);

    for (GU8 ppg_cnt = 0; ppg_cnt < g_usPpgSyncCnt[uchFuncOffsetID]; ppg_cnt++)
    {
        GH3X2X_ALGO_LOG_PARAM("[%s]:ppgcnt=%d\n", __FUNCTION__, ppg_cnt);
        for (GU8 acc_cnt = uchAcccntRecord; acc_cnt < g_usAccSyncCnt; acc_cnt++)
        {
            GH3X2X_ALGO_LOG_PARAM("[%s]:acccnt=%d\n", __FUNCTION__, acc_cnt);
            if (nAccPpgTsDiff > SYNC_ABS((int)g_stAccSyncStuct[acc_cnt].unTimeStamp - (int)g_pstPpgSyncStuct[uchFuncOffsetID][ppg_cnt].unTimeStamp)\
                || acc_cnt == uchAcccntRecord)
            {
                nAccPpgTsDiff = SYNC_ABS((int)g_stAccSyncStuct[acc_cnt].unTimeStamp - (int)g_pstPpgSyncStuct[uchFuncOffsetID][ppg_cnt].unTimeStamp);
            }

            if (nAccPpgTsDiff < SYNC_ABS((int)g_stAccSyncStuct[acc_cnt + 1].unTimeStamp - (int)g_pstPpgSyncStuct[uchFuncOffsetID][ppg_cnt].unTimeStamp)
                || acc_cnt == g_usAccSyncCnt - 1)
            {
                uchAcccntRecord = acc_cnt;
                pstFrameInfo->pusFrameGsensordata[0] = g_stAccSyncStuct[acc_cnt].usAccX;
                pstFrameInfo->pusFrameGsensordata[1] = g_stAccSyncStuct[acc_cnt].usAccY;
                pstFrameInfo->pusFrameGsensordata[2] = g_stAccSyncStuct[acc_cnt].usAccZ;
                pstFrameInfo->punFrameCnt[0] = g_pstPpgSyncStuct[uchFuncOffsetID][ppg_cnt].unFrameCnt;
                memcpy(pstFrameInfo->punFrameRawdata, g_pstPpgSyncStuct[uchFuncOffsetID][ppg_cnt].unRawdata, pstFrameInfo->pstFunctionInfo->uchChnlNum * sizeof(GU32));
                memcpy(pstFrameInfo->punFrameAgcInfo, g_pstPpgSyncStuct[uchFuncOffsetID][ppg_cnt].unAgcinfo, pstFrameInfo->pstFunctionInfo->uchChnlNum * sizeof(GU32));
                memcpy(pstFrameInfo->punFrameFlag, g_pstPpgSyncStuct[uchFuncOffsetID][ppg_cnt].unFlag, 8 * sizeof(GU32));

                gh3x2x_algorithm_get_io_data_hook_func(pstFrameInfo);

                ppg_cnt_record++;
                break;
            }
        }
    }
    if (ppg_cnt_record == g_usPpgSyncCnt[uchFuncOffsetID])
    {
        GH3X2X_ALGO_LOG_PARAM("[%s]:success!!!\n", __FUNCTION__);
    }
    else
    {
        GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:failed!!!\n", __FUNCTION__);
    }

    g_uchSyncGetFrameDataFlag = 0;
#endif
}

void GH3X2X_TimestampSyncFillAccSyncBuffer(GU32 unTimeStamp,
                                           GS16 usAccX,
                                           GS16 usAccY,
                                           GS16 usAccZ)
{
#if (__GS_TIMESTAMP_READ_EN__)
    if (g_stAccSyncStuct[g_usAccSyncCnt - 1].unTimeStamp > unTimeStamp
        && g_usAccSyncCnt != 0)
    {
        GH3X2X_ALGO_LOG_PARAM("[%s]:input time stamp error!!!\n", __FUNCTION__);
        return;
    }

    if (g_usAccSyncCnt >= __SYNC_ACC_FRAME_NUM__)
    {
        g_usAccSyncCnt = __SYNC_ACC_FRAME_NUM__ - 1;
        memcpy(&g_stAccSyncStuct[0], &g_stAccSyncStuct[1], g_usAccSyncCnt * sizeof(STSyncAccStuct));
    }

    g_stAccSyncStuct[g_usAccSyncCnt].unTimeStamp = unTimeStamp;
    g_stAccSyncStuct[g_usAccSyncCnt].usAccX = usAccX;
    g_stAccSyncStuct[g_usAccSyncCnt].usAccY = usAccY;
    g_stAccSyncStuct[g_usAccSyncCnt].usAccZ = usAccZ;
    g_usAccSyncCnt++;

    for (GU8 uchFuncOffsetID = 0; uchFuncOffsetID < __SYNC_FUNC_MAX__; uchFuncOffsetID++)
    {
        if (g_pstPpgSyncStuct[uchFuncOffsetID] == GH3X2X_PTR_NULL\
            || g_usPpgSyncCnt[uchFuncOffsetID] == 0)
        {
            continue;
        }

        GH3X2X_ALGO_LOG_PARAM("[%s]:ts=%d,%d.cnt=%d,%d\n", __FUNCTION__,\
                                                                g_stAccSyncStuct[g_usAccSyncCnt - 1].unTimeStamp,\
                                                                g_pstPpgSyncStuct[uchFuncOffsetID][g_usPpgSyncCnt[uchFuncOffsetID] - 1].unTimeStamp,\
                                                                g_usAccSyncCnt,\
                                                                g_usPpgSyncCnt[uchFuncOffsetID]);

        if (g_stAccSyncStuct[g_usAccSyncCnt - 1].unTimeStamp >=\
            g_pstPpgSyncStuct[uchFuncOffsetID][g_usPpgSyncCnt[uchFuncOffsetID] - 1].unTimeStamp\
            && g_usPpgSyncCnt[uchFuncOffsetID] > 1\
            && g_usAccSyncCnt > 1)
        {
            GH3X2X_ALGO_LOG_PARAM("[%s]:Algo Handle Flag!!!\n", __FUNCTION__);
            g_uchAlgoHandleFlag[uchFuncOffsetID] = 1;
        }
    }
#endif
}

void GH3X2X_TimestampSyncFillPpgSyncBuffer(GU32 unTimeStamp,
                                           const STGh3x2xFrameInfo * const pstFrameInfo)
{
#if (__GS_TIMESTAMP_READ_EN__)
    GU8 uchFuncOffsetID = 0;

    for(GU8 uchFunCnt = 0; uchFunCnt < __SYNC_FUNC_MAX__; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & pstFrameInfo->unFunctionID) == (1<<uchFunCnt))
        {
            uchFuncOffsetID = uchFunCnt;
            break;
        }
    }

    if (g_uchAlgoHandleFlag[uchFuncOffsetID] && ((g_unPpgIntFlag >> uchFuncOffsetID) & 0x1))
    {
        GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:func=%d,calc!!!\n", __FUNCTION__, uchFuncOffsetID);
        GH3X2X_TimestampSyncSend2AlgoCall(uchFuncOffsetID);
        g_usPpgSyncCnt[uchFuncOffsetID] = 0;
        g_uchAlgoHandleFlag[uchFuncOffsetID] = 0;
        g_unPpgIntFlag = g_unPpgIntFlag & ~(0x1 << uchFuncOffsetID);
    }

    STSyncPpgStuct *pstPpgSyncStuct = g_pstPpgSyncStuct[uchFuncOffsetID];

    if (pstFrameInfo == GH3X2X_PTR_NULL || pstPpgSyncStuct == GH3X2X_PTR_NULL)
    {
        return;
    }

    pstPpgSyncStuct[g_usPpgSyncCnt[uchFuncOffsetID]].unTimeStamp = unTimeStamp;
    pstPpgSyncStuct[g_usPpgSyncCnt[uchFuncOffsetID]].unFrameCnt = pstFrameInfo->punFrameCnt[0];
    memcpy(pstPpgSyncStuct[g_usPpgSyncCnt[uchFuncOffsetID]].unRawdata, pstFrameInfo->punFrameRawdata, __SYNC_RAWDATA_MAX_NUM__ * sizeof(GU32));
    memcpy(pstPpgSyncStuct[g_usPpgSyncCnt[uchFuncOffsetID]].unAgcinfo, pstFrameInfo->punFrameAgcInfo, __SYNC_AGCINFO_MAX_NUM__ * sizeof(GU32));
    memcpy(pstPpgSyncStuct[g_usPpgSyncCnt[uchFuncOffsetID]].unFlag, pstFrameInfo->punFrameFlag, __SYNC_FLAG_MAX_NUM__ * sizeof(GU32));
    g_usPpgSyncCnt[uchFuncOffsetID]++;

    if (g_usPpgSyncCnt[uchFuncOffsetID] >= __SYNC_PPG_FRAME_NUM__)
    {
        GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:acc timestamp receive error!!!\n", __FUNCTION__);
        GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:need to empty ppg buffer!!!\n", __FUNCTION__);
        GH3X2X_TimestampSyncSend2AlgoCall(uchFuncOffsetID);
        g_usPpgSyncCnt[uchFuncOffsetID] = 0;
        g_uchAlgoHandleFlag[uchFuncOffsetID] = 0;
        g_unPpgIntFlag = g_unPpgIntFlag & ~(0x1 << uchFuncOffsetID);
    }

    GH3X2X_ALGO_LOG_PARAM("[%s]:func=%d,ts=%d,%d.cnt=%d,%d\n", __FUNCTION__, uchFuncOffsetID,\
                                                            g_stAccSyncStuct[g_usAccSyncCnt - 1].unTimeStamp,\
                                                            g_pstPpgSyncStuct[uchFuncOffsetID][g_usPpgSyncCnt[uchFuncOffsetID] - 1].unTimeStamp,\
                                                            g_usAccSyncCnt,\
                                                            g_usPpgSyncCnt[uchFuncOffsetID]);

    if (g_stAccSyncStuct[g_usAccSyncCnt - 1].unTimeStamp >\
        g_pstPpgSyncStuct[uchFuncOffsetID][g_usPpgSyncCnt[uchFuncOffsetID] - 1].unTimeStamp\
        && g_usPpgSyncCnt[uchFuncOffsetID] > 1\
        && g_usAccSyncCnt > 1)
    {
        GH3X2X_ALGO_LOG_PARAM("[%s]:Algo Handle Flag!!!\n", __FUNCTION__);
        g_uchAlgoHandleFlag[uchFuncOffsetID] = 1;
    }
    else
    {
        g_uchAlgoHandleFlag[uchFuncOffsetID] = 0;
    }
#endif
}

