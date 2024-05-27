#ifndef USER_DFU_H
#define USER_DFU_H

#include <stdint.h>
#include "gr55xx_dfu.h"

#define FW_IMG_PATTERN                 0x4744
#define FW_MAX_IMG_CNT                 10
#define FW_MAX_COMMENTS_CNT            12

#define FW_FIRST_BOOT_INFO_ADDR        (0x01000000UL)                    //boot info size 32 Bytes
#define FW_IMG_INFO_ADDR               (FW_FIRST_BOOT_INFO_ADDR + 0x40)  //400 Bytes
#define FW_IMG_INFO_SIZE               400
#define FW_IMG_SIZE                     40

typedef struct {
    uint16_t    pattern;
    uint16_t    version;
    boot_info_t boot_info;
    uint8_t     comments[FW_MAX_COMMENTS_CNT];
} fw_img_info_t;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void dfu_enter(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif