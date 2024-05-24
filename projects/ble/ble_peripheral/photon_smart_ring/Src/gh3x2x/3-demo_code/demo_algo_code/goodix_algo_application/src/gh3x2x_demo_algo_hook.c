#include <stdio.h>
#include "gh3x2x_demo_common.h"
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_algo_hook.h"

#include "gh3x2x_demo.h"
#include "app_log.h"

#include "user_common.h"
#include "hrs.h"
#include "health.h"
#include "user_func_ctrl.h"
#include "user_rtc.h"
#include "user_data_center.h"

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

GU16 low_confidence_cnt_hr = 0;

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
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:hr = %dbpm, confidence = %d\r\n",
                                 __FUNCTION__,
                                 pstAlgoResult->snResult[0],
                                 pstAlgoResult->snResult[1]);

    uint8_t data = (uint8_t)pstAlgoResult->snResult[0];
    health_hr_data_send(0, &data, 1);


    if (pstAlgoResult->snResult[1] < CONFIDENCE_THRESHOLD_HR) {
        low_confidence_cnt_hr++;
    } else {
        low_confidence_cnt_hr = 0;

        DataCenterS2f* data_center_s2f = get_data_center_s2f();
        if (data_center_s2f->recv_sensor_func) {
            int ret = data_center_s2f->recv_sensor_func(data_center_s2f, kDataTypeHr, (uint16_t)data);
            if (ret != GUNTER_SUCCESS) {
                GH3X2X_SAMPLE_ALGO_LOG_PARAM("DataCenterS2f recv_sensor_func error: %d\n", ret);
            }
        }
    }

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("low_confidence_cnt_hr: %d\n", low_confidence_cnt_hr);

    // extern GU32 g_unDemoFuncMode;
    // if ((g_unDemoFuncMode & GH3X2X_FUNCTION_SOFT_ADT_GREEN) != GH3X2X_FUNCTION_SOFT_ADT_GREEN) {
    //     hrs_heart_rate_measurement_send(0, pstAlgoResult->snResult[0], 1);
    // }

#endif
}

GU16 low_confidence_cnt_spo2 = 0;

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
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]: spo2 = %d%%, R = %d, confidence = %d, con_lvl = %d\r\n",
                                 __FUNCTION__, 
                                 pstAlgoResult->snResult[0],
                                 pstAlgoResult->snResult[1],
                                 pstAlgoResult->snResult[2],
                                 pstAlgoResult->snResult[3]);

    uint8_t data = (uint8_t)pstAlgoResult->snResult[0];
    health_spo2_data_send(0, &data, 1);


    if (pstAlgoResult->snResult[3] < CONFIDENCE_THRESHOLD_SPO2) {
        low_confidence_cnt_spo2++;
    } else {
        low_confidence_cnt_spo2 = 0;

        DataCenterS2f* data_center_s2f = get_data_center_s2f();
        if (data_center_s2f->recv_sensor_func) {
            int ret = data_center_s2f->recv_sensor_func(data_center_s2f, kDataTypeSpo2, (uint16_t)data);
            if (ret != GUNTER_SUCCESS) {
                GH3X2X_SAMPLE_ALGO_LOG_PARAM("DataCenterS2f recv_sensor_func error: %d\n", ret);
            }
        }
    }

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("low_confidence_cnt_spo2: %d\n", low_confidence_cnt_spo2);

#endif
}

GU16 low_confidence_cnt_hrv = 0;

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
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:hrv_num = %d, RRI0 = %d, RRI1 = %d, RRI2 = %d, RRI3 = %d, confidence = %d\r\n",
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

    if (pstAlgoResult->snResult[4] < CONFIDENCE_THRESHOLD_HRV) {
        low_confidence_cnt_hrv++;
    } else {
        low_confidence_cnt_hrv = 0;
    }

    // TODO: send hrv data to data center

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("low_confidence_cnt_hrv: %d\n", low_confidence_cnt_hrv);
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

GU16 lower_lvl_cnt_adt  = 0;
GU16 higher_lvl_cnt_adt = 0;

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

    if (pstAlgoResult->snResult[1] < CONFIDENCE_THRESHOLD_NADT_GREEN) {
        lower_lvl_cnt_adt++;
        // higher_lvl_cnt_adt = 0;
    } else {
        // lower_lvl_cnt_adt = 0;
        higher_lvl_cnt_adt++;
    }

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("lower_lvl_cnt_adt: %d\n", lower_lvl_cnt_adt);
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("higher_lvl_cnt_adt: %d\n", higher_lvl_cnt_adt);

    // live object
    if ((pstAlgoResult->snResult[0] == 0x1) || (higher_lvl_cnt_adt > (20 * 5))) {
        // extern GU32 g_unDemoFuncMode;
        // if((g_unDemoFuncMode & GH3X2X_FUNCTION_HR) != GH3X2X_FUNCTION_HR)
        // {
        //     Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR);
        // }

        GH3X2X_SAMPLE_ALGO_LOG_PARAM("Wear on\r\n");

        func_ctrl_set_result_adt(kFuncResultOn);
        func_ctrl_set_switch_func(kFuncSwitchOff);
        func_ctrl_set_switch_adt(kFuncSwitchOff);
        wear_off_cnt_reset();
    }
    // non live object
    // TODO: Further improvement is needed
    else if ((pstAlgoResult->snResult[0] & 0x2) || (lower_lvl_cnt_adt > (20 * 20))) {
#if __GH_MSG_WTIH_DRV_LAYER_EN__
        GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_OFF);
#else
        GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
#endif
        /* code implement by user */

        GH3X2X_SAMPLE_ALGO_LOG_PARAM("Wear off\r\n");

        func_ctrl_set_result_adt(kFuncResultOff);
        func_ctrl_set_switch_func(kFuncSwitchOff);
        func_ctrl_set_switch_adt(kFuncSwitchOff);
        wear_off_cnt_condition_increase();
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

