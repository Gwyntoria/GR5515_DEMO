#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_algo.h"
#include "goodix_spo2.h"

#if (__USE_GOODIX_SPO2_ALGORITHM__)


goodix_spo2_ret goodix_spo2_init_func(GU32 fs)
{
    goodix_spo2_ret stAlgoRet = GX_ALGO_SPO2_SUCCESS;
    //GU8 uchChMapCnt = 0;
    //GU8 uchChIndex  = 0;

    GH3X2X_Spo2AlgoChnlMapDefultSet();

    GCHAR uchSpo2Version[100] = {0};
    goodix_spo2_version((uint8_t*)uchSpo2Version);
    GH3X2X_ALGO_LOG_PARAM("spo2 algorithm version : %s\r\n", uchSpo2Version);
    GH3X2X_ALGO_LOG_PARAM("spo2 algorithm legal chnl num : %d\r\n", g_stSpo2AlgoChnlMap.uchNum);

    if(g_stSpo2AlgoChnlMap.uchNum > Gh3x2xGetSpo2AlgoSupportChnl())
    {
        GH3X2X_ALGO_LOG_PARAM("ERROR: spo2 algorithm legal chnl num is too big, check cfg !!! \r\n");
        return GX_ALGO_SPO2_RWONG_INPUT;//while(1);
    }

#if 1
    goodix_spo2_config stSpo2Cfg = { 
            g_stSpo2AlgoChnlMap.uchNum, fs,// 1. 原始数据配置信息
            g_nSpo2ParamssArr[0],
            g_nSpo2ParamssArr[1],
            g_nSpo2ParamssArr[2],
            g_nSpo2ParamssArr[3],//3. 心率使能标志
            g_nSpo2ParamssArr[4],//4. 佩戴模式
            g_nSpo2ParamssArr[5], g_nSpo2ParamssArr[6], g_nSpo2ParamssArr[7], g_nSpo2ParamssArr[8],//6. Acc相关
            g_nSpo2ParamssArr[9],g_nSpo2ParamssArr[10],
            };
#else
    goodix_spo2_config stSpo2Cfg = {g_stSpo2AlgoChnlMap.uchNum, fs,// 1. 原始数据配置信息
                                    -162708, -30749, 1035316,
                                    1,//3. 心率使能标志
                                    0,//4. 佩戴模式
                                    30, 15, 4, 3,//6. Acc相关
                                    0, 0
                                    };
#endif

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:params = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__,
                                                                            stSpo2Cfg.valid_chl_num,
                                                                            stSpo2Cfg.raw_fs,
                                                                            stSpo2Cfg.cali_coef_a,
                                                                            stSpo2Cfg.cali_coef_b,
                                                                            stSpo2Cfg.cali_coef_c,
                                                                            stSpo2Cfg.hb_en_flg,
                                                                            stSpo2Cfg.wear_mode,
                                                                            stSpo2Cfg.acc_thr_max,
                                                                            stSpo2Cfg.acc_thr_min,
                                                                            stSpo2Cfg.acc_thr_scale,
                                                                            stSpo2Cfg.acc_thr_num,
                                                                            stSpo2Cfg.ctr_en_flg,
                                                                            stSpo2Cfg.ctr_red_thr);

    stAlgoRet = goodix_spo2_init(&stSpo2Cfg);
    return stAlgoRet;
}

goodix_spo2_ret goodix_spo2_deinit_func(void)
{
    goodix_spo2_deinit();
    return GX_ALGO_SPO2_SUCCESS;
}

GS8 GH3x2xSpo2AlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_spo2_init_func(pstFrameInfo ->pstFunctionInfo ->usSampleRate) == GX_ALGO_SPO2_SUCCESS)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("spo2 init error!\r\n");
    }
    return chRet;
}


GS8 GH3x2xSpo2AlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_spo2_deinit_func() == GX_ALGO_SPO2_SUCCESS)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("spo2 deinit error!\r\n");
    }
    return chRet;
}


GS8 GH3x2xSpo2AlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }
    goodix_spo2_input_rawdata stRawdata = {0};
    goodix_spo2_result stResult;
    goodix_spo2_ret emAlgoRet = GX_ALGO_SPO2_SUCCESS;
    
    //GU8 uchChnlNum = pstFrameInfo->pstFunctionInfo->uchChnlNum; 
    stRawdata.frameid = GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.bit_num = 24;
    stRawdata.acc_x = pstFrameInfo->pusFrameGsensordata[0];
    stRawdata.acc_y = pstFrameInfo->pusFrameGsensordata[1];
    stRawdata.acc_z = pstFrameInfo->pusFrameGsensordata[2];
    stRawdata.wear_on_flag = 1;
    
    /* call algorithm, update result */
    memset(&stResult, 0, sizeof(goodix_spo2_result));


    GU8 uchChMapCnt = 0;
    //GU8 uchGainValue = GH3X2X_GAIN_VALUE_INVALID;

    
    stRawdata.wear_on_flag = 1;

    for (uchChMapCnt = 0; uchChMapCnt < CHIP_PPG_CHL_NUM * 3; ++uchChMapCnt)
    {
        if (g_stSpo2AlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)] < 0xff)
        {
            GU8 uchRawdataIndex = g_stSpo2AlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)];

            stRawdata.ch_agc_drv0[uchChMapCnt]  = ((pstFrameInfo->punFrameAgcInfo[uchRawdataIndex]>>8) & 0xFF);
            stRawdata.ch_agc_drv1[uchChMapCnt]  = ((pstFrameInfo->punFrameAgcInfo[uchRawdataIndex]>>16) & 0xFF);
            stRawdata.ch_agc_gain[uchChMapCnt]  = (pstFrameInfo->punFrameAgcInfo[uchRawdataIndex] & 0xF);

            if (((pstFrameInfo->puchFrameLastGain[uchRawdataIndex] != stRawdata.ch_agc_gain[uchChMapCnt]) && \
                (pstFrameInfo->puchFrameLastGain[uchRawdataIndex] != GH3X2X_GAIN_VALUE_INVALID)) ||\
                ((pstFrameInfo->punFrameFlag[0]) & 0x01))
            {
                stRawdata.gain_adj_flg[uchChMapCnt] = 1;
                pstFrameInfo->puchFrameLastGain[uchRawdataIndex] = stRawdata.ch_agc_gain[uchChMapCnt];
            }
            stRawdata.cur_adj_flg[uchChMapCnt] = (((pstFrameInfo->punFrameFlag[0]) >> uchRawdataIndex) & 0x01);
            stRawdata.enable_flg[uchChMapCnt]  = 1;
            
            stRawdata.ppg_rawdata[uchChMapCnt] = pstFrameInfo->punFrameRawdata[uchRawdataIndex];
            GH3X2X_ALGO_LOG_PARAM("[%s]ppg_rawdata = %d,gain_adj_flg = %d,cur_adj_flg = %d\r\n",__FUNCTION__,\
                (int)stRawdata.ppg_rawdata[uchChMapCnt],\
                (int)stRawdata.gain_adj_flg[uchChMapCnt],\
                (int)stRawdata.cur_adj_flg[uchChMapCnt]);
        }
    }
    
    
    emAlgoRet = goodix_spo2_calc(&stRawdata, &stResult);
    if (emAlgoRet == GX_ALGO_SPO2_SUCCESS || emAlgoRet == GX_ALGO_SPO2_FRAME_UNCOMPLETE || emAlgoRet == GX_ALGO_SPO2_WIN_UNCOMPLETE)
    {
        if (stResult.final_calc_flg == 1)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = (GU8)stResult.final_calc_flg;
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)(GH3x2x_Round((float)stResult.final_spo2/10000));
            pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)stResult.final_r_val;
            pstFrameInfo->pstAlgoResult->snResult[2] = (GS32)stResult.final_confi_coeff;
            pstFrameInfo->pstAlgoResult->snResult[3] = (GS32)stResult.final_valid_level;
            pstFrameInfo->pstAlgoResult->snResult[4] = (GS32)stResult.final_hb_mean;
            pstFrameInfo->pstAlgoResult->snResult[5] = (GS32)stResult.final_invalidFlg;
            pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
            GH3X2X_Spo2AlgorithmResultReport(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
        }
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("error code = 0x%x, please feedback to GOODIX!\r\n", (int)emAlgoRet);
    }
    return (GS8)emAlgoRet;
}

#endif
