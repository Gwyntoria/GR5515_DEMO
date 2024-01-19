/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_algo_config.h
 *
 * @brief   gh3x2x algo config
 *
 * @version ref gh3x2x_demo_algo_config.h
 *
 */
#ifndef _GH3X2X_DEMO_ALGO_CONFIG_H_
#define _GH3X2X_DEMO_ALGO_CONFIG_H_

/* algorithm config */
/**
 * @brief support multi algorithm run simultaneously type enum
 */
#define SUPPORT_NO_MULTI_ALGORITHM             (0)
#define SUPPORT_MULTI_ALGORITHM                (1)
#define SUPPORT_GROUP_MULTI_ALGORITHM          (2)


/* SUPPORT_NO_MULTI_ALGORITHM:    not support multi algorithm,
 * SUPPORT_MULTI_ALGORITHM:       support multi algorithm run simultaneously,
 * SUPPORT_GROUP_MULTI_ALGORITHM: support several multi algorithm */
#define __ALGO_RUN_SIMULTANEOUSLY_SUPPORT__    (SUPPORT_MULTI_ALGORITHM)   // SUPPORT_NO_MULTI_ALGORITHM / SUPPORT_MULTI_ALGORITHM / SUPPORT_GROUP_MULTI_ALGORITHM

#ifdef GOODIX_DEMO_PLANFORM
//#include "Goodix_Platform_config.h"
#endif

#define GH3X2X_ALGO_FUNCTION_GROUP_0 (GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_SOFT_ADT_GREEN)
#define GH3X2X_ALGO_FUNCTION_GROUP_1 (GH3X2X_FUNCTION_SPO2|GH3X2X_FUNCTION_SOFT_ADT_GREEN)
#define GH3X2X_ALGO_FUNCTION_GROUP_2 (GH3X2X_FUNCTION_SOFT_ADT_GREEN)
#define GH3X2X_ALGO_FUNCTION_GROUP_3 (GH3X2X_NO_FUNCTION)
#define GH3X2X_ALGO_FUNCTION_GROUP_4 (GH3X2X_NO_FUNCTION)



/* 0: global buffer for goodix algorithm mem pool
 * 1: user dynamic buffer for goodix algorithm mem pool */
#define __USER_DYNAMIC_ALGO_MEM_EN__           (0)
#if __USER_DYNAMIC_ALGO_MEM_EN__
#define GH3X2X_ALGORITHMS_MEMORY_SIZE_USERSET  (0)
#endif

/* function algo enable config */
#define __USE_GOODIX_ADT_ALGORITHM__           (0)
#define __USE_GOODIX_HR_ALGORITHM__            (1)
#define __USE_GOODIX_HRV_ALGORITHM__           (1)
#define __USE_GOODIX_HSM_ALGORITHM__           (0)
#define __USE_GOODIX_SPO2_ALGORITHM__          (1)
#define __USE_GOODIX_ECG_ALGORITHM__           (0)
#define __USE_GOODIX_BT_ALGORITHM__            (0)
#define __USE_GOODIX_RESP_ALGORITHM__          (0)
#define __USE_GOODIX_AF_ALGORITHM__            (0)
#define __USE_GOODIX_BP_ALGORITHM__            (0)
#define __USE_GOODIX_SOFT_ADT_ALGORITHM__      (1)

/* algorithm log */
#define __ALGO_LOG_CONFIG__                    (1)

/* algorithm config max number */
#define __GOODIX_ALGO_CFG_LIST_MAX_NUM__       (2)

/*algorithm output value process strategy config*/
#define __GH3X2X_HR_OUTPUT_VALUE_STRATEGY_EN__ (0)

/* algoritm channel nunber config*/
#define __HR_ALGORITHM_SUPPORT_CHNL_NUM__      (4)  /* range 1~4 */
#define __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__    (1)  /* range 1-4 */

/* algo and drv interactivity*/
#define __GH_MSG_WTIH_DRV_LAYER_EN__           (0)
#define __GH_SEPARATE_ALGO_EN__                (0)

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
#define __SOFT_ADT_IR_DETECT_TIMEOUT__         (30) /** (recomended value = 30 seconds) ir detect timeout **/
#endif

#define __GS_TIMESTAMP_READ_EN__               (0)
#if __GS_TIMESTAMP_READ_EN__
#define __SYNC_ACC_SAMPLERATE__                (25)
#define __SYNC_PPG_SAMPLERATE__                (25)
#define __SYNC_ACC_BUFFER_SECOND__             (2)
#define __SYNC_PPG_BUFFER_SECOND__             (1)
#define __SYNC_PPG_FRAME_NUM__                 (10)
#define __SYNC_ACC_FRAME_NUM__                 (20)
#define __SYNC_RAWDATA_MAX_NUM__               (3)
#define __SYNC_AGCINFO_MAX_NUM__               (3)
#define __SYNC_FLAG_MAX_NUM__                  (3)
#endif



#define GH_SEND_MSG_WEAR_EVENT(event)   //add your code to send message to drv layer


















/***********************************  DO NOT MODIFY FOLLOWING CODE *******************************/
#if (\
    __USE_GOODIX_HR_ALGORITHM__      ||\
    __USE_GOODIX_HRV_ALGORITHM__     ||\
    __USE_GOODIX_HSM_ALGORITHM__     ||\
    __USE_GOODIX_SPO2_ALGORITHM__    ||\
    __USE_GOODIX_ECG_ALGORITHM__     ||\
    __USE_GOODIX_BT_ALGORITHM__      ||\
    __USE_GOODIX_RESP_ALGORITHM__    ||\
    __USE_GOODIX_AF_ALGORITHM__      ||\
    __USE_GOODIX_BP_ALGORITHM__      ||\
    __USE_GOODIX_SOFT_ADT_ALGORITHM__\
    )
#define __GOODIX_ALGO_CALL_MODE__           (1)
#else
#define __GOODIX_ALGO_CALL_MODE__           (0)
#endif


#endif /* _GH3X2X_DEMO_ALGO_CALL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
