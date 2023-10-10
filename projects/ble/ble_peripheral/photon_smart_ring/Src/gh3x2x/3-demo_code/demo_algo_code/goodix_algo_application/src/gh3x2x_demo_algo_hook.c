#include <stdio.h>
#include "gh3x2x_demo_common.h"
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_algo_hook.h"

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
    GOODIX_PLANFROM_LOG_ENTITY();
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
    /* code implement by user */
    //GOODIX_PLANFROM_HR_RESULT_REPORT_ENTITY();
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

    /* code implement by user */
    //GOODIX_PLANFROM_SPO2_RESULT_REPORT_ENTITY();
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
    //SLAVER_LOG("snHrvOut0=%d,snHrvOut1=%d,snHrvOut2=%d,snHrvOut3=%d,snHrvNum=%d,snHrvConfi=%d\r\n",
    //          stHrvAlgoRes[0].snHrvOut0,stHrvAlgoRes[0].snHrvOut1,stHrvAlgoRes[0].snHrvOut2,stHrvAlgoRes[0].snHrvOut3,stHrvAlgoRes[0].snHrvNum,stHrvAlgoRes[0].snHrvNum);
    //GOODIX_PLANFROM_HRV_RESULT_REPORT_ENTITY();
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
void GH3X2X_SoftAdtGreenAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    GH3X2X_ALGO_LOG_PARAM("[%s]:result = %d,%d\r\n", __FUNCTION__, pstAlgoResult->snResult[0], pstAlgoResult->snResult[1]);
    //live object
    if (pstAlgoResult->snResult[0] == 0x1)
    {
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

