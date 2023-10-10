
#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_ecg.h"

#if (__USE_GOODIX_ECG_ALGORITHM__)

#define GH3X2X_SINGLE_CLASSIFICATION_TIME 32


goodix_ecg_ret goodix_ecg_init_func(GU32 fs)
{
    GCHAR uchEcgVersion[100] = {0};
    goodix_ecg_version((uint8_t*)uchEcgVersion);
    GH3X2X_ALGO_LOG_PARAM("ecg algorithm version : %s\r\n", uchEcgVersion);

//    goodix_ecg_ret stAlgoRet = GX_ALGO_SUCCESS;
    goodix_ecg_config stEcgCfg = {0};
    /*
    if (MEMPOOL_IS_OK_E != goodix_mem_init(memaddr, memsize))
    {
        stAlgoRet = GX_ALGO_NO_MEMORY;
        GH3X2X_ALGO_LOG_PARAM("ECG algo mem init error!\r\n");
        return stAlgoRet;
    }
    */

    stEcgCfg.fs = fs;
    stEcgCfg.valid_channel_num = (uint32_t)g_nEcgParamsArr[1];
    stEcgCfg.qrs_flag = (uint32_t)g_nEcgParamsArr[2];
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:params = %d,%d,%d,\r\n", __FUNCTION__, stEcgCfg.fs, stEcgCfg.valid_channel_num, stEcgCfg.qrs_flag);
    goodix_ecg_init(&stEcgCfg);
    g_uchClassificationFlag = 0;
    g_unEcgTimeSeconds = 0;
    return GX_ALGO_SUCCESS;
}

goodix_ecg_ret goodix_ecg_deinit_func(void)
{
    goodix_ecg_deinit();
    return GX_ALGO_SUCCESS;
}

GS8 GH3x2xEcgAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GU16 usSampleRate = pstFrameInfo ->pstFunctionInfo ->usSampleRate;    
    GH3X2X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_ecg_init_func(usSampleRate) == GX_ALGO_SUCCESS)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("ecg init error!\r\n");
    }
    g_unEcgFrameCnt  = 0;
    GH3X2X_SetNewDataFlag(GH3X2X_FUNC_OFFSET_ECG, GH3X2X_IS_NEW_RAWDATA);

    *(pstFrameInfo->punFrameCnt) = 0;
    return chRet;
}


GS8 GH3x2xEcgAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_ecg_deinit_func() == GX_ALGO_SUCCESS)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("ecg deinit error!\r\n");
    }
    return chRet;
}

GS8 GH3x2xEcgAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo)
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }
    goodix_ecg_input_rawdata stRawdata = {0};
    goodix_ecg_result stResult;
    GS8 chAlgoRet = 0;

    //GU8 uchChnlNum = pstFrameInfo->pstFunctionInfo->uchChnlNum; 
    stRawdata.frameid = GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.acc_x = pstFrameInfo->pusFrameGsensordata[0];
    stRawdata.acc_y = pstFrameInfo->pusFrameGsensordata[1];
    stRawdata.acc_z = pstFrameInfo->pusFrameGsensordata[2];
    stRawdata.ecg_rawdata[0] = pstFrameInfo->punFrameRawdata[0];
    stRawdata.ecg_rawdata[1] = pstFrameInfo->punFrameRawdata[1];

    stRawdata.ecg_qc_flag = GH3X2X_CHECK_BIT_IS_SET(pstFrameInfo->punFrameFlag[2], 0x01);
    

    if (g_unEcgFrameCnt <= ECG_FRAME_UNUSED) /* the first 5 frame data can't be used for algo */
    {
        g_unEcgFrameCnt ++;
        stRawdata.ecg_qc_flag = 1;
    }

    if (1 == stRawdata.ecg_qc_flag)     /* when fast recovery,data can't be used for algo */
    {
        return chAlgoRet;
    }
    
    goodix_ecg_ret stRet = GX_ALGO_SUCCESS;
    
    stRet = goodix_ecg_update(&stRawdata, &stResult);
#if 0
    if (*(g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->punFrameCnt) %\
        (g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_ECG] ->pstFunctionInfo->usSampleRate) == 0\
        && g_unEcgTimeSeconds < GH3X2X_SINGLE_CLASSIFICATION_TIME)
    {
        g_unEcgTimeSeconds ++;    
    }
    if (g_unEcgTimeSeconds >= GH3X2X_SINGLE_CLASSIFICATION_TIME)
    {        
        goodix_ecg_classification_update((goodix_classification_flag*)&g_uchClassificationFlag);
        g_unEcgTimeSeconds = 0;
    }
#endif
    if (stRet == GX_ALGO_SUCCESS)
    {
        
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        pstFrameInfo->pstAlgoResult->snResult[0] = GH3x2x_Round(stResult.ecg_out[0] * ECG_OUT_FACTOR);
        pstFrameInfo->pstAlgoResult->snResult[2] = GH3x2x_Round(stResult.ecg_out[1] * ECG_OUT_FACTOR);

        GH3X2X_ALGO_LOG_PARAM("ecg out0 = %.3f\r\n",stResult.ecg_out[0]);
        GH3X2X_ALGO_LOG_PARAM("ecg out1 = %.3f\r\n",stResult.ecg_out[1]);
        //GH3X2X_SAMPLE_ALGO_LOG_PARAM("bpm result = %d\r\n",(GS32)(stResult.ecg_bpm + 0.5f));

        pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)(stResult.ecg_bpm + 0.5f);
#if 1
        pstFrameInfo->pstAlgoResult->usResultBit = 0x1 |\
                                                 ((0x1 << 1) * (pstFrameInfo->pstAlgoResult->snResult[1] != 0)) |\
                                                 ((0x1 << 2) * (g_nEcgParamsArr[1] == 2));
        pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
#endif
        GH3X2X_EcgAlgorithmResultReport(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("no enough ram for algorithm error!\r\n");
        GH3X2X_ALGO_LOG_PARAM("please feedback to GOODIX!\r\n");
        chAlgoRet = GH3X2X_RET_RESOURCE_ERROR;
    }
    return chAlgoRet;
}

GU8 GH3x2xEcgAlgoClassification(void)
{
    goodix_ecg_classification_update((goodix_classification_flag*)&g_uchClassificationFlag);
    return g_uchClassificationFlag;
}


#endif
