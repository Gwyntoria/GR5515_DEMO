/**
 * @file ST25DV.c
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief NFC 初始化和tag写入
 * @version 0.1
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ST25DV.h"
#include "ST25DV_Reg.h"

#include "app_i2c.h"
#include "app_log.h"

/**
 * @brief ST25DV04K Initialization only check plant UID correctness
 *
 * @return int8_t ST25DV Initialization status.
 */

int8_t ST25DV_ReadUid(void) {
    uint8_t GET_ID;
    /** read ST25DV IC_REF*/
    app_i2c_mem_read_sync(ST25DV_I2C_ID, ST25DV_SYS_ADD, ST25DV_ICREF_REG, ST25DV_Reg_len, &GET_ID, 1, 0x1000);
    switch (GET_ID) {
        case ST25DV04K_IE_UID:
            APP_LOG_INFO("Drivers is ST25DV04K_IE");
            return NFCTAG_OK;
        case ST25DV04K_JF_UID:
            APP_LOG_INFO("Drivers is ST25DV04K_JF");
            return NFCTAG_OK;
        case ST25DV64K_IE_UID:
            APP_LOG_INFO("Drivers is ST25DV16K_IE or ST25DV64K_IE");
            return NFCTAG_OK;
        case ST25DV64K_JF_UID:
            APP_LOG_INFO("Drivers is ST25DV16K_JF or ST25DV64K_JF");
            return NFCTAG_OK;
        case ST25DV04KC_IE_UID:
            APP_LOG_INFO("Drivers is ST25DV04KC_IE");
            return NFCTAG_OK;
        case ST25DV64KC_IE_UID:
            APP_LOG_INFO("Drivers is ST25DV16KC_IE or ST25DV64KC_IE");
            return NFCTAG_OK;
        case ST25DV04KC_JF_UID:
            APP_LOG_INFO("Drivers is ST25DV04KC_JF");
            return NFCTAG_OK;
        case ST25DV64KC_JF_UID:
            APP_LOG_INFO("Drivers is ST25DV16KC_JF or ST25DV64KC_JF");
            return NFCTAG_OK;
        default:
            APP_LOG_INFO("Errors in testing equipment");
            return NFCTAG_ERROR;
    }
}

/**
 * @brief ST25DV write data to user memory
 *
 * @param User_memory_add   need memory began address
 * @param pData             input data point
 * @param size              input data size
 * @return int8_t           write data status
 */
int8_t ST25DV_WriteData(uint16_t User_memory_add, uint8_t* pData, uint16_t size) {
    uint16_t ret;
    ret = app_i2c_mem_write_sync(ST25DV_I2C_ID, ST25DV_User_ADD, User_memory_add, ST25DV_Reg_len, pData, size, 0x1000);
    if (ret == 0) {
        return NFCTAG_OK;
    } else
        return NFCTAG_ERROR;
}

/**
 * @brief ST25DV Read data to user memory
 *
 * @param User_memory_add   need memory began address
 * @param pData             input data point
 * @param size              input data size
 * @return int8_t           write data status
 */
int8_t ST25DV_ReadData(uint16_t User_memory_add, uint8_t* pData, uint16_t size) {
    uint16_t ret;
    ret = app_i2c_mem_read_sync(ST25DV_I2C_ID, ST25DV_SYS_ADD, User_memory_add, ST25DV_Reg_len, pData, size, 0x1000);
    if (ret == 0) {
        return NFCTAG_OK;
    } else
        return NFCTAG_ERROR;
}

/**
 * @brief ST25DV Read data to user memory
 *
 * @param User_memory_add   need memory began address
 * @param pData             input data point
 * @param size              input data size
 * @return int8_t           write data status
 */
int8_t ST25DV_ReadRegData(uint16_t Reg_add, uint8_t* pData, uint16_t size) {
    uint16_t ret;
    ret = app_i2c_mem_read_sync(ST25DV_I2C_ID, ST25DV_User_ADD, Reg_add, ST25DV_Reg_len, pData, size, 0x1000);
    if (ret == 0) {
        return NFCTAG_OK;
    } else
        return NFCTAG_ERROR;
}
