/**
 * @file user_common.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief General function modules
 * @version 0.1
 * @date 2023-10-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "user_common.h"

#include "app_log.h"

#define BP_OFFSET 9
#define BP_GRAPH  60
#define BP_LEN    80

void data_stream_hex(const uint8_t* data, unsigned long len)
{
    char              line[BP_LEN];
    unsigned long     i;
    static const char hexdig[] = "0123456789abcdef";

    if (!data)
        return;

    /* in case len is zero */
    line[0] = '\0';

    for (i = 0; i < len; i++) {
        int      n = i % 16;
        unsigned off;

        if (!n) {
            if (i)
                printf("%s\n", line);
            memset(line, ' ', sizeof(line) - 2);
            line[sizeof(line) - 2] = '\0';

            off = i % 0x0ffffU;

            line[2] = hexdig[0x0f & (off >> 12)];
            line[3] = hexdig[0x0f & (off >> 8)];
            line[4] = hexdig[0x0f & (off >> 4)];
            line[5] = hexdig[0x0f & off];
            line[6] = ':';
        }

        off           = BP_OFFSET + n * 3 + ((n >= 8) ? 1 : 0);
        line[off]     = hexdig[0x0f & (data[i] >> 4)];
        line[off + 1] = hexdig[0x0f & data[i]];

        off = BP_GRAPH + n + ((n >= 8) ? 1 : 0);

        if (isprint(data[i])) {
            line[BP_GRAPH + n] = data[i];
        } else {
            line[BP_GRAPH + n] = '.';
        }
    }

    printf("%s\n", line);
}

uint16_t get_checksum_crc16(uint8_t* data, size_t length) {
    const uint16_t kPoly = 0x1021;
    uint16_t       crc16 = 0;

    for (size_t i = 0; i < length; ++i) {
        crc16 ^= ((uint16_t)data[i] << 8);
        for (size_t j = 0; j < 8; ++j) {
            if (crc16 & 0x8000) {
                crc16 = (crc16 << 1) ^ kPoly;
            } else {
                crc16 <<= 1;
            }
        }
    }

    return crc16;
}

// 写入2字节数据到缓冲区
void write_big_endian_2(uint8_t *buffer, uint16_t data) {
    buffer[0] = (data >> 8) & 0xFF; // 高字节
    buffer[1] = data & 0xFF;        // 低字节
}

// 从缓冲区读取2字节数据
uint16_t read_big_endian_2(const uint8_t *buffer) {
    return (buffer[0] << 8) | buffer[1];
}

// 写入4字节数据到缓冲区
void write_big_endian_4(uint8_t *buffer, uint32_t data) {
    buffer[0] = (data >> 24) & 0xFF; // 最高字节
    buffer[1] = (data >> 16) & 0xFF; // 次高字节
    buffer[2] = (data >> 8) & 0xFF;  // 次低字节
    buffer[3] = data & 0xFF;         // 最低字节
}

// 从缓冲区读取4字节数据
uint32_t read_big_endian_4(const uint8_t *buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

// 写入8字节数据到缓冲区
void write_big_endian_8(uint8_t *buffer, uint64_t data) {
    buffer[0] = (data >> 56) & 0xFF; // 最高字节
    buffer[1] = (data >> 48) & 0xFF; // 次高字节
    buffer[2] = (data >> 40) & 0xFF; // ...
    buffer[3] = (data >> 32) & 0xFF;
    buffer[4] = (data >> 24) & 0xFF;
    buffer[5] = (data >> 16) & 0xFF;
    buffer[6] = (data >> 8) & 0xFF;
    buffer[7] = data & 0xFF;         // 最低字节
}

// 从缓冲区读取8字节数据
uint64_t read_big_endian_8(const uint8_t *buffer) {
    return ((uint64_t)buffer[0] << 56) | ((uint64_t)buffer[1] << 48) | ((uint64_t)buffer[2] << 40) | ((uint64_t)buffer[3] << 32) | ((uint64_t)buffer[4] << 24) | ((uint64_t)buffer[5] << 16) | ((uint64_t)buffer[6] << 8) | (uint64_t)buffer[7];
}


// 写入2字节数据到缓冲区
void write_little_endian_2(uint8_t *buffer, uint16_t data) {
    buffer[0] = data & 0xFF;        // 低字节
    buffer[1] = (data >> 8) & 0xFF; // 高字节
}

// 从缓冲区读取2字节数据
uint16_t read_little_endian_2(const uint8_t *buffer) {
    return buffer[0] | (buffer[1] << 8);
}

// 写入4字节数据到缓冲区
void write_little_endian_4(uint8_t *buffer, uint32_t data) {
    buffer[0] = data & 0xFF;         // 最低字节
    buffer[1] = (data >> 8) & 0xFF;  // 次低字节
    buffer[2] = (data >> 16) & 0xFF; // 次高字节
    buffer[3] = (data >> 24) & 0xFF; // 最高字节
}

// 从缓冲区读取4字节数据
uint32_t read_little_endian_4(const uint8_t *buffer) {
    return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
}

// 写入8字节数据到缓冲区
void write_little_endian_8(uint8_t *buffer, uint64_t data) {
    buffer[0] = data & 0xFF;         // 最低字节
    buffer[1] = (data >> 8) & 0xFF;  // ...
    buffer[2] = (data >> 16) & 0xFF;
    buffer[3] = (data >> 24) & 0xFF;
    buffer[4] = (data >> 32) & 0xFF;
    buffer[5] = (data >> 40) & 0xFF;
    buffer[6] = (data >> 48) & 0xFF;
    buffer[7] = (data >> 56) & 0xFF; // 最高字节
}

// 从缓冲区读取8字节数据
uint64_t read_little_endian_8(const uint8_t *buffer) {
    return (uint64_t)buffer[0] | ((uint64_t)buffer[1] << 8) | ((uint64_t)buffer[2] << 16) | ((uint64_t)buffer[3] << 24) | ((uint64_t)buffer[4] << 32) | ((uint64_t)buffer[5] << 40) | ((uint64_t)buffer[6] << 48) | ((uint64_t)buffer[7] << 56);
}

