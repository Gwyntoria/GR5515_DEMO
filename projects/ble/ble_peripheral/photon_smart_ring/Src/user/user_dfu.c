#include "user_dfu.h"

extern bool get_system_config_info(uint32_t address, uint8_t* data, uint16_t length);
extern bool check_boot_info(volatile boot_info_t* p_boot);
extern bool check_image_crc(const uint8_t * p_data,uint32_t len, uint32_t check);


/**
 * @brief Retrieves the firmware image information from the specified address.
 *
 * This function reads the firmware image information from the given address and checks if it matches the expected pattern and comments.
 *
 * @param fw_img_info Pointer to the fw_img_info_t structure where the firmware image information will be stored.
 * @return true if the firmware image information matches the expected pattern and comments, false otherwise.
 */
bool _get_dfu_img_info(fw_img_info_t* fw_img_info) {
    uint8_t  i;
    uint32_t read_addr = FW_IMG_INFO_ADDR;

    for (i = 0; i < FW_MAX_IMG_CNT; i++) {
        get_system_config_info(read_addr, (uint8_t*)fw_img_info, FW_IMG_SIZE);
        if (fw_img_info->pattern == FW_IMG_PATTERN) {
            if (memcmp(&fw_img_info->comments, "ble_dfu_boot", FW_MAX_COMMENTS_CNT) == 0) {
                return true;
            }
        }
        read_addr += FW_IMG_SIZE;
    }

    return false;
}

void _run_dfu_boot(void) {
    fw_img_info_t dfu_image_info;

    if (!_get_dfu_img_info(&dfu_image_info))
        return;

    if (!check_boot_info(&dfu_image_info.boot_info)) {
        return;
    }

    // check image

    uint32_t buf      = dfu_image_info.boot_info.load_addr;
    uint32_t bin_size = dfu_image_info.boot_info.bin_size;

    if (!check_image_crc((uint8_t*)buf, bin_size, dfu_image_info.boot_info.check_sum)) {
        return;
    }

    dfu_start_address(&dfu_image_info.boot_info);
}

void dfu_enter(void) {
    _run_dfu_boot();
}