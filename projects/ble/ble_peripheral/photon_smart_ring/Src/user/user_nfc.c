#include "user_nfc.h"

#include "app_i2c.h"
#include "app_log.h"

#include "ST25DV04KC/st25dv.h"
#include "ST25DV04KC/st25dv_reg.h"

#include "user_common.h"

#define ST25DV_I2C_ID APP_I2C_ID_0

/* Registers i2c address */
#define ST25DV_REG_LEN 2

#define ST25DV_DATA_ADDR_OP 0x00

int32_t _st25dv_i2c_init(void) {
    return ST25DV_OK;
}

int32_t _st25dv_i2c_deinit(void) {
    return ST25DV_OK;
}

int32_t _st25dv_i2c_is_ready(uint16_t dev_addr, const uint32_t trials) {
    int      ret;
    uint8_t  buf;
    uint32_t cnt = trials;

    i2c_handle_t* i2c_handle;

    dev_addr = dev_addr >> 1;

    do {
        i2c_handle = app_i2c_get_handle(ST25DV_I2C_ID);
        if (i2c_handle->state != HAL_I2C_STATE_READY) {
            ret = i2c_handle->state;
            APP_LOG_ERROR("I2C BUS IS BUSY");
        } else {
            ret = 0;
        }

        cnt--;
    } while (ret != 0 && cnt > 0);

    if (ret != 0 && cnt == 0) {
        APP_LOG_ERROR("ST25DV IS BUSY");
        return ST25DV_BUSY;
    }

    return ST25DV_OK;
}

int32_t _st25dv_i2c_write(uint16_t dev_addr, uint16_t reg_addr, uint8_t* data, uint16_t size) {
    uint16_t  ret;

    dev_addr = dev_addr >> 1;

    ret = app_i2c_mem_write_sync(ST25DV_I2C_ID, dev_addr, reg_addr, ST25DV_REG_LEN, data, size, 0x1000);
    if (ret != 0) {
        APP_LOG_ERROR("write reg[%#.4x] failed with %#.4x", reg_addr, ret);
        return ST25DV_ERROR;
    }

    return ST25DV_OK;
}

int32_t _st25dv_i2c_read(uint16_t dev_addr, uint16_t reg_addr, uint8_t* buf, uint16_t size) {
    uint16_t  ret;

    dev_addr = dev_addr >> 1;

    ret = app_i2c_mem_read_sync(ST25DV_I2C_ID, dev_addr, reg_addr, ST25DV_REG_LEN, buf, size, 0x1000);
    if (ret != 0) {
        APP_LOG_ERROR("read reg[%#.4x] failed with %#.4x", reg_addr, ret);
        return ST25DV_ERROR;
    }

    return ST25DV_OK;
}

uint32_t _st25dv_i2c_get_tick(void) {
    uint64_t tick = rtc_get_relative_ms();
    return (int)tick;
}

void _st25dv_check_ic_ref(uint8_t ic_ref) {
    switch (ic_ref) {
        case I_AM_ST25DV04:
            APP_LOG_INFO("Drivers is ST25DV04K_IE");
            break;
        case I_AM_ST25DV04_J:
            APP_LOG_INFO("Drivers is ST25DV04K_JF");
            break;
        case I_AM_ST25DV64:
            APP_LOG_INFO("Drivers is ST25DV16K_IE or ST25DV64K_IE");
            break;
        case I_AM_ST25DV64_J:
            APP_LOG_INFO("Drivers is ST25DV16K_JF or ST25DV64K_JF");
            break;
        case I_AM_ST25DV04_KC:
            APP_LOG_INFO("Drivers is ST25DV04KC_IE");
            break;
        case I_AM_ST25DV64_KC:
            APP_LOG_INFO("Drivers is ST25DV16KC_IE or ST25DV64KC_IE");
            break;
        case I_AM_ST25DV04_KC_J:
            APP_LOG_INFO("Drivers is ST25DV04KC_JF");
            break;
        case I_AM_ST25DV64_KC_J:
            APP_LOG_INFO("Drivers is ST25DV16KC_JF or ST25DV64KC_JF");
            break;
        default:
            APP_LOG_INFO("Errors in testing equipment");
            break;
    }
}

static ST25DV_Object_t st25dv_obj;

static ST25DV_IO_t st25dv_io = {
    .Init    = _st25dv_i2c_init,
    .DeInit  = _st25dv_i2c_deinit,
    .IsReady = _st25dv_i2c_is_ready,
    .Write   = _st25dv_i2c_write,
    .Read    = _st25dv_i2c_read,
    .GetTick = _st25dv_i2c_get_tick,
};

extern ST25DV_Drv_t St25Dv_Drv;

uint16_t user_nfc_init(void) {
    int ret;

    ret = ST25DV_RegisterBusIO(&st25dv_obj, &st25dv_io);
    if (ret != ST25DV_OK) {
        APP_LOG_ERROR("ST25DV_RegisterBusIO failed: %d", ret);
        return GUNTER_FAILURE;
    }

    ret = St25Dv_Drv.Init(&st25dv_obj);
    if (ret != ST25DV_OK) {
        APP_LOG_ERROR("St25Dv_Drv.Init failed: %d", ret);
        return GUNTER_FAILURE;
    }

    uint8_t ic_ref;
    ret = St25Dv_Drv.ReadID(&st25dv_obj, &ic_ref);
    if (ret != ST25DV_OK) {
        APP_LOG_ERROR("St25Dv_Drv.ReadID failed: %d", ret);
        return GUNTER_FAILURE;
    }

    _st25dv_check_ic_ref(ic_ref);

    return GUNTER_SUCCESS;
}

uint16_t user_nfc_read_user_mem(uint16_t user_memory_addr, uint8_t* buffer, uint16_t size) {
    int ret;

    ret = St25Dv_Drv.ReadData(&st25dv_obj, buffer, user_memory_addr, size);
    if (ret != ST25DV_OK) {
        APP_LOG_ERROR("St25Dv_Drv.ReadData failed: %d", ret);
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

uint16_t user_nfc_write_user_mem(uint16_t user_memory_addr, uint8_t* data, uint16_t size) {
    int ret;

    ret = St25Dv_Drv.WriteData(&st25dv_obj, data, user_memory_addr, size);
    if (ret != ST25DV_OK) {
        APP_LOG_ERROR("St25Dv_Drv.WriteData failed: %d", ret);
        return GUNTER_FAILURE;
    }

    sys_delay_ms((size / 8) * 5 + 5); // 5ms per 16 bytes

    return GUNTER_SUCCESS;
}

uint16_t user_nfc_read_sys_reg(uint16_t reg_addr, uint8_t* buffer, uint16_t size) {
    int ret;

    ret = ST25DV_ReadRegister(&st25dv_obj, buffer, reg_addr, size);
    if (ret != ST25DV_OK) {
        APP_LOG_ERROR("ST25DV_ReadRegister failed: %d", ret);
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

uint16_t user_nfc_write_sys_reg(uint16_t reg_addr, uint8_t* data, uint16_t size) {
    int ret;

    ret = ST25DV_WriteRegister(&st25dv_obj, data, reg_addr, size);
    if (ret != ST25DV_OK) {
        APP_LOG_ERROR("ST25DV_WriteRegister failed: %d", ret);
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

// test func
void user_nfc_test(void) {
    uint8_t data[32]  = {0x01, 0xEE, 0x33, 0x0E, 0x34, 0x71, 0xF2, 0x01,
                         0x01, 0xEE, 0x33, 0x0E, 0x34, 0x71, 0xF2, 0x02,
                         0x01, 0xEE, 0x33, 0x0E, 0x34, 0x71, 0xF2, 0x03,
                         0x01, 0xEE, 0x33, 0x0E, 0x34, 0x71, 0xF2, 0x04};
    uint8_t buffer[64] = {0};

    // user_nfc_init();

    user_nfc_write_user_mem(0x0000, data, sizeof(data));
    // data_stream_hex(data, sizeof(data));

    // sys_delay_ms(10);

    user_nfc_read_user_mem(0x0000, buffer, sizeof(buffer));
    data_stream_hex(buffer, sizeof(buffer));
}