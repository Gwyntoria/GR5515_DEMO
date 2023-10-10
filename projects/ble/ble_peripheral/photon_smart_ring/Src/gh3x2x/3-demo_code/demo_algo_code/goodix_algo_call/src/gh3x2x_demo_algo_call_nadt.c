#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "goodix_mem.h"
#include "goodix_nadt.h"

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)

#define NADT_CONFIG_SAMPLE_RATE_TYPE               (4)
#define NADT_CONFIG_MULTI_SAMPLE_RATE_TYPE         (4)

#define SOFT_WEAR_INPUT_DATA_CHNL0_INDEX   (0)
#define SOFT_WEAR_INPUT_DATA_ACCX_INDEX    (1)
#define SOFT_WEAR_INPUT_DATA_ACCY_INDEX    (2)
#define SOFT_WEAR_INPUT_DATA_ACCZ_INDEX    (3)
#define SOFT_WEAR_INPUT_COLOR_INDEX        (4)
#define SOFT_WEAR_INPUT_DATA_ADT_INDEX     (5) //ir  adt data
#define SOFT_WEAR_INPUT_DATA_BG_INDEX      (6)
#define SOFT_WEAR_INPUT_DATA_CAP0_INDEX    (7)
#define SOFT_WEAR_INPUT_DATA_CAP1_INDEX    (8)
#define SOFT_WEAR_INPUT_DATA_CAP2_INDEX    (9)
#define SOFT_WEAR_INPUT_DATA_CAP3_INDEX    (10)


#define SOFT_WEAR_INPUT_DATA_INDEX_TOTAL (11)
#define SOFT_WEAR_OUTPUT_DATA_INDEX_TOTAL (8)

#define WATCH_NUM     (3)

#if (WATCH_NUM == 1)
static GS32 lWearCap1 = 16400;
static GS32 lUnwearCap1 = 12400;
static GS32 lNoise1 = 300;
static GS32 lWearCap2 = 0;
static GS32 lUnwearCap2 = 0;
static GS32 lNoise2 = 0;
#elif (WATCH_NUM == 2)
static GS32 lWearCap1 = 20000;
static GS32 lUnwearCap1 = 16000;
static GS32 lNoise1 = 200;
static GS32 lWearCap2 = 3700;
static GS32 lUnwearCap2 = 1200;
static GS32 lNoise2 = 200;
#elif (WATCH_NUM == 3)
static GS32 lWearCap1 = 42500;
static GS32 lUnwearCap1 = 39500;
static GS32 lNoise1 = 150;
static GS32 lWearCap2 = 42500;
static GS32 lUnwearCap2 = 39500;
static GS32 lNoise2 = 150;
#endif

static GS32 ghbrelsult[10];
static GS32 glConfig[3] = { 0 };
static GU8  g_uchSoftAdtChannl0Color;   // 0: green   1: ir
static GU32 g_unNadtIrDefaultTimeOutRecord = 0;
static GU32 g_unNadtIrDefaultTimeOutSecondsThrd = __SOFT_ADT_IR_DETECT_TIMEOUT__;
static GU8 g_SoftWearQuality = 0;

void Gh3x2x_SetIRDetectTimeThreshold(GU32 unIRDetectTimeThreshold)
{
    g_unNadtIrDefaultTimeOutSecondsThrd = unIRDetectTimeThreshold;
}

void SoftWearControlInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    memset(&ghbrelsult, 0, sizeof(GS32)* 10);
    glConfig[0] = 8538608;
    //NADTControl(NADT_CONFIG_ADT_UNWEAR_THR_TYPE, glConfig); //初始化未佩戴阈值
    glConfig[0] = pstFrameInfo ->pstFunctionInfo ->usSampleRate;
    NADTControl(NADT_CONFIG_SAMPLE_RATE_TYPE, glConfig);//初始化频率
    // 电容阈值: 1
    glConfig[0] = lWearCap1;        // 佩戴电容值
    glConfig[1] = lUnwearCap1;      // 对空电容值
    glConfig[2] = lNoise1;          // 噪声值
    //NADTControl(NADT_CONFIG_CAP1_INFO_TYPE, glConfig);

    // 电容阈值: 2
    glConfig[0] = lWearCap2;        // 佩戴电容值
    glConfig[1] = lUnwearCap2;      // 对空电容值
    glConfig[2] = lNoise2;          // 噪声值
    //NADTControl(NADT_CONFIG_CAP2_INFO_TYPE, glConfig);
    NADTProcInit();     //初始化
}



void GH3X2X_SoftWearOffDetInit(void)
{
    GS32 nHbSampleRate = g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR] -> pstFunctionInfo -> usSampleRate;
    //HBA_NADTProcInit();
    NADTProcInit();
    NADTControl(NADT_CONFIG_MULTI_SAMPLE_RATE_TYPE,&nHbSampleRate);
    GH3X2X_ALGO_LOG_PARAM("[GH3X2X_SoftWearOffDetInit] nHbSampleRate = %d",(int)nHbSampleRate);
}

void GH3x2xCapNadtResultPro(const STGh3x2xFrameInfo * const pstFrameInfo,GS32  pnSoftWearOffDetResult[])
{
    if (pnSoftWearOffDetResult[3])            // 电容更新标记,更新佩戴检测阈值
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        lUnwearCap1 = pnSoftWearOffDetResult[4];
        lWearCap1 = pnSoftWearOffDetResult[5];
        lUnwearCap2 = pnSoftWearOffDetResult[6];
        lWearCap2 = pnSoftWearOffDetResult[7];
    }
    if(g_SoftWearQuality != pnSoftWearOffDetResult[2])
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        g_SoftWearQuality = pnSoftWearOffDetResult[2];
    }
    if(pstFrameInfo->punFrameCnt == 0)
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
    }
    if(pstFrameInfo->pstAlgoResult->uchUpdateFlag == 1)
    {
        pstFrameInfo->pstAlgoResult->usResultBit = 0x7;
        pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);      
        pstFrameInfo->pstAlgoResult->snResult[0] = pnSoftWearOffDetResult[0]; // (bit0-bit1)：佩戴状态（0默认，1佩戴，2脱落，3非活体）； (bit2)：疑似脱落标记（0正常，1疑似脱落）；
        pstFrameInfo->pstAlgoResult->snResult[1] = pnSoftWearOffDetResult[1]; //活体检测置信度
        pstFrameInfo->pstAlgoResult->snResult[2] = pnSoftWearOffDetResult[2]; //佩戴质量（1紧佩戴，2松佩戴，3极度松佩戴，4脱落）；
        if(pstFrameInfo->punFrameCnt == 0)
        {
            pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
            pstFrameInfo->pstAlgoResult->snResult[3] = lUnwearCap1; //脱落电容值1；
            pstFrameInfo->pstAlgoResult->snResult[4] = lWearCap1; //佩戴电容值1；
            pstFrameInfo->pstAlgoResult->snResult[5] = lUnwearCap2; //脱落电容值2；
            pstFrameInfo->pstAlgoResult->snResult[6] = lWearCap2; //佩戴电容值2；
        }
        else
        {
            pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
            pstFrameInfo->pstAlgoResult->snResult[3] = pnSoftWearOffDetResult[4]; //脱落电容值1；
            pstFrameInfo->pstAlgoResult->snResult[4] = pnSoftWearOffDetResult[5]; //佩戴电容值1；
            pstFrameInfo->pstAlgoResult->snResult[5] = pnSoftWearOffDetResult[6]; //脱落电容值2；
            pstFrameInfo->pstAlgoResult->snResult[6] = pnSoftWearOffDetResult[7]; //佩戴电容值2；
        }
    }
}

GS8 GH3x2xSoftAdtAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GU32 nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_INDEX_TOTAL] = {0};
    GS32 pnSoftWearOffDetResult[SOFT_WEAR_OUTPUT_DATA_INDEX_TOTAL] = {0};

    if(pstFrameInfo)
    {
        GU32 unLedAdjFlag = 0;

        if(pstFrameInfo->punFrameFlag[0]&0x00000001)  //led current change
        {
            unLedAdjFlag = 1;
        }
        if((0xFF != pstFrameInfo->puchFrameLastGain[0])&&((pstFrameInfo->punFrameAgcInfo[0]&0x0000000F) != pstFrameInfo->puchFrameLastGain[0]))  //tia gain change
        {
            unLedAdjFlag = 1;
        }

        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_ACCX_INDEX] = pstFrameInfo->pusFrameGsensordata[0];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_ACCY_INDEX] = pstFrameInfo->pusFrameGsensordata[1];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_ACCZ_INDEX] = pstFrameInfo->pusFrameGsensordata[2];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_CHNL0_INDEX] = pstFrameInfo->punFrameRawdata[0];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_CHNL0_INDEX] |= (unLedAdjFlag << 26);  //led adj flag
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_COLOR_INDEX]      =  g_uchSoftAdtChannl0Color;      //0:GREEN  1:IR
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_ADT_INDEX]   = pstFrameInfo->punFrameRawdata[1];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_BG_INDEX]    = pstFrameInfo->punFrameRawdata[2];

        if (g_uchAlgoCapEnable == 1)
        {
            for (int k = 0; k < 4; k++)
            {
                nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_CAP0_INDEX + k] = pstFrameInfo->pstFrameCapdata->unCapData[k];//  [PPGACCLEN + ALGOLEN + guchCompareNum + k]; //电容
            }
        }

        GU8 uchRet = NADTProc(nSoftWearOffInputBuf,pnSoftWearOffDetResult);
        GU32 unNadtIrDefaultTimeOutThrd = g_unNadtIrDefaultTimeOutSecondsThrd * pstFrameInfo->pstFunctionInfo->usSampleRate;
        if (g_unNadtIrDefaultTimeOutRecord >= unNadtIrDefaultTimeOutThrd && ((pnSoftWearOffDetResult[0] & 0x3) == 0))
        {
            pnSoftWearOffDetResult[0] = pnSoftWearOffDetResult[0] | 0x1;
            g_unNadtIrDefaultTimeOutRecord = 0;
        }

        if ((pnSoftWearOffDetResult[0] & 0x2) == 0)
        {
            if (GH3X2X_FUNCTION_SOFT_ADT_IR == pstFrameInfo->unFunctionID)
            {
                g_unNadtIrDefaultTimeOutRecord++;
            }
        }

        if (uchRet != 0)
        {
            GH3X2X_ALGO_LOG_PARAM("[GH3x2xSoftAdtAlgoExe]NADTProc: error!!!\r\n");
        }

        if (g_uchAlgoCapEnable == 1)
        {
            GH3x2xCapNadtResultPro(pstFrameInfo, pnSoftWearOffDetResult);
        }
        else
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
            pstFrameInfo->pstAlgoResult->usResultBit = 0x3;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
            pstFrameInfo->pstAlgoResult->snResult[0] = pnSoftWearOffDetResult[0]; // (bit0-bit1)：佩戴状态（0默认，1佩戴，2脱落，3非活体）； (bit2)：疑似脱落标记（0正常，1疑似脱落）；
            pstFrameInfo->pstAlgoResult->snResult[1] = pnSoftWearOffDetResult[1]; //活体检测置信度
        }

        //report
        if (g_uchSoftAdtChannl0Color == 0)
        {
            GH3X2X_SoftAdtGreenAlgorithmResultReport(pstFrameInfo->pstAlgoResult, GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt)));
        }
        else if(g_uchSoftAdtChannl0Color == 1)
        {
            GH3X2X_SoftAdtIrAlgorithmResultReport(pstFrameInfo->pstAlgoResult, GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt)));
        }
    }
    return GH3X2X_RET_OK;
}


GS8 GH3x2xSoftAdtAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(pstFrameInfo)
    {
        g_SoftWearQuality  = 0;
        g_uchSoftAdtChannl0Color = 1 * (GH3X2X_FUNCTION_SOFT_ADT_IR == (pstFrameInfo ->unFunctionID));

        GH3X2X_ALGO_LOG_PARAM("[SoftAdt]CH0 slot = %d, rx = %d\r\n", GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[0]),GH3X2X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[0]));
        GH3X2X_ALGO_LOG_PARAM("[SoftAdt]CH1 slot = %d, rx = %d\r\n", GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[1]),GH3X2X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[1]));
        GH3X2X_ALGO_LOG_PARAM("[SoftAdt]CH2 slot = %d, rx = %d\r\n", GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[2]),GH3X2X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[2]));
        GH3X2X_ALGO_LOG_PARAM("[SoftAdt]g_uchSoftAdtChannl0Color = %d\r\n", g_uchSoftAdtChannl0Color);

        SoftWearControlInit(pstFrameInfo);
    }
    return GH3X2X_RET_OK;
}


GS8 GH3x2xSoftAdtAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GH3X2X_ALGO_LOG_PARAM("[SoftAdt]%s\r\n", __FUNCTION__);
    NADTProcDeinit();
    g_unNadtIrDefaultTimeOutRecord = 0;
    return GH3X2X_RET_OK;
}

#endif
