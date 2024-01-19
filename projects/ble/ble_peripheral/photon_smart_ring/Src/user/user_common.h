/**
 * @file user_common.h
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief General function modules
 * @version 0.1
 * @date 2023-10-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef USER_COMMON_H
#define USER_COMMON_H

#include "stdint.h"

#include "app_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GUNTER_SUCCESS (0)
#define GUNTER_FAILURE (1)

// TIME
#define SEC_TO_MS (1000)
#define MIN_TO_MS (1000 * 60)
#define HOR_TO_MS (1000 * 60 * 60)

#define MIN_TO_SEC (60)
#define HOR_TO_SEC (60 * 60)
#define DAY_TO_SEC (60 * 60 * 24)

/**
 * @brief Print data in the specified format. 
 *        Print byte data in hexadecimal format, outputting sixteen bytes per line
 *
 * @param data Data that needs to be printed
 * @param len Length of the data that needs to be printed
 */
void data_stream_hex(const uint8_t* data, unsigned long len);

#ifdef __cplusplus
}
#endif

#endif
