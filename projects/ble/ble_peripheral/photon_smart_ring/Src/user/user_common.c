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
