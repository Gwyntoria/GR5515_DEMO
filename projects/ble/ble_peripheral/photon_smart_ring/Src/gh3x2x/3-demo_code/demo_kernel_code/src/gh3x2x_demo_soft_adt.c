/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_adt.c
 *
 * @brief   gh3x2x adt functions
 *
 * @version ref gh3x2x_drv_version.h
 *
 */


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gh3x2x_demo_config.h"
#include "gh3x2x_demo_inner.h"
#include "gh3x2x_drv.h"
#include "gh3x2x_demo_soft_adt.h"

/**********佩戴检测全局变量**********/
GS32 ghbinfo[12] = { 0 };
GS32 glCapValue[2] = { 0 };
GS32 gnRawDataArr[15] = { 0 };
GBOOL gbFirst = NADTCAP_BYTE_TRUE;
GU8 gbUnWear = NADTCAP_BYTE_FALSE;
GS32 g_lHbCount = 0;
GU8 gbase_col = 0;
#define WATCH_NUM     (3)

#if (WATCH_NUM == 1)
GS32 lWearCap1 = 16400;
GS32 lUnwearCap1 = 12400;
GS32 lNoise1 = 300;
GS32 lWearCap2 = 0;
GS32 lUnwearCap2 = 0;
GS32 lNoise2 = 0;
#elif (WATCH_NUM == 2)
GS32 lWearCap1 = 20000;
GS32 lUnwearCap1 = 16000;
GS32 lNoise1 = 200;
GS32 lWearCap2 = 3700;
GS32 lUnwearCap2 = 1200;
GS32 lNoise2 = 200;
#elif (WATCH_NUM == 3)
GS32 lWearCap1 = 42500;
GS32 lUnwearCap1 = 39500;
GS32 lNoise1 = 150;
GS32 lWearCap2 = 42500;
GS32 lUnwearCap2 = 39500;
GS32 lNoise2 = 150;
#endif


#if (__SUPPORT_HARD_ADT_CONFIG__)

/// movement status of g sensor
GU8 g_uchGsensorStatus = GH3X2X_SENSOR_IS_NOT_MOVING;

/// sum of square of g sensor x,y,z value
GU32 g_unGsDataSumOfSquare = 0;

/// sum of square threshold of g sensor data that can be judged as movement
GU32 g_unGsMoveThreshold = 0;

/// count how many times of movement detected by g sensor 
GU16 g_usGsMoveDetectCnt = 0;

/// count how many times of not movement detected by g sensor 
GU16 g_usGsNotMoveDetectCnt = 0;

/// count threshold of continuous g sensor movement
GU16 g_unGsMoveCntThreshold = 0;

/// count threshold of continuous g sensor not movement
GU16 g_unGsNotMoveCntThreshold = 0;

/// enable gs movement detect or not
GU8 g_uchAdtWithConfirmEnable = 0;

/// the first time of G sensor move detection
GU8 g_uchGsMoveDetectFirstTime = 0;

typedef struct
{
    GS16 sXAxisVal;     /**< x-axis value */
    GS16 sYAxisVal;     /**< y-axis value */
    GS16 sZAxisVal;     /**< z-axis value */
} STGsAccRawdata;

STGsAccRawdata g_LastPointAdtGsenorRawdata;


GU32 g_unLastAdtGsensorPeakPosi = 0;
GU32 g_unLastLastAdtGsensorPeakPosi = 0;
GU32 g_unCurrentAdtGsensorPeakPosi = 0;


extern void GH3X2X_RedetectWearOn(void);


/// function started bitmap, use for sampling control
GU8 g_uchDecByCapResult = 0;
#if 1
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
extern GU8 NADTWearDetectByCap(GS32 lPacketInfoArr[]);
void GH3X2X_MoveDetectByCapData(STCapRawdata* pusCapData, GU16 usCapDataCnt)
{
    if( 0 == GH3X2X_GetCapEnableFlag())
    {
        g_uchDecByCapResult = 1;
        return;
    }
    GS32 lPacketInfoArr[4];
    for(GU16 i = 0; i < usCapDataCnt;i++)
    {
        lPacketInfoArr[0] = pusCapData[i].unCapData[0];
        lPacketInfoArr[1] = pusCapData[i].unCapData[1];
        lPacketInfoArr[2] = pusCapData[i].unCapData[2];
        lPacketInfoArr[3] = pusCapData[i].unCapData[3];

        if(1 == NADTWearDetectByCap(lPacketInfoArr))
        {
            g_uchDecByCapResult = 1;
            return;
        }
    }
}
#else
void GH3X2X_MoveDetectByCapData(STCapRawdata* pusCapData, GU16 usCapDataCnt)
{
    g_uchDecByCapResult = 1;
    return;
}
#endif
#endif
/**
 * @fn      void GH3X2X_MoveDetectByGsData(STGsensorRawdata* stGsData, GU16 usGsDataCnt, GU8 uchCheckWindowSize)
 
 *
 * @brief  Move detection by software.
 *
 * @attention   None
 *
 * @param[in]   stGsData        gsensor data buffer
 * @param[in]   usGsDataCnt        gsensor data count
 * @param[in]   uchCheckWindowSize        the window size of cheak moving peak num 
 * @param[out]  None
 *
 * @return  None
 */
//extern void SlaverLog(const char * lpsbLog, ...);
void GH3X2X_MoveDetectByGsData(GS16* pusGsData, GU16 usGsDataCnt)
{
    GS16 sXAxisVal = 0;
    GS16 sYAxisVal = 0;
    GS16 sZAxisVal = 0;
    GU8  uchGyroEnable = g_uchGyroEnable;
    GU32 unSumOfSquare = 0;    
    GU16  usGsDataIndex = 0;
    if ((GH3X2X_PTR_NULL == pusGsData) || (0 == usGsDataCnt))
    {
        return;
    }
    
    if (g_uchAdtWithConfirmEnable)
    {
        if(g_uchGsMoveDetectFirstTime)
        {
            g_LastPointAdtGsenorRawdata.sXAxisVal = pusGsData[0];
            g_LastPointAdtGsenorRawdata.sYAxisVal = pusGsData[0+1];
            g_LastPointAdtGsenorRawdata.sZAxisVal = pusGsData[0+2];
            g_uchGsMoveDetectFirstTime = 0;
        }
        //calculate average of x, y, z
        for (usGsDataIndex = 0; usGsDataIndex < usGsDataCnt; usGsDataIndex++)
        {
            sXAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable)] - g_LastPointAdtGsenorRawdata.sXAxisVal;
            sYAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 1] - g_LastPointAdtGsenorRawdata.sYAxisVal;
            sZAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 2] - g_LastPointAdtGsenorRawdata.sZAxisVal;

            g_LastPointAdtGsenorRawdata.sXAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable)];
            g_LastPointAdtGsenorRawdata.sYAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 1];
            g_LastPointAdtGsenorRawdata.sZAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 2];


            unSumOfSquare = (GU32)((GS32)sXAxisVal * sXAxisVal) + (GU32)((GS32)sYAxisVal * sYAxisVal) + (GU32)((GS32)sZAxisVal * sZAxisVal);

            if(unSumOfSquare > g_unGsMoveThreshold)
            {
                g_usGsMoveDetectCnt ++;
                GH3X2X_DEMO_LOG_PARAM_ADT("g_usGsMoveDetectCnt = %d \r\n", g_usGsMoveDetectCnt);
                g_usGsNotMoveDetectCnt = 0;
                if (g_usGsMoveDetectCnt > g_unGsMoveCntThreshold && g_uchDecByCapResult == 1)
                {
                    GH3X2X_DEMO_LOG_PARAM_ADT("over g_unGsMoveCntThreshold!!! \r\n");
                    g_uchDecByCapResult = 0;
                    g_uchGsensorStatus = GH3X2X_SENSOR_IS_MOVING;
                    GH3X2X_DEMO_LOG_PARAM_ADT("Move detect effect!!! \r\n");
                    if (((GH3X2X_GetFuncStartedBitmap()) & (GH3X2X_FUNCTION_ADT)) != (GH3X2X_FUNCTION_ADT))
                    {
                        GH3X2X_DEMO_LOG_PARAM_ADT("start hard adt!!! \r\n");
                        GH3X2X_StartHardAdtAndResetGsDetect();
                    }
                    else  //adt sample is open
                    {
                        GH3X2X_RedetectWearOn();  //it can trigle a wear on event(INT) when it is detecting wear off
                    }
                    GH3X2X_EnterLowPowerMode();
                    
                }
            }
            else
            {
                if (g_usGsMoveDetectCnt != 0)
                {
                    g_usGsNotMoveDetectCnt ++;
                    GH3X2X_DEMO_LOG_PARAM_ADT("g_usGsNotMoveDetectCnt = %d \r\n", g_usGsNotMoveDetectCnt);
                    
                    if (g_usGsNotMoveDetectCnt >= g_unGsNotMoveCntThreshold)
                    {
                        g_usGsMoveDetectCnt = 0;
                        g_uchGsensorStatus = GH3X2X_SENSOR_IS_NOT_MOVING;
                        GH3X2X_DEMO_LOG_PARAM_ADT("Move detect is not effect!!! \r\n");
                    }
                }
            }
        }
        g_uchDecByCapResult = 0;
    }
}

/**
 * @fn     void Gh3x2x_ResetMoveDetectByGsData(void)
 *
 * @brief  Reset movement detection by gsensor data
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  1: enabled, 0:disabled
 */

void Gh3x2x_ResetMoveDetectByGsData(void)
{
    g_uchGsensorStatus  = GH3X2X_SENSOR_IS_NOT_MOVING;
    g_uchGsMoveDetectFirstTime = 1;
    g_uchAdtWithConfirmEnable  = 1;
    g_usGsMoveDetectCnt = 0;
    g_usGsNotMoveDetectCnt = 0;

    GH3X2X_DEMO_LOG_PARAM_ADT("Gh3x2x_ResetMoveDetectByGsData g_uchAdtWithConfirmEnable = %x\r\n",g_uchAdtWithConfirmEnable);
}
/**
 * @fn     GS8 GH3X2X_AdtFuncStartWithConfirm(void)
 *
 * @brief  Start adt function with move detect confirm
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AdtFuncStartWithGsDetect(void)
{
    if (GH3X2X_GetSoftWearOffDetEn())
    {
        GH3X2X_DEMO_LOG_PARAM_ADT("[%s]\r\n", __FUNCTION__);
        Gh3x2x_ResetMoveDetectByGsData();
        GH3X2X_AdtFuncStartWithGsDetectHook();
        if (((GH3X2X_GetFuncStartedBitmap()) & (GH3X2X_FUNCTION_ADT)) == (GH3X2X_FUNCTION_ADT))
        {
            GH3X2X_DEMO_LOG_PARAM_ADT("stop hard adt!!! \r\n");
            GH3X2X_StopHardAdtAndStartGsDetect();
            #if __HARD_ADT_ALGO_CHECK_EN__
            EXAMPLE_LOG("[%s]reset adt algo.\r\n", __FUNCTION__);
            Gh3x2xDemoStopAlgoInner(GH3X2X_FUNCTION_ADT);
            Gh3x2xDemoStartAlgoInner(GH3X2X_FUNCTION_ADT);
            #endif
            
        }
        GH3X2X_DEMO_LOG_PARAM_ADT("[GH3X2X_GetSoftWearOffDetEn()]:g_uchAdtWithConfirmEnable = %x\r\n",g_uchAdtWithConfirmEnable);
    }
}

/**
 * @fn     GS8 GH3X2X_AdtFuncStopWithConfirm(void)
 *
 * @brief  stop adt function with move detect confirm
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AdtFuncStopWithGsDetect(void)
{
    if (GH3X2X_GetSoftWearOffDetEn())
    {
        GH3X2X_DEMO_LOG_PARAM_ADT("[%s]\r\n", __FUNCTION__);
        Gh3x2x_ResetMoveDetectByGsData();
        g_uchAdtWithConfirmEnable = 0;
    }
}

/**
 * @fn     GU8 Gh3x2x_GetAdtConfirmStatus(void)
 *
 * @brief  Get status of movement detected
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  GH3X2X_SENSOR_IS_NOT_MOVING/GH3X2X_SENSOR_IS_MOVING
 */
GU8 Gh3x2x_GetAdtConfirmStatus(void)
{
    return g_uchGsensorStatus;
}


/**
 * @fn     void Gh3x2x_SetAdtConfirmPara(GU8 uchMoveThreshold, GU16 usMoveCntThreshold)
 *
 * @brief  Set some parameter of adt confirm
 *
 * @attention   None
 *
 * @param[in]   usMoveThreshold            threshold of gsensor data(sqrt(sum of square)) that can be judged as movement
 * @param[in]   usMoveCntThreshold        how many times of continuous overtake usMoveThreshold that can trig sampling
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_SetAdtConfirmPara(GU8 uchMoveThreshold, GU16 usMoveCntThreshold, GU16 usNotMoveCntThreshold)
{
    g_unGsMoveThreshold = uchMoveThreshold * uchMoveThreshold;
    g_unGsMoveCntThreshold = usMoveCntThreshold;
    g_unGsNotMoveCntThreshold = usNotMoveCntThreshold;
}

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/


GU8  Gh3x2xCheckSoftAdtTimeOut(const STGh3x2xFrameInfo * const pstFrameInfo, GU32 unSpecialAngleTimeSec, GU32 unMovelessTimeSec)
{
    return 0;
}

#endif

extern  void hal_gh3x2x_write_cap_to_flash(GS32 WearCap1,GS32 UnwearCap1,GS32 WearCap2,GS32 UnwearCap2);
extern  void hal_gh3x2x_read_cap_from_flash(GS32* WearCap1,GS32* UnwearCap1,GS32* WearCap2,GS32* UnwearCap2);

void GH3x2xWriteCapToFlash(void)
{
    hal_gh3x2x_write_cap_to_flash(lWearCap1,lUnwearCap1,lWearCap2,lUnwearCap2);
}

void GH3x2xReadCapFromFlash(void)
{
    hal_gh3x2x_read_cap_from_flash(&lWearCap1,&lUnwearCap1,&lWearCap2,&lUnwearCap2);
}

