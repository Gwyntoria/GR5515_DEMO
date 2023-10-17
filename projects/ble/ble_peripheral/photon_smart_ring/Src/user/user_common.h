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

#ifdef __cplusplus
extern "C" {
#endif

// TIME
#define SEC_TO_MS (1000)
#define MIN_TO_MS (1000 * 60)
#define HOR_TO_MS (1000 * 60 * 60)

#define MIN_TO_SEC (60)
#define HOR_TO_SEC (60 * 60)
#define DAY_TO_SEC (60 * 60 * 24)

void print_data_stream_hex(const uint8_t* data, unsigned long len);

#ifdef __cplusplus
}
#endif

#endif