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

uint8_t get_checksum_crc8(uint8_t* data, size_t length) {
    const uint8_t kPoly = 0x8C;
    uint8_t       crc   = 0x00;
    while (length--) {
        crc ^= *data++;
        for (int i = 0; i < 8; i++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ kPoly; // 多项式：x^8 + x^5 + x^4 + 1
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

uint16_t get_checksum_crc16(uint8_t* data, size_t length) {
    const uint16_t kPoly = 0x1021;
    uint16_t       crc16 = 0x0000;

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

uint32_t get_checksum_crc32(uint8_t* data, size_t length) {
    const uint32_t kPoly = 0x04C11DB7;
    uint32_t       crc32 = 0xFFFFFFFF;

    for (size_t i = 0; i < length; ++i) {
        crc32 ^= ((uint32_t)data[i] << 24);
        for (size_t j = 0; j < 8; ++j) {
            if (crc32 & 0x80000000) {
                crc32 = (crc32 << 1) ^ kPoly;
            } else {
                crc32 <<= 1;
            }
        }
    }

    return crc32;
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

/**
 * 计算特定字节序列在给定缓冲区中出现的次数。
 * 
 * @param buffer 指向要搜索的缓冲区的指针。
 * @param buffer_size 缓冲区的大小（字节为单位）。
 * @param sequence 要查找的字节序列。
 * @param sequence_size 字节序列的大小。
 * @return 出现的次数。
 */
int count_sequence_in_buffer(const uint8_t* buffer, size_t buffer_size,
                             const uint8_t* sequence, size_t sequence_size) {
    int count = 0;
    if (sequence_size > buffer_size) {
        return GUNTER_ERR_INVALID_PARAM; // 序列比缓冲区还大，无法匹配
    }
    
    // 遍历缓冲区查找序列
    for (size_t i = 0; i <= buffer_size - sequence_size; i++) {
        // 比较当前位置开始的sequence_size长度的数据
        if (memcmp(buffer + i, sequence, sequence_size) == 0) {
            count++; // 发现匹配
            i += sequence_size - 1; // 移动索引到匹配序列的末尾的前一个位置，因为循环会自动加1
        }
    }
    
    return count;
}

/**
 * Converts a floating-point number to a fixed-point number within a specified range.
 *
 * @param input The input floating-point number to be converted.
 * @param min The minimum value of the input range.
 * @param max The maximum value of the input range.
 * @param scale The scaling factor to convert the input to a fixed-point number.
 * @return The converted fixed-point number.
 */
uint32_t float_to_fixed_point(float input, int min, int max, int scale) {
    // Check input range
    if (input < min || input > max) {
        APP_LOG_ERROR("Input value out of range: %f", input);
        return 0;
    }

    // Multiply the floating-point number by 100 and convert it to a fixed-point number
    uint32_t scaledInput = (uint32_t)((input - min) * scale);
    
    return scaledInput;
}

/**
 * Converts a fixed-point number to a floating-point number.
 *
 * @param input The fixed-point number to be converted.
 * @param min The minimum value of the converted floating-point number.
 * @param max The maximum value of the converted floating-point number.
 * @param scale The scaling factor to divide the fixed-point number by.
 * @return The converted floating-point number.
 */
float fixed_point_to_float(uint32_t input, int min, int max, int scale) {
    // Divide the fixed-point number by the scaling factor and add the minimum value
    float scaledInput = (float)input / scale + min;
    
    return scaledInput;
}
