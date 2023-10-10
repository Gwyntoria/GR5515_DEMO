#include <stdio.h>
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_call.h"
#include "goodix_mem.h"
#include "goodix_algo.h"
#include "goodix_algo_bt.h"

#if (__USE_GOODIX_BT_ALGORITHM__)

/*****体温全局变量*****/
//GS32 resis[] = {72790,61030,53590,49190,45180,41540,36660};
//GS8  temp[] = {25,29,32,34,36,38,41}; 

//GS32 resis[] = {100000,84080,74010,68050,62630,57690,51090};
//GS8  temp[] = {25,29,32,34,36,38,41}; 
GU32 g_unBtLastHrResult = 0;

STGoodixAlgorithmBtControlConfigParametersBuffer gstBtCFG;
STGoodixAlgorithmBtInputRef                      gstBtInputRef;
STGoodixAlgorithmBtCalculateOutputResult         gstBtAlgoOUT;
//STGoodixAlgorithmBtFinalOutputResult             gstBtAlgoFinal;
STGoodixAlgorithmInfo                            gstBtAlgoInfo;

/**
 * @brief memory malloc function
 * @param size
 * @return pointer of memory
*/
void* GoodixSensorAlgoMalloc(GS32 size)
{
    return goodix_mem_malloc(size);
}

/**
 * @brief memory free function
 * @param addr
*/
void GoodixSensorAlgoFree(void* addr)
{
    goodix_mem_free(addr);
}

/**
 * @brief algorithm log function
 * @param addr
*/
void GoodixSensorAlgoLog(GCHAR* pLogStr)
{
    //GH3X2X_ALGO_LOG_PARAM("[%s]:%s\n", __FUNCTION__, pLogStr);
}

GS8 GH3x2xBtAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    //gstBtAlgoInfo.stAlgorithmConfig.pnParametersBuffer = &gstBtCFG;
    gstBtAlgoInfo.stAlgorithmCalcOutput.pnResult = &gstBtAlgoOUT;
    gstBtAlgoInfo.stAlgorithmCalcInput.pnRefBuffer = &gstBtInputRef;
    gstBtCFG.unChannelNum = 2;

    if(BT_ERRORCODE_OK != GoodixBtAlgoInit(&gstBtAlgoInfo))
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    pstFrameInfo->pstAlgoResult->usResultBit = 0x1F;
    g_unBtLastHrResult = 0;
    memset(&gstBtAlgoOUT, 0, sizeof(STGoodixAlgorithmBtCalculateOutputResult));
    memset(&gstBtInputRef, 0, sizeof(STGoodixAlgorithmBtInputRef));

    return GH3X2X_RET_OK;
}

GS8 GH3x2xBtAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(BT_ERRORCODE_OK != GoodixBtAlgoDeinit(&gstBtAlgoInfo))
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }
    return GH3X2X_RET_OK;
}

GS8  GH3x2xBtAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS32 TempRawdata[4] = {0};
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    for(GU8 i = 0; i < 4; i++)
    {
        TempRawdata[i] = (int)(pstFrameInfo->pstFrameTempdata->unTempData[i]);
    }

    if (g_uchAlgoTempEnable == 0)
    {
        for(GU8 i = 0; i < 4; i++)
        {
            TempRawdata[i] = (int)(pstFrameInfo->punFrameRawdata[i]);
        }
    }

    gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata = TempRawdata;

    if (g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->uchUpdateFlag)
    {
        g_unBtLastHrResult = g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->snResult[0];
    }
    gstBtInputRef.unHR = g_unBtLastHrResult;

    //GH3X2X_ALGO_LOG_PARAM("[%s]:%d,%d,%d,%d\n", __FUNCTION__, gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata[0], gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata[1], gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata[2], gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata[3]);

    if(BT_ERRORCODE_OK == GoodixBtAlgoCalculate(&gstBtAlgoInfo))
    {
        //if(gstBtAlgoOUT.uchRefresh)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
            pstFrameInfo->pstAlgoResult->usResultBit = 0x3F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
            if (0!=gstBtAlgoInfo.stAlgorithmCalcOutput.pnMiddleBaseBuffer)
            {
                for (int i =0; i < 2; i++)
                {  
                    pstFrameInfo->pstAlgoResult->snResult[i] = gstBtAlgoInfo.stAlgorithmCalcOutput.pnMiddleBaseBuffer[i];//ntc0
                    //pstFrameInfo->pstAlgoResult->snResult[1] = gstBtAlgoOUT.nFPCBT;//ntc1
                }
            }
            pstFrameInfo->pstAlgoResult->snResult[2] = gstBtAlgoOUT.uchState;
            pstFrameInfo->pstAlgoResult->snResult[3] = gstBtAlgoOUT.uchConfidence;
            pstFrameInfo->pstAlgoResult->snResult[4] = gstBtAlgoOUT.nCBT;
            pstFrameInfo->pstAlgoResult->snResult[5] = g_unBtLastHrResult;
        }
        chRet = GH3X2X_RET_OK;
    }
    return chRet;
}


#endif
