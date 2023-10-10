#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "goodix_hba.h"
#include "goodix_hrv.h"

#if (__USE_GOODIX_HRV_ALGORITHM__)

GU32 g_unHrvLastHrResult = 0;
extern GU8 g_uchHrvInterEnable;
extern GS32 g_nHrvParamsArr[5];

//hrv
goodix_hba_ret goodix_hrv_init_func(GU32 fs)
{
    GS32 nRet = GX_ALGO_HBA_SUCCESS;
    GCHAR uchHrvVersion[100] = {0};
    GS32 nHrvAccThrConfigArr[5] = {0};
    memcpy(uchHrvVersion, HRVAlgoVersion(), strlen((GCHAR *)HRVAlgoVersion()) + 1);
    GH3X2X_ALGO_LOG_PARAM("hrv algorithm version : %s\r\n", uchHrvVersion);
    for (GU8 nArrCnt = 1 ; nArrCnt <= 4 ; nArrCnt ++)
    {
        nHrvAccThrConfigArr[nArrCnt] = g_nHrvParamsArr[nArrCnt] * 10000;
    }
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:params = %d,%d,%d,%d,%d,\r\n", __FUNCTION__,
                                                                g_nHrvParamsArr[0],
                                                                nHrvAccThrConfigArr[1],
                                                                nHrvAccThrConfigArr[2],
                                                                nHrvAccThrConfigArr[3],
                                                                nHrvAccThrConfigArr[4]);
    nRet = HRVAlgoInit(g_nHrvParamsArr[0], fs, &nHrvAccThrConfigArr[1]);
    if (nRet != 0)
    {
        GH3X2X_ALGO_LOG_PARAM("hrv init errorcode : %x\r\n", (unsigned int)nRet);
    }
    return (goodix_hba_ret)nRet;
}

goodix_hba_ret goodix_hrv_deinit_func(void)
{
    GS32 nRet = GX_ALGO_HBA_SUCCESS;
    nRet = HRVAlgoDeInit();
    if (nRet != 0)
    {
        GH3X2X_ALGO_LOG_PARAM("hrv init errorcode : %x\r\n", (unsigned int)nRet);
    }
    return (goodix_hba_ret)nRet;
}

GS8 GH3x2xHrvAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hrv_init_func(pstFrameInfo ->pstFunctionInfo ->usSampleRate) == GX_ALGO_HBA_SUCCESS)        
    {
        pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("hrv init error!\r\n");
    }
    g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->uchUpdateFlag = 0;
    g_unHrvLastHrResult = 0;
    return chRet;
}



GS8 GH3x2xHrvAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;


    GH3X2X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hrv_deinit_func() == 0)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("hrv deinit error!\r\n");
    }
    return chRet;
}


GS8 GH3x2xHrvAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    GS32 usOutputData[6]   = {0};
    ST_HRV_INPUT_INFO siRawdataInput;
    memset((GU8*)&siRawdataInput, 0, sizeof(ST_HRV_INPUT_INFO));
    GU8 uchGainValue = 0;

    for (int nRawdatacnt = 0 ; nRawdatacnt< HRV_CHNL_MAX_NUM ; nRawdatacnt ++)
    {
        siRawdataInput.ppg_green[nRawdatacnt] = pstFrameInfo->punFrameRawdata[nRawdatacnt];
        uchGainValue = GH3X2X_GET_LOW_4BITS(pstFrameInfo->punFrameAgcInfo[nRawdatacnt]);
        if ((pstFrameInfo->puchFrameLastGain[nRawdatacnt] != uchGainValue) && \
            (pstFrameInfo->puchFrameLastGain[nRawdatacnt] != GH3X2X_GAIN_VALUE_INVALID))
        {
            siRawdataInput.green_gain_adj_flag[nRawdatacnt] = 1;
            pstFrameInfo->puchFrameLastGain[nRawdatacnt] = uchGainValue;
        }
        siRawdataInput.green_cur_adj_flag[nRawdatacnt] = ((pstFrameInfo->punFrameFlag[0]) & 0x01);
    }
    siRawdataInput.acc_x = pstFrameInfo->pusFrameGsensordata[0];
    siRawdataInput.acc_y = pstFrameInfo->pusFrameGsensordata[1];
    siRawdataInput.acc_z = pstFrameInfo->pusFrameGsensordata[2];
    siRawdataInput.frame_id = GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    if (g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->uchUpdateFlag)
    {
        g_unHrvLastHrResult = g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->snResult[0];
    }
    siRawdataInput.hr = g_unHrvLastHrResult;
    
    GU8 uchUpdateFlag = HRVCalculateAlgo(&siRawdataInput, usOutputData);

    if (uchUpdateFlag)
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = uchUpdateFlag;
#if 0
        pstAlgoResArr[*pusAlgoResCnt].snHrvOut0   = (GS32)usOutputData[0] * g_unHrvSamplingFreq / 1000;
        pstAlgoResArr[*pusAlgoResCnt].snHrvOut1   = (GS32)usOutputData[1] * g_unHrvSamplingFreq / 1000;
        pstAlgoResArr[*pusAlgoResCnt].snHrvOut2   = (GS32)usOutputData[2] * g_unHrvSamplingFreq / 1000;
        pstAlgoResArr[*pusAlgoResCnt].snHrvOut3   = (GS32)usOutputData[3] * g_unHrvSamplingFreq / 1000;
#else
        pstFrameInfo->pstAlgoResult->snResult[0]   = (GS32)usOutputData[0];
        pstFrameInfo->pstAlgoResult->snResult[1]   = (GS32)usOutputData[1];
        pstFrameInfo->pstAlgoResult->snResult[2]   = (GS32)usOutputData[2];
        pstFrameInfo->pstAlgoResult->snResult[3]   = (GS32)usOutputData[3];
#endif
        pstFrameInfo->pstAlgoResult->snResult[4]   = (GS32)usOutputData[4];
        pstFrameInfo->pstAlgoResult->snResult[5]   = (GS32)usOutputData[5];
        pstFrameInfo->pstAlgoResult->snResult[6]   = (GS32)g_unHrvLastHrResult;
        pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
        pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
        GH3X2X_HrvAlgorithmResultReport(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
    }
    return GH3X2X_RET_OK;
}

#endif
