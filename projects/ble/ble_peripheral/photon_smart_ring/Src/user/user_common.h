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

#define GUNTER_FAILURE           (0xFFFF)
#define GUNTER_SUCCESS           (0x0000)
#define GUNTER_ERR_NULL_POINTER  (0x0001)
#define GUNTER_ERR_INVALID_TYPE  (0x0002)
#define GUNTER_ERR_INVALID_ADDR  (0x0003)
#define GUNTER_ERR_INVALID_PARAM (0x0003)
#define GUNTER_ERR_NOT_INITIAL   (0x0004)
#define GUNTER_ERR_NOT_SUPPORT   (0x0005)

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

/**
 * @brief Use CRC-8(POLY = 0x8C) to calculate the checksum
 *
 * @param data Data that requires checksum calculation
 * @param length Datalength
 * @return checksum
 */
uint8_t get_checksum_crc8(uint8_t* data, size_t length);

/**
 * @brief Use CRC-16-CCITT(POLY = 0x1021) to calculate the checksum
 *
 * @param data Data that requires checksum calculation
 * @param length Datalength
 * @return checksum
 */
uint16_t get_checksum_crc16(uint8_t* data, size_t length);

/**
 * @brief Use CRC-32(POLY = 0x04C11DB7) to calculate the checksum
 *
 * @param data Data that requires checksum calculation
 * @param length Datalength
 * @return checksum
 */
uint32_t get_checksum_crc32(uint8_t* data, size_t length);

void     write_big_endian_2(uint8_t* buffer, uint16_t data);
uint16_t read_big_endian_2(const uint8_t* buffer);

void     write_big_endian_4(uint8_t* buffer, uint32_t data);
uint32_t read_big_endian_4(const uint8_t* buffer);

void     write_big_endian_8(uint8_t* buffer, uint64_t data);
uint64_t read_big_endian_8(const uint8_t* buffer);

void     write_little_endian_2(uint8_t* buffer, uint16_t data);
uint16_t read_little_endian_2(const uint8_t* buffer);

void     write_little_endian_4(uint8_t* buffer, uint32_t data);
uint32_t read_little_endian_4(const uint8_t* buffer);

void     write_little_endian_8(uint8_t* buffer, uint64_t data);
uint64_t read_little_endian_8(const uint8_t* buffer);

#ifdef __cplusplus
}
#endif

#endif
