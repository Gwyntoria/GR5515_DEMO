#include <stdio.h>
#include "gh3x2x_demo_common.h"
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_algo_hook.h"

#include "gh3x2x_demo.h"

#include "hrs.h"
#include "health.h"

#include"user_func_ctrl.h"
#if (__GOODIX_ALGO_CALL_MODE__)

/**
 * @fn     void GH3X2X_AlgoLog(char *log_string)
 * 
 * @brief  for debug version, log
 *
 * @attention   this function must define that use debug version lib
 *
 * @param[in]   log_string      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */

void GH3X2X_AlgoLog(GCHAR *log_string)
{
    printf("%s", log_string);
}

/**
 * @fn     void GH3X2X_AdtAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hb algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHbAlgoRes             hb algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AdtAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_ADT_ALGORITHM__)
    if (pstAlgoResult->uchUpdateFlag)
    {
        if (pstAlgoResult->snResult[0] == 1)
        {
            #if __GH_MSG_WTIH_DRV_LAYER_EN__
                GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_ON);
            #else
                GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_ON);
            #endif
            /* code implement by user */
        }
        else if (pstAlgoResult->snResult[0] == 2)
        {
            #if __GH_MSG_WTIH_DRV_LAYER_EN__
                GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_OFF);
            #else
                GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
            #endif
            /* code implement by user */
        }
    }
#endif
}

/**
 * @fn     void GH3X2X_HrAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hb algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHbAlgoRes             hb algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_HrAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_HR_ALGORITHM__)
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:%dbpm %d %d\r\n", 
                      __FUNCTION__, pstAlgoResult->snResult[0],
                                    pstAlgoResult->snResult[1],
                                    pstAlgoResult->snResult[2]);

    uint8_t data = pstAlgoResult->snResult[0];
    health_hr_data_send(0, &data, 1);

    static GU16 low_confidence_cnt_hr = 0;

    if (pstAlgoResult->snResult[1] < CONFIDENCE_THRESHOLD) {
        low_confidence_cnt_hr++;
    } else {
        low_confidence_cnt_hr = 0;
    }

    printf("low_confidence_cnt_hr: %d\n", low_confidence_cnt_hr);


    if (low_confidence_cnt_hr > (5)) {
        func_ctrl_set_wearing_status(kWearingStatusOff);
        low_confidence_cnt_hr = 0;
    }

    // extern GU32 g_unDemoFuncMode;
    // if ((g_unDemoFuncMode & GH3X2X_FUNCTION_SOFT_ADT_GREEN) != GH3X2X_FUNCTION_SOFT_ADT_GREEN) {
    //     hrs_heart_rate_measurement_send(0, pstAlgoResult->snResult[0], 1);
    // }

#endif
}

/**
 * @fn     void GH3X2X_Spo2AlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult)
 *
 *
 * @brief  This function will be called after calculate spo2 algorithm.
 *
 * @attention   None
 *
 * @param[in]   stSpo2AlgoRes           spo2 algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_Spo2AlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SPO2_ALGORITHM__)
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:%d%% R=%d %d %d %d %d\r\n", 
                      __FUNCTION__, pstAlgoResult->snResult[0],
                                    pstAlgoResult->snResult[1],
                                    pstAlgoResult->snResult[2],
                                    pstAlgoResult->snResult[3],
                                    pstAlgoResult->snResult[4],
                                    pstAlgoResult->snResult[5]);
            
    uint8_t data = (uint8_t)pstAlgoResult->snResult[0];
    health_spo2_data_send(0, &data, 1);

    static GU16 low_confidence_cnt_spo2 = 0;

    if (pstAlgoResult->snResult[2] < CONFIDENCE_THRESHOLD) {
        low_confidence_cnt_spo2++;
    } else {
        low_confidence_cnt_spo2 = 0;
    }

    printf("low_confidence_cnt_spo2: %d\n", low_confidence_cnt_spo2);


    if (low_confidence_cnt_spo2 > (5)) {
        func_ctrl_set_wearing_status(kWearingStatusOff);
        low_confidence_cnt_spo2 = 0;
    }
#endif
}

/**
 * @fn     void GH3X2X_HrvAlgorithmResultReport(STHrvAlgoResult stHrvAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hrv algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHrvAlgoRes            hrv algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_HrvAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_HRV_ALGORITHM__)
    /* code implement by user */
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:hrv_num = %d, RRI0 = %d, RRI1 = %d, RRI2 = %d, RRI3 = %d, Confidence = %d\r\n",
                                 __FUNCTION__, 
                                 pstAlgoResult->snResult[5], 
                                 pstAlgoResult->snResult[0],
                                 pstAlgoResult->snResult[1], 
                                 pstAlgoResult->snResult[2], 
                                 pstAlgoResult->snResult[3],
                                 pstAlgoResult->snResult[4]);
#endif

    uint8_t data = (uint8_t)pstAlgoResult->snResult[0];
    health_hrv_data_send(0, &data, 1);

    static GU16 low_confidence_cnt_hrv = 0;

    if (pstAlgoResult->snResult[4] < CONFIDENCE_THRESHOLD) {
        low_confidence_cnt_hrv++;
    } else {
        low_confidence_cnt_hrv = 0;
    }

    printf("low_confidence_cnt_hrv: %d\n", low_confidence_cnt_hrv);


    if (low_confidence_cnt_hrv > (5)) {
        func_ctrl_set_wearing_status(kWearingStatusOff);
        low_confidence_cnt_hrv = 0;
    }
}

/**
 * @fn     void GH3X2X_EcgAlgorithmResultReport(STEcgAlgoResult stEcgAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_EcgAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_ECG_ALGORITHM__)
    /* code implement by user */
    //GOODIX_PLANFROM_ECG_RESULT_REPORT_ENTITY();
#endif
}

/**
 * @fn     void GH3X2X_SoftAdtGreenAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SoftAdtGreenAlgorithmResultReport(STGh3x2xAlgoResult* pstAlgoResult, GU32 lubFrameId) {
#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:result: %d, confidence: %d\r\n", 
                                 __FUNCTION__,
                                 pstAlgoResult->snResult[0],
                                 pstAlgoResult->snResult[1]);

    static GU16 lower_lvl_cnt = 0;
    static GU16 higher_lvl_cnt = 0;

    if (pstAlgoResult->snResult[1] < CONFIDENCE_THRESHOLD_NADT_GREEN) {
        lower_lvl_cnt++;
        higher_lvl_cnt = 0;
    } else {
        lower_lvl_cnt = 0;
        higher_lvl_cnt++;
    }

    printf("lower_lvl_cnt: %d\n", lower_lvl_cnt);
    printf("higher_lvl_cnt: %d\n", higher_lvl_cnt);

    // live object
    if (pstAlgoResult->snResult[0] == 0x1 ||
        ((pstAlgoResult->snResult[1] >= CONFIDENCE_THRESHOLD_NADT_GREEN) && higher_lvl_cnt > (20 * 2))) {
        // extern GU32 g_unDemoFuncMode;
        // if((g_unDemoFuncMode & GH3X2X_FUNCTION_HR) != GH3X2X_FUNCTION_HR)
        // {
        //     Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
        // }

        func_ctrl_set_wearing_status(kWearingStatusOn);
        func_ctrl_set_adt_switch(kFuncSwitchOff);
        lower_lvl_cnt = 0;
        higher_lvl_cnt = 0;
    }
    // non live object
    // TODO: Further improvement is needed
    else if ((pstAlgoResult->snResult[0] & 0x2) || (lower_lvl_cnt > (20 * 15))) {
#if __GH_MSG_WTIH_DRV_LAYER_EN__
        GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_OFF);
#else
        GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
#endif
        /* code implement by user */

        func_ctrl_set_wearing_status(kWearingStatusOff);
        func_ctrl_set_adt_switch(kFuncSwitchOff);
        lower_lvl_cnt = 0;
        higher_lvl_cnt = 0;
    }
    GOODIX_PLANFROM_NADT_RESULT_HANDLE_ENTITY();
#endif
}

/**
 * @fn     void GH3X2X_SoftAdtIrAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SoftAdtIrAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    GH3X2X_ALGO_LOG_PARAM("[%s]:result = %d,%d\r\n", __FUNCTION__, pstAlgoResult->snResult[0], pstAlgoResult->snResult[1]);
    //live object
    if (pstAlgoResult->snResult[0] == 0x1)
    {
        //Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SOFT_ADT_IR);
        //Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_SOFT_ADT_GREEN|GH3X2X_FUNCTION_HR);
        /* code implement by user */
    }
    //non live object
    else if (pstAlgoResult->snResult[0] & 0x2)
    {
        #if __GH_MSG_WTIH_DRV_LAYER_EN__
          GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_OFF);
        #else
          GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
        #endif
        /* code implement by user */
    }
    GOODIX_PLANFROM_NADT_RESULT_HANDLE_ENTITY();
#endif
}

#endif // end else #if (__DRIVER_LIB_MODE__==__DRV_LIB_WITHOUT_ALGO__)

