#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_hba.h"

#if (__USE_GOODIX_HR_ALGORITHM__)


void Gh3x2xSetHbaMode(GS32 nHbaScenario)
{
    g_unHrParamsArr[1] = nHbaScenario;
}

goodix_hba_ret goodix_hba_init_func(GU32 fs)
{
    goodix_hba_config stHbCfg;
    goodix_hba_ret stAlgoRet = GX_ALGO_HBA_SUCCESS;
    //GU8 uchChMapCnt = 0;
    //GU8 uchChIndex = 0;

    GCHAR uchHrVersion[100] = {0};
    goodix_hba_version((uint8_t*)uchHrVersion);    
    GH3X2X_HbaAlgoChnlMapDefultSet();
    
    GH3X2X_ALGO_LOG_PARAM("hba algorithm version : %s\r\n", uchHrVersion);
    GH3X2X_ALGO_LOG_PARAM("hba algorithm legal chnl num : %d\r\n", g_stHbaAlgoChnlMap.uchNum);

    if(g_stHbaAlgoChnlMap.uchNum > Gh3x2xGetHrAlgoSupportChnl())
    {
        GH3X2X_ALGO_LOG_PARAM("ERROR: hr algorithm legal chnl num is too big, check cfg !!! \r\n");
        return GX_ALGO_HBA_RWONG_INPUT;//while(1);
    }

    memset(&stHbCfg, 0, sizeof(goodix_hba_config));
    //for (uchChMapCnt = 0; uchChMapCnt < g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstFunctionInfo->uchChnlNum; ++uchChMapCnt)
    //{
        //uchChIndex = GH3X2X_BYTE_RAWDATA_GET_SLOT_ADC_NUM(g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pchChnlMap[uchChMapCnt]);
        //g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->puchFrameLastGain[uchChMapCnt] = GH3X2X_GAIN_VALUE_INVALID;
    //}
    /*
    if (MEMPOOL_IS_OK_E != goodix_mem_init(memaddr, memsize))
    {
        stAlgoRet = GX_ALGO_HBA_NO_MEMORY;
        GH3X2X_ALGO_LOG_PARAM("HB algo mem init error!\r\n");
        return stAlgoRet;
    }
    */
    stHbCfg.mode = (hba_test_mode)g_unHrParamsArr[0];
    stHbCfg.scence = (hba_scenes_e)g_unHrParamsArr[1];
    stHbCfg.fs = fs;
    stHbCfg.valid_channel_num = g_stHbaAlgoChnlMap.uchNum;
    stHbCfg.back_track_len = g_unHrParamsArr[2];
    stHbCfg.hba_latest_output_time = g_unHrParamsArr[3];
    stHbCfg.hba_earliest_output_time = g_unHrParamsArr[4];

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:params = %d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__,
                                                                            stHbCfg.mode,
                                                                            stHbCfg.scence,
                                                                            stHbCfg.fs,
                                                                            stHbCfg.valid_channel_num,
                                                                            stHbCfg.back_track_len,
                                                                            stHbCfg.hba_latest_output_time,
                                                                            stHbCfg.hba_earliest_output_time);

    stAlgoRet = goodix_hba_init(&stHbCfg);
    return stAlgoRet;
}

goodix_hba_ret goodix_hba_deinit_func(void)
{
    goodix_hba_deinit();
    return GX_ALGO_HBA_SUCCESS;
}


GS8 GH3x2xHrAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hba_init_func(pstFrameInfo ->pstFunctionInfo ->usSampleRate) == GX_ALGO_HBA_SUCCESS)        
    {
        
        chRet = GH3X2X_RET_OK;
        GH3X2X_ALGO_LOG_PARAM("[%s]GH3X2X_HbaInit success!!\r\n", __FUNCTION__);
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("hba init error!\r\n");
    }
    pstFrameInfo->pstAlgoRecordResult->uchUpdateFlag = 0;
    pstFrameInfo->pstAlgoRecordResult->snResult[0] = 0;
    return chRet;
}



GS8 GH3x2xHrAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GH3X2X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hba_deinit_func() == GX_ALGO_HBA_SUCCESS)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_ALGO_LOG_PARAM("hba deinit error!\r\n");
    }
    return chRet;
}




//call algo 2-level interface

GS8 GH3x2xHrAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    goodix_hba_input_rawdata stRawdata = {0};
    goodix_hba_result stResult = {0};
    
    stRawdata.frameid = GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.acc_x = pstFrameInfo->pusFrameGsensordata[0];
    stRawdata.acc_y = pstFrameInfo->pusFrameGsensordata[1];
    stRawdata.acc_z = pstFrameInfo->pusFrameGsensordata[2];
    stRawdata.sleep_flg = GH3x2xSleepFlagGet();

    GU8 uchChMapCnt = 0;
    GU8 uchRawdataIndex = 0;
    GS8 chAlgoRet = 0;

    for (uchChMapCnt = 0; uchChMapCnt < 3 * PPG_CHANNEL_NUM; ++uchChMapCnt)
    {
        if (g_stHbaAlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)] < 0xff)
        {
            uchRawdataIndex = g_stHbaAlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)];

            //make gain_adj_flg and cur_adj_flg to 0 for algo 1.0.5.0(alpha).
            stRawdata.gain_adj_flg[uchChMapCnt] = 0;// (pstFrameInfo->punFrameAgcInfo[uchRawdataIndex]) & 0xF;
            stRawdata.cur_adj_flg[uchChMapCnt] = 0;//((pstFrameInfo->punFrameFlag[0] >> uchChMapCnt) & 0x01);
            stRawdata.enable_flg[uchChMapCnt]  = 1;

            stRawdata.ppg_rawdata[uchChMapCnt] = pstFrameInfo->punFrameRawdata[uchRawdataIndex];
            GH3X2X_ALGO_LOG_PARAM("[%s]ppg_rawdata = %d,gain_adj_flg = %d,cur_adj_flg = %d\r\n",__FUNCTION__,\
                (int)stRawdata.ppg_rawdata[uchChMapCnt],\
                (int)stRawdata.gain_adj_flg[uchChMapCnt],\
                (int)stRawdata.cur_adj_flg[uchChMapCnt]);
        }
    }


    /* call algorithm, update result */
    if (goodix_hba_update(&stRawdata, &stResult) == GX_ALGO_HBA_SUCCESS)
    {
        if (stResult.hba_out_flag == 1)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = (GU8)stResult.hba_out_flag;
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)stResult.hba_out;
            pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)stResult.valid_score;
            pstFrameInfo->pstAlgoResult->snResult[2] = GH3X2X_ALGORITHM_GF32_2_GS32(stResult.hba_snr);
            pstFrameInfo->pstAlgoResult->snResult[3] = (GS32)stResult.valid_level;
            pstFrameInfo->pstAlgoResult->snResult[4] = (GS32)stResult.hba_acc_info;
            pstFrameInfo->pstAlgoResult->snResult[5] = (GS32)stResult.hba_acc_scence;

            GH3X2X_ALGO_LOG_PARAM("[%s]hr = %d,UpdateFlag = %d\r\n",__FUNCTION__,\
                (int)pstFrameInfo->pstAlgoResult->snResult[0],(int)pstFrameInfo->pstAlgoResult->uchUpdateFlag);

            pstFrameInfo->pstAlgoResult->usResultBit = 0x3F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
#if __GH3X2X_HR_OUTPUT_VALUE_STRATEGY_EN__
            Gh3x2xHrOutputValueStrategyPro(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
#endif
            GH3X2X_HrAlgorithmResultReport(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
            
            pstFrameInfo->pstAlgoRecordResult->uchUpdateFlag = (GU8)stResult.hba_out_flag;
            pstFrameInfo->pstAlgoRecordResult->snResult[0] = (GS32)stResult.hba_out;
        }
    }
    else
    {
        chAlgoRet = GH3X2X_RET_RESOURCE_ERROR;
        GH3X2X_ALGO_LOG_PARAM("no enough ram for algorithm error!\r\n");
        GH3X2X_ALGO_LOG_PARAM("please feedback to GOODIX!\r\n");
    }
    return chAlgoRet;
}

#endif

