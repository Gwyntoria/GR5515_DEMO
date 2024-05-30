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
#include "user_hrv.h"

#define CAL_TIME_SWITCH  (1)
#define CAL_TIME_START() g_report_func_start_time = rtc_get_relative_ms();
#define CAL_TIME_END()                                 \
    g_report_func_end_time = rtc_get_relative_ms(); \
    g_report_func_time     = g_report_func_end_time - g_report_func_start_time;

#define CAL_TIME_PRINT(type) GH3X2X_SAMPLE_ALGO_LOG_PARAM("Report %s time: %d ms\n", type, g_report_func_time);

extern GU16 g_low_confidence_cnt_hr;
extern GU16 g_low_confidence_cnt_spo2;
extern GU16 g_low_confidence_cnt_hrv;
extern GU16 g_low_confidence_cnt_adt;
extern GU16 g_high_confidence_cnt_adt;
extern GU16 g_data_cnt_hr;
extern GU16 g_data_cnt_spo2;
extern GU16 g_data_cnt_hrv;

uint64_t g_report_func_start_time = 0;
uint64_t g_report_func_end_time = 0;
uint64_t g_report_func_time = 0;

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
#if CAL_TIME_SWITCH
    CAL_TIME_START();
#endif

#if (__USE_GOODIX_HR_ALGORITHM__)
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:hr = %dbpm, confidence = %d\r\n",
                                 __FUNCTION__,
                                 pstAlgoResult->snResult[0],
                                 pstAlgoResult->snResult[1]);

    uint8_t data = (uint8_t)pstAlgoResult->snResult[0];
    health_hr_data_send(0, &data, 1);


    if (pstAlgoResult->snResult[1] < CONFIDENCE_THRESHOLD_HR) {
        g_low_confidence_cnt_hr++;
    } else {
        g_low_confidence_cnt_hr = 0;

        if (g_data_cnt_hr < DATA_CNT_THRESHOLD_HR) {
            DataCenterS2f* data_center_s2f = get_data_center_s2f();
            if (data_center_s2f->recv_sensor_func) {
                int ret = data_center_s2f->recv_sensor_func(data_center_s2f, kDataTypeHr, (uint16_t)data);
                if (ret != GUNTER_SUCCESS) {
                    GH3X2X_SAMPLE_ALGO_LOG_PARAM("DataCenterS2f recv_sensor_func error: %d\n", ret);
                }
            }
        }

        g_data_cnt_hr++;
    }

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("g_low_confidence_cnt_hr: %d\n", g_low_confidence_cnt_hr);

    if (g_data_cnt_hr > DATA_CNT_THRESHOLD_HR * 3) {
        func_ctrl_set_switch_func(kFuncSwitchOff);
        func_ctrl_set_switch_hr(kFuncSwitchOff);
    }

    // extern GU32 g_unDemoFuncMode;
    // if ((g_unDemoFuncMode & GH3X2X_FUNCTION_SOFT_ADT_GREEN) != GH3X2X_FUNCTION_SOFT_ADT_GREEN) {
    //     hrs_heart_rate_measurement_send(0, pstAlgoResult->snResult[0], 1);
    // }

#endif

#if CAL_TIME_SWITCH
    CAL_TIME_END();
    CAL_TIME_PRINT("HR");
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
#if CAL_TIME_SWITCH
    CAL_TIME_START();
#endif

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
        g_low_confidence_cnt_spo2++;
    } else {
        g_low_confidence_cnt_spo2 = 0;

        if (g_data_cnt_spo2 < DATA_CNT_THRESHOLD_SPO2) {
            DataCenterS2f* data_center_s2f = get_data_center_s2f();
            if (data_center_s2f->recv_sensor_func) {
                int ret = data_center_s2f->recv_sensor_func(data_center_s2f, kDataTypeSpo2, (uint16_t)data);
                if (ret != GUNTER_SUCCESS) {
                    GH3X2X_SAMPLE_ALGO_LOG_PARAM("DataCenterS2f recv_sensor_func error: %d\n", ret);
                }
            }
        }

        g_data_cnt_spo2++;
    }

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("g_low_confidence_cnt_spo2: %d\n", g_low_confidence_cnt_spo2);

    if (g_data_cnt_spo2 > DATA_CNT_THRESHOLD_SPO2 * 3) {
        func_ctrl_set_switch_func(kFuncSwitchOff);
        func_ctrl_set_switch_spo2(kFuncSwitchOff);
    }

#endif

#if CAL_TIME_SWITCH
    CAL_TIME_END();
    CAL_TIME_PRINT("SPO2");
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
#if CAL_TIME_SWITCH
    CAL_TIME_START();
#endif

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

    // uint8_t data = (uint8_t)pstAlgoResult->snResult[0];
    // health_hrv_data_send(0, &data, 1);

    if (pstAlgoResult->snResult[4] < CONFIDENCE_THRESHOLD_HRV) {
        g_low_confidence_cnt_hrv++;
    } else {
        g_low_confidence_cnt_hrv = 0;

        for (int i = 0; i < pstAlgoResult->snResult[5]; i++) {
            if (i >= 4) {
                break;
            }

            user_hrv_add_data((int)pstAlgoResult->snResult[i]);
        }

        g_data_cnt_hrv++;

        // printf("HRV data count: %d\n", g_data_cnt_hrv);
    }

    if (g_data_cnt_hrv >= DATA_CNT_THRESHOLD_RRI) {
        func_ctrl_set_switch_func(kFuncSwitchOff);
        func_ctrl_set_switch_hrv(kFuncSwitchOff);
    }

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("g_low_confidence_cnt_hrv: %d\n", g_low_confidence_cnt_hrv);
#endif

#if CAL_TIME_SWITCH
    CAL_TIME_END();
    CAL_TIME_PRINT("HRV");
#endif
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
#if CAL_TIME_SWITCH
    CAL_TIME_START();
#endif

#if (__USE_GOODIX_ECG_ALGORITHM__)
    /* code implement by user */
    //GOODIX_PLANFROM_ECG_RESULT_REPORT_ENTITY();
#endif

#if CAL_TIME_SWITCH
    CAL_TIME_END();
    CAL_TIME_PRINT("ECG");
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
#if CAL_TIME_SWITCH
    CAL_TIME_START();
#endif

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("[%s]:result: %d, confidence: %d\r\n", 
                                 __FUNCTION__,
                                 pstAlgoResult->snResult[0],
                                 pstAlgoResult->snResult[1]);

    if (pstAlgoResult->snResult[1] < CONFIDENCE_THRESHOLD_NADT_GREEN) {
        g_low_confidence_cnt_adt++;
        // g_high_confidence_cnt_adt = 0;
    } else {
        // g_low_confidence_cnt_adt = 0;
        g_high_confidence_cnt_adt++;
    }

    GH3X2X_SAMPLE_ALGO_LOG_PARAM("g_low_confidence_cnt_adt: %d\n", g_low_confidence_cnt_adt);
    GH3X2X_SAMPLE_ALGO_LOG_PARAM("g_high_confidence_cnt_adt: %d\n", g_high_confidence_cnt_adt);

    // live object
    if ((pstAlgoResult->snResult[0] == 0x1) || (g_high_confidence_cnt_adt > (20 * 5))) {
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
    else if ((pstAlgoResult->snResult[0] & 0x2) || (g_low_confidence_cnt_adt > (20 * 20))) {
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

#if CAL_TIME_SWITCH
    CAL_TIME_END();
    CAL_TIME_PRINT("S_ADT_GREEN");
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
#if CAL_TIME_SWITCH
    CAL_TIME_START();
#endif

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

#if CAL_TIME_SWITCH
    CAL_TIME_END();
    CAL_TIME_PRINT("S_ADT_IR");
#endif
}

#endif // end else #if (__DRIVER_LIB_MODE__==__DRV_LIB_WITHOUT_ALGO__)

