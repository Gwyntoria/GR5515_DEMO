#include "user_file_sys.h"

#include <stdio.h>
#include <string.h>

#include "app_log.h"

#include "GD25LE128E/Flash_Spi.h"
#include "user_app.h"
#include "user_common.h"

static FlashZoneConfig s_flash_zone_config;
static FlashZoneInfo   s_flash_zone_info_log;
static FlashZoneInfo   s_flash_zone_info_data;

static FlashZoneDataRange s_flash_zone_data_range_table[kFlashZone_Max] = {
    {
     .flash_zone_info      = NULL,
     .data_range_sector_op = FS_SECTOR_OP_CFG,
     .data_range_sector_ed = FS_SECTOR_ED_CFG,
     .data_range_addr_op   = FS_ADDR_OP_CFG,
     .data_range_addr_ed   = FS_ADDR_ED_CFG,
     },
    {
     .flash_zone_info      = &s_flash_zone_info_log,
     .data_range_sector_op = FS_SECTOR_OP_LOG + 1,
     .data_range_sector_ed = FS_SECTOR_ED_LOG,
     .data_range_addr_op   = FS_ADDR_OP_LOG + FS_SECTOR_SIZE,
     .data_range_addr_ed   = FS_ADDR_ED_LOG,
     },
    {
     .flash_zone_info      = &s_flash_zone_info_data,
     .data_range_sector_op = FS_SECTOR_OP_DAT + 1,
     .data_range_sector_ed = FS_SECTOR_ED_DAT,
     .data_range_addr_op   = FS_ADDR_OP_DTA + FS_SECTOR_SIZE,
     .data_range_addr_ed   = FS_ADDR_ED_DTA,
     }
};

int _erase_flash_zone_data(FlashZone flash_zone) {
    int      ret       = 0;
    uint16_t sector_op = 0;
    uint16_t sector_ed = 0;
    uint32_t addr      = 0;

    switch (flash_zone) {
        case kFlashZone_Config:
            sector_op = FS_SECTOR_OP_CFG;
            sector_ed = FS_SECTOR_ED_CFG;
            break;
        case kFlashZone_Log:
            sector_op = FS_SECTOR_OP_LOG + 1;
            sector_ed = FS_SECTOR_ED_LOG;
            break;

        case kFlashZone_SensorData:
            sector_op = FS_SECTOR_OP_DAT + 1;
            sector_ed = FS_SECTOR_ED_DAT;
            break;

        default:
            APP_LOG_ERROR("flash_zone[%d] error", flash_zone);
            return GUNTER_ERR_INVALID_TYPE;
            // break;
    }

    for (int i = sector_op; i < sector_ed; i++) {
        addr = sector_op * FS_SECTOR_SIZE;
        addr = FS_SECTOR_BASE(addr);
        ret  = flash_erase_sector(addr);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("ufs: flash_erase_sector failed");
            return GUNTER_FAILURE;
        }
    }

    return GUNTER_SUCCESS;
}

int _serialize_config_data(uint8_t* buffer, FlashZoneConfig* flash_zone_config) {
    if (buffer == NULL || flash_zone_config == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    uint16_t offset = 0;

    buffer[offset] = flash_zone_config->status;
    offset++;
    write_big_endian_2(buffer + offset, flash_zone_config->pack_len);
    offset += 2;
    write_big_endian_4(buffer + offset, flash_zone_config->wt_addr);
    offset += 4;
    buffer[offset] = flash_zone_config->ver_major;
    offset++;
    buffer[offset] = flash_zone_config->ver_minor;
    offset++;
    buffer[offset] = flash_zone_config->ver_build;
    offset++;
    buffer[offset] = flash_zone_config->ver_exten;
    offset++;
    buffer[offset] = flash_zone_config->activation;
    offset++;

    return GUNTER_SUCCESS;
}

int _deserialize_config_data(uint8_t* buffer, FlashZoneConfig* flash_zone_config) {
    if (buffer == NULL || flash_zone_config == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    APP_LOG_INFO("_deserialize_config_data");

    uint16_t offset = 0;

    flash_zone_config->status = buffer[offset];
    offset++;
    flash_zone_config->pack_len = read_big_endian_2(buffer + offset);
    offset += 2;
    flash_zone_config->wt_addr = read_big_endian_4(buffer + offset);
    offset += 4;
    flash_zone_config->ver_major = buffer[offset];
    offset++;
    flash_zone_config->ver_minor = buffer[offset];
    offset++;
    flash_zone_config->ver_build = buffer[offset];
    offset++;
    flash_zone_config->ver_exten = buffer[offset];
    offset++;
    flash_zone_config->activation = buffer[offset];
    offset++;

    APP_LOG_DEBUG("config status: %02x", flash_zone_config->status);

    APP_LOG_INFO("_deserialize_config_data finished");

    return GUNTER_SUCCESS;
}

int _read_config(void) {
    int     ret = 0;
    uint8_t flash_zone_config_buf[FS_CFG_DATA_LEN];

    memset(flash_zone_config_buf, 0, sizeof(flash_zone_config_buf));

    ret = flash_read_data(FS_ADDR_OP_CFG, flash_zone_config_buf, FS_CFG_DATA_LEN);
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: reading config data failed");
        return GUNTER_FAILURE;
    }

    APP_LOG_INFO("read flash config:");
    data_stream_hex(flash_zone_config_buf, FS_CFG_DATA_LEN);

    ret = _deserialize_config_data(flash_zone_config_buf, &s_flash_zone_config);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs: reading config data failed");
        return GUNTER_FAILURE;
    }

    // APP_LOG_INFO("_read_config finished");

    return GUNTER_SUCCESS;
}

int _update_config(FlashZoneConfig* flash_zone_config) {
    int     ret = 0;
    uint8_t flash_zone_config_buf[FS_CFG_DATA_LEN];

    memset(flash_zone_config_buf, 0, sizeof(flash_zone_config_buf));

    ret = _serialize_config_data(flash_zone_config_buf, flash_zone_config);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs: _serialize_config_data failed");
        return GUNTER_FAILURE;
    }

    APP_LOG_INFO("update flash config:");
    data_stream_hex(flash_zone_config_buf, FS_CFG_DATA_LEN);

    ret = flash_update_sector_data(FS_ADDR_OP_CFG, flash_zone_config_buf, FS_CFG_DATA_LEN);
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: writing config data failed");
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

int _init_config(void) {
    APP_LOG_INFO("_init_config");

    int ret = 0;

    ret = _read_config();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_read_config failed with 0x%02x", ret);
        return GUNTER_FAILURE;
    }

    if (s_flash_zone_config.status != SS_DATA) {
        APP_LOG_ERROR("Init flash config zone");

        s_flash_zone_config.status     = SS_DATA;
        s_flash_zone_config.pack_len   = FS_CFG_DATA_LEN;
        s_flash_zone_config.wt_addr    = FS_ADDR_OP_CFG + FS_CFG_DATA_LEN;
        s_flash_zone_config.ver_major  = VER_MAJOR;
        s_flash_zone_config.ver_minor  = VER_MINOR;
        s_flash_zone_config.ver_build  = VER_BUILD;
        s_flash_zone_config.ver_exten  = VER_EXTEN;
        s_flash_zone_config.activation = BLE_ACTIVA_OFF;

        ret = _update_config(&s_flash_zone_config);
        if (ret != FLASH_SUCCESS) {
            APP_LOG_ERROR("ufs: _update_config failed");
            return GUNTER_FAILURE;
        }

    } else if (s_flash_zone_config.status == SS_DATA) {
        if (s_flash_zone_config.ver_major != VER_MAJOR ||
            s_flash_zone_config.ver_minor != VER_MINOR ||
            s_flash_zone_config.ver_build != VER_BUILD ||
            s_flash_zone_config.ver_exten != VER_EXTEN) {
            APP_LOG_INFO("app version update");

            s_flash_zone_config.pack_len  = FS_CFG_DATA_LEN;
            s_flash_zone_config.wt_addr   = FS_ADDR_OP_CFG + FS_CFG_DATA_LEN;
            s_flash_zone_config.ver_major = VER_MAJOR;
            s_flash_zone_config.ver_minor = VER_MINOR;
            s_flash_zone_config.ver_build = VER_BUILD;
            s_flash_zone_config.ver_exten = VER_EXTEN;

            ret = _update_config(&s_flash_zone_config);
            if (ret != FLASH_SUCCESS) {
                APP_LOG_ERROR("ufs: _update_config failed");
                return GUNTER_FAILURE;
            }
        }
    }

    // TODO: 完善配置解析

    return GUNTER_SUCCESS;
}

int _serialize_flash_zone_info(uint8_t* buffer, FlashZoneInfo* flash_zone_info) {
    if (buffer == NULL || flash_zone_info == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    uint16_t offset = 0;

    buffer[offset] = flash_zone_info->status;
    offset++;
    buffer[offset] = flash_zone_info->type;
    offset++;
    write_big_endian_2(buffer + offset, flash_zone_info->pack_len);
    offset += 2;
    write_big_endian_2(buffer + offset, flash_zone_info->sector_op);
    offset += 2;
    write_big_endian_2(buffer + offset, flash_zone_info->sector_ed);
    offset += 2;
    write_big_endian_2(buffer + offset, flash_zone_info->rd_sector);
    offset += 2;
    write_big_endian_2(buffer + offset, flash_zone_info->wt_sector);
    offset += 2;
    write_big_endian_2(buffer + offset, flash_zone_info->wt_page);
    offset += 2;
    write_big_endian_4(buffer + offset, flash_zone_info->rd_addr);
    offset += 4;
    write_big_endian_4(buffer + offset, flash_zone_info->wt_addr);
    APP_LOG_DEBUG("flash_zone_info->wt_addr = %u", flash_zone_info->wt_addr);
    data_stream_hex(buffer + offset, 4);
    offset += 4;
    buffer[offset] = flash_zone_info->cycle_state;
    offset++;

    return GUNTER_SUCCESS;
}

int _deserialize_flash_zone_info(uint8_t* buffer, FlashZoneInfo* flash_zone_info) {
    if (buffer == NULL || flash_zone_info == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    uint16_t offset = 0;

    flash_zone_info->status = buffer[offset];
    offset++;
    flash_zone_info->type = buffer[offset];
    offset++;
    flash_zone_info->pack_len = read_big_endian_2(buffer + offset);
    offset += 2;
    flash_zone_info->sector_op = read_big_endian_2(buffer + offset);
    offset += 2;
    flash_zone_info->sector_ed = read_big_endian_2(buffer + offset);
    offset += 2;
    flash_zone_info->rd_sector = read_big_endian_2(buffer + offset);
    offset += 2;
    flash_zone_info->wt_sector = read_big_endian_2(buffer + offset);
    offset += 2;
    flash_zone_info->wt_page = read_big_endian_2(buffer + offset);
    offset += 2;
    flash_zone_info->rd_addr = read_big_endian_4(buffer + offset);
    offset += 4;
    flash_zone_info->wt_addr = read_big_endian_4(buffer + offset);
    offset += 4;
    flash_zone_info->cycle_state = buffer[offset];
    offset++;

    return GUNTER_SUCCESS;
}

int _read_flash_zone_info(FlashZone flash_zone) {
    int            ret             = 0;
    uint32_t       addr            = 0;
    FlashZoneInfo* flash_zone_info = NULL;
    uint8_t        flash_zone_info_buf[FS_ZONE_INFO_LEN];

    memset(flash_zone_info_buf, 0, sizeof(flash_zone_info_buf));

    switch (flash_zone) {
        case kFlashZone_Log:
            addr            = FS_ADDR_OP_LOG;
            flash_zone_info = &s_flash_zone_info_log;
            break;

        case kFlashZone_SensorData:
            addr            = FS_ADDR_OP_DTA;
            flash_zone_info = &s_flash_zone_info_data;
            break;

        default:
            APP_LOG_ERROR("FlashZone error");
            return GUNTER_ERR_INVALID_TYPE;
            // break;
    }

    ret = flash_read_data(addr, flash_zone_info_buf, FS_ZONE_INFO_LEN);
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: reading flash zone info failed");
        return GUNTER_FAILURE;
    }

    APP_LOG_DEBUG("read flash_zone[%d] info:", flash_zone);
    data_stream_hex(flash_zone_info_buf, FS_ZONE_INFO_LEN);

    ret = _deserialize_flash_zone_info(flash_zone_info_buf, flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs: _deserialize_flash_zone_info failed");
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

int _update_flash_zone_info(FlashZoneInfo* flash_zone_info) {
    int      ret  = 0;
    uint32_t addr = 0;
    uint8_t  flash_zone_info_buf[FS_ZONE_INFO_LEN];

    memset(flash_zone_info_buf, 0, sizeof(flash_zone_info_buf));

    switch (flash_zone_info->type) {
        case kFlashZone_Log:
            addr = FS_ADDR_OP_LOG;
            break;

        case kFlashZone_SensorData:
            addr = FS_ADDR_OP_DTA;
            break;

        default:
            APP_LOG_ERROR("FlashZone error");
            return GUNTER_ERR_INVALID_TYPE;
            // break;
    }

    ret = _serialize_flash_zone_info(flash_zone_info_buf, flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs: _serialize_config_data failed");
        return GUNTER_FAILURE;
    }

    APP_LOG_DEBUG("update flash_zone[%d] info:", flash_zone_info->type);
    data_stream_hex(flash_zone_info_buf, FS_ZONE_INFO_LEN);

    ret = flash_update_sector_data(addr, flash_zone_info_buf, FS_ZONE_INFO_LEN);
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: writing config data failed");
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

int _init_flash_zone_info(FlashZone flash_zone) {
    int            ret             = 0;
    FlashZoneInfo* flash_zone_info = NULL;
    uint16_t       sector_op       = 0;
    uint16_t       sector_ed       = 0;
    uint16_t       rd_sector       = 0;
    uint16_t       wt_sector       = 0;
    uint32_t       rd_addr         = 0;
    uint32_t       wt_addr         = 0;

    ret = _read_flash_zone_info(flash_zone);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_read_flash_zone_info[%u] failed!", flash_zone);
        return GUNTER_FAILURE;
    }

    // APP_LOG_DEBUG("_init_flash_zone_info[%d]", flash_zone);

    switch (flash_zone) {
        case kFlashZone_Log:
            flash_zone_info = &s_flash_zone_info_log;
            sector_op       = FS_SECTOR_OP_LOG;
            sector_ed       = FS_SECTOR_ED_LOG;
            rd_sector       = FS_SECTOR_OP_LOG + 1;
            wt_sector       = FS_SECTOR_OP_LOG + 1;
            rd_addr         = FS_ADDR_OP_LOG + FS_SECTOR_SIZE;
            wt_addr         = FS_ADDR_OP_LOG + FS_SECTOR_SIZE;
            break;

        case kFlashZone_SensorData:
            flash_zone_info = &s_flash_zone_info_data;
            sector_op       = FS_SECTOR_OP_DAT;
            sector_ed       = FS_SECTOR_ED_DAT;
            rd_sector       = FS_SECTOR_OP_DAT + 1;
            wt_sector       = FS_SECTOR_OP_DAT + 1;
            rd_addr         = FS_ADDR_OP_DTA + FS_SECTOR_SIZE;
            wt_addr         = FS_ADDR_OP_DTA + FS_SECTOR_SIZE;
            break;

        default:
            APP_LOG_ERROR("FlashZone error");
            return GUNTER_ERR_INVALID_TYPE;
            // break;
    }

    APP_LOG_DEBUG("sector_op: %u", sector_op);
    APP_LOG_DEBUG("sector_ed: %u", sector_ed);
    APP_LOG_DEBUG("rd_sector: %u", rd_sector);
    APP_LOG_DEBUG("wt_sector: %u", wt_sector);
    APP_LOG_DEBUG("rd_addr: %u", rd_addr);
    APP_LOG_DEBUG("wt_addr: %u", wt_addr);

    if (flash_zone_info->status != SS_DATA) {
        APP_LOG_INFO("Init flash_zone[%d] info", flash_zone);

        flash_zone_info->status      = SS_DATA;
        flash_zone_info->type        = flash_zone;
        flash_zone_info->pack_len    = FS_ZONE_INFO_LEN;
        flash_zone_info->sector_op   = sector_op;
        flash_zone_info->sector_ed   = sector_ed;
        flash_zone_info->rd_sector   = rd_sector;
        flash_zone_info->wt_sector   = wt_sector;
        flash_zone_info->wt_page     = 0;
        flash_zone_info->rd_addr     = rd_addr;
        flash_zone_info->wt_addr     = wt_addr;
        flash_zone_info->cycle_state = FS_ZONE_CYCLE_OFF;

        APP_LOG_DEBUG("flash_zone_info->sector_op: %u", flash_zone_info->sector_op);
        APP_LOG_DEBUG("flash_zone_info->wt_addr: %u", flash_zone_info->wt_addr);

        _update_flash_zone_info(flash_zone_info);
    } else if (flash_zone_info->status == SS_DATA) {
        if (flash_zone_info->pack_len != FS_ZONE_INFO_LEN ||
            flash_zone_info->type != flash_zone) {
            APP_LOG_INFO("Reinit flash_zone[%d] info", flash_zone);

            flash_zone_info->status      = SS_DATA;
            flash_zone_info->type        = flash_zone;
            flash_zone_info->pack_len    = FS_ZONE_INFO_LEN;
            flash_zone_info->sector_op   = sector_op;
            flash_zone_info->sector_ed   = sector_ed;
            flash_zone_info->rd_sector   = rd_sector;
            flash_zone_info->wt_sector   = wt_sector;
            flash_zone_info->wt_page     = 0;
            flash_zone_info->rd_addr     = rd_addr;
            flash_zone_info->wt_addr     = wt_addr;
            flash_zone_info->cycle_state = FS_ZONE_CYCLE_OFF;

            _update_flash_zone_info(flash_zone_info);

            _erase_flash_zone_data(flash_zone);
        }
    }

    return GUNTER_SUCCESS;
}

int _get_flash_zone_range_info(FlashZone       flash_zone,
                               uint32_t*       range_sector_op,
                               uint32_t*       range_sector_ed,
                               uint32_t*       range_addr_op,
                               uint32_t*       range_addr_ed,
                               FlashZoneInfo** flash_zone_info) {
    if (flash_zone != kFlashZone_Log &&
        flash_zone != kFlashZone_SensorData) {
        APP_LOG_ERROR("FlashZone error");
        return GUNTER_ERR_INVALID_TYPE;
    }

    *flash_zone_info = s_flash_zone_data_range_table[flash_zone].flash_zone_info;
    *range_sector_op = s_flash_zone_data_range_table[flash_zone].data_range_sector_op;
    *range_sector_ed = s_flash_zone_data_range_table[flash_zone].data_range_sector_ed;
    *range_addr_op   = s_flash_zone_data_range_table[flash_zone].data_range_addr_op;
    *range_addr_ed   = s_flash_zone_data_range_table[flash_zone].data_range_addr_ed;

    if (*flash_zone_info == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    if ((*flash_zone_info)->status != SS_DATA) {
        APP_LOG_ERROR("status: %d", (*flash_zone_info)->status);
        APP_LOG_ERROR("ufs_rd: flash zone[%d] is not initialized!", flash_zone);
        return GUNTER_ERR_NOT_INITIAL;
    }

    if ((*flash_zone_info)->type != flash_zone) {
        APP_LOG_ERROR("ufs_rd: flash zone[%d] is invalid!", flash_zone);
        return GUNTER_ERR_INVALID_TYPE;
    }

    return GUNTER_SUCCESS;
}

int _read_data_per_page(uint32_t addr, uint32_t len, uint8_t* buffer) {
    int      ret           = 0;
    uint32_t data_len      = 0;
    uint32_t rd_len        = 0;
    uint32_t buffer_offset = 0;

    while (len) {
        if (len > FS_PAGE_SIZE) {
            rd_len = FS_PAGE_SIZE;
        } else {
            rd_len = len;
        }

        ret = flash_read_data(addr, buffer + buffer_offset, rd_len);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("ufs_rd: flash_read_data faailed with 0x%02x", ret);
            break;
        }

        APP_LOG_DEBUG("read addr: %u", addr);
        // data_stream_hex(buffer + buffer_offset, rd_len);

        addr += rd_len;
        buffer_offset += rd_len;
        len -= rd_len;

        data_len += rd_len;
    }

    return data_len;
}

int ufs_init(void) {
    int ret = 0;

    ret = flash_init();
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: flash_init failed");
        return GUNTER_FAILURE;
    }

    memset(&s_flash_zone_config, 0, sizeof(FlashZoneConfig));
    memset(&s_flash_zone_info_log, 0, sizeof(FlashZoneInfo));
    memset(&s_flash_zone_info_data, 0, sizeof(FlashZoneInfo));

    ret = _init_config();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_init_config failed with 0x%02x", ret);
        return GUNTER_FAILURE;
    }

    ret = _init_flash_zone_info(kFlashZone_Log);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_init_flash_zone_info log failed!");
        return GUNTER_FAILURE;
    }

    ret = _init_flash_zone_info(kFlashZone_SensorData);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_init_flash_zone_info data failed!");
        return GUNTER_FAILURE;
    }

    return ret;
}

int ufs_reinit(void) {
    int ret = 0;

    ret = flash_init();
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: flash_init failed");
        return GUNTER_FAILURE;
    }

    flash_erase_chip();

    APP_LOG_INFO("flash_erase_chip");

    return GUNTER_SUCCESS;
}

int ufs_write_data(FlashZone flash_zone, uint8_t* data, uint32_t len) {
    if (data == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    if (len > FS_WRITE_MAX_SIZE) {
        APP_LOG_ERROR("ufs_wt: len(%u) is bigger than 4096", len);
        return GUNTER_ERR_INVALID_PARAM;
    }

    int ret = 0;

    ret = flash_init();
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: flash_init failed");
        return GUNTER_FAILURE;
    }

    ret = _read_flash_zone_info(flash_zone);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_rd: _read_flash_zone_info[%d] failed with 0x%02x", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint32_t       range_sector_op = 0; // 分区可写入开始扇区
    uint32_t       range_sector_ed = 0; // 分区可写入结束扇区
    uint32_t       range_addr_op   = 0; // 分区可写入开始地址
    uint32_t       range_addr_ed   = 0; // 分区可写入结束地址
    FlashZoneInfo* flash_zone_info = NULL;

    ret = _get_flash_zone_range_info(flash_zone,
                                     &range_sector_op,
                                     &range_sector_ed,
                                     &range_addr_op,
                                     &range_addr_ed,
                                     &flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_rd: _get_flash_zone_range_info[%d] failed with 0x%02x", flash_zone, ret);
        return ret;
    }

    // uint32_t rd_addr     = flash_zone_info->rd_addr;     // 可读取地址
    uint32_t wt_addr     = flash_zone_info->wt_addr;     // 可写入地址
    uint16_t rd_sector   = flash_zone_info->rd_sector;   // 当前读取扇区
    uint16_t wt_sector   = flash_zone_info->wt_sector;   // 当前写入扇区
    uint16_t wt_page     = flash_zone_info->wt_page;     // 当前写入扇区中的写入页
    uint8_t  cycle_state = flash_zone_info->cycle_state; // 分区循环状态
    uint8_t  er_sector   = 0;                            // 记录是否需要擦除写入地址所在扇区

    APP_LOG_DEBUG("wt_addr: %u", wt_addr);
    APP_LOG_DEBUG("range_addr_op: %u", range_addr_op);
    APP_LOG_DEBUG("range_addr_ed: %u", range_addr_ed);

    if (wt_addr < range_addr_op || wt_addr >= range_addr_ed) {
        APP_LOG_ERROR("ufs_wt: invalid wt_addr");
        return GUNTER_ERR_INVALID_ADDR;
    }

    uint32_t wt_addr_ed = wt_addr + len; // 理论上的写入结束地址

    if (wt_addr_ed > range_addr_ed) {
        APP_LOG_INFO("ufs_wt: zone cycle");

        if (cycle_state != FS_ZONE_CYCLE_ON) {
            cycle_state = FS_ZONE_CYCLE_ON;
        }

        wt_sector = FS_SECTOR_OP_LOG + 1;
        wt_addr   = range_addr_op;
        er_sector = 1;
        wt_page   = 0;
        rd_sector += 1;
    }

    wt_addr_ed = wt_addr + len - 1;

    uint32_t wt_addr_sector_base    = FS_SECTOR_BASE(wt_addr);
    uint32_t wt_addr_ed_sector_base = FS_SECTOR_BASE(wt_addr_ed);

    if (wt_addr_ed_sector_base > wt_addr_sector_base) {
        wt_sector += 1;
        wt_addr   = wt_sector * FS_SECTOR_SIZE;
        er_sector = 1;
        wt_page   = 0;

        if (cycle_state == FS_ZONE_CYCLE_ON) {
            rd_sector += 1;

            if (rd_sector >= range_sector_ed) {
                rd_sector = range_sector_op;
            }
        }
    }

    if (er_sector) {
        flash_erase_sector(wt_addr);
    }

    uint32_t wt_len              = 0;
    uint32_t data_offset         = 0;
    uint32_t wt_addr_page_offset = FS_PAGE_OFFSET(wt_addr);
    uint32_t page_left           = FS_PAGE_SIZE - wt_addr_page_offset;

    if (len > page_left) {
        wt_len = page_left;
        wt_page += 1;
    } else {
        wt_len = len;
    }

    while (len) {
        ret = flash_write_data(wt_addr, data + data_offset, wt_len);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("ufs_wt: flash_write_data faailed with 0x%02x", ret);
            return ret;
        }

        APP_LOG_DEBUG("wtite addr: %u", wt_addr);
        data_stream_hex(data + data_offset, wt_len);

        wt_addr += wt_len;
        data_offset += wt_len;
        len -= wt_len;

        if (len > FS_PAGE_SIZE) {
            wt_len = FS_PAGE_SIZE;
            wt_page += 1;
        } else {
            wt_len = len;
        }
    }

    flash_zone_info->rd_sector   = rd_sector;
    flash_zone_info->wt_sector   = wt_sector;
    flash_zone_info->wt_page     = wt_page;
    flash_zone_info->rd_addr     = rd_sector * FS_SECTOR_SIZE;
    flash_zone_info->wt_addr     = wt_addr;
    flash_zone_info->cycle_state = cycle_state;

    _update_flash_zone_info(flash_zone_info);

    return GUNTER_SUCCESS;
}

int ufs_write_data_mass(FlashZone flash_zone, uint8_t* data, uint32_t len) {
    if (data == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    int      ret         = 0;
    uint32_t wt_len      = 0;
    uint32_t data_offset = 0;

    while (len) {
        if (len >= FS_WRITE_MAX_SIZE) {
            wt_len = FS_WRITE_MAX_SIZE;
        } else {
            wt_len = len;
        }

        ret = ufs_write_data(flash_zone, data + data_offset, wt_len);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("ufs_wt_m: ufs_write_data failed with 0x%02x", ret);
            return ret;
        }

        data_offset += wt_len;
        len -= wt_len;
    }

    return GUNTER_SUCCESS;
}

/**
 * @brief 读取 flash 中的数据。
 *        如果 whole 为 false，则读取最后一个 sector 中的数据，
 *        如果保存的数据超过一个 sector 则多读取一个 sector。
 *        如果 whole 为 true，则需要通过多次调用该方法来全部读取flash中的数据。
 *
 *        不可以多线程调用。
 *
 *        buffer 大小至少为(4096 * 2)个字节。
 *
 *        在调用该方法之前，需先调用 ufs_get_zone_data_sector_cnt 获取数据扇区数量
 *
 * @param[in] flash_zone @ref FlashZone
 * @param[out] buffer 保存读取数据的buffer
 * @param[in] whole 判断是否要全部读取写入数据
 * @return 读取成功则返回读取长度；读取失败是返回错误代码
 */
int ufs_read_data(FlashZone flash_zone, uint8_t* buffer, bool whole) {
    if (buffer == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    int ret = 0;

    ret = flash_init();
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: flash_init failed");
        return GUNTER_FAILURE;
    }

    ret = _read_flash_zone_info(flash_zone);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_rd: _read_flash_zone_info[%d] failed with 0x%02x", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint32_t       range_sector_op = 0; // 分区可写入开始扇区
    uint32_t       range_sector_ed = 0; // 分区可写入结束扇区
    uint32_t       range_addr_op   = 0; // 分区可写入开始地址
    uint32_t       range_addr_ed   = 0; // 分区可写入结束地址
    FlashZoneInfo* flash_zone_info = NULL;

    ret = _get_flash_zone_range_info(flash_zone,
                                     &range_sector_op,
                                     &range_sector_ed,
                                     &range_addr_op,
                                     &range_addr_ed,
                                     &flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_rd: _get_flash_zone_range_info[%d] failed with 0x%02x", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint32_t rd_addr             = flash_zone_info->rd_addr;     // 读取地址
    uint32_t wt_addr             = flash_zone_info->wt_addr;     // 写入地址
    uint32_t cycle_state         = flash_zone_info->cycle_state; // 写入地址
    uint32_t wt_addr_sector_base = FS_SECTOR_BASE(wt_addr);      // 写入地址的扇区基地址
    uint32_t whole_data_len      = 0;                            // 可读数据总长度

    if (cycle_state == FS_ZONE_CYCLE_ON) {
        whole_data_len = (range_addr_ed - rd_addr) + (wt_addr - range_addr_op);
    } else {
        whole_data_len = wt_addr - rd_addr;
    }

    uint32_t addr;
    uint32_t data_len; // 读取内容长度

    static uint32_t cyc_rd_len = 0;

    if (!whole) {
        addr     = wt_addr_sector_base;
        data_len = wt_addr - wt_addr_sector_base; // 读取内容长度

        if (((wt_addr_sector_base - range_addr_op) / FS_SECTOR_SIZE) > 0) {
            addr -= FS_SECTOR_SIZE;
            data_len += FS_SECTOR_SIZE;
        }

        ret = _read_data_per_page(addr, data_len, buffer);
        if (ret < 0) {
            APP_LOG_ERROR("_read_data_per_page failed");
            return GUNTER_FAILURE;
        }

    } else {
        addr = rd_addr + cyc_rd_len;
        if (addr >= range_addr_ed &&
            flash_zone_info->cycle_state == FS_ZONE_CYCLE_ON) {
            addr = range_addr_op + (cyc_rd_len - (range_addr_ed - rd_addr));
        }

        uint32_t left_data_len = whole_data_len - cyc_rd_len; // 未读取数据总长度

        if (left_data_len >= FS_SECTOR_SIZE) {
            data_len = FS_SECTOR_SIZE;
        } else {
            data_len = whole_data_len - cyc_rd_len;
        }

        ret = _read_data_per_page(addr, data_len, buffer);
        if (ret < 0) {
            APP_LOG_ERROR("_read_data_per_page failed");
            return GUNTER_FAILURE;
        }

        cyc_rd_len += data_len;
    }

    APP_LOG_DEBUG("cyc_rd_len: %u", cyc_rd_len);

    if (cyc_rd_len == whole_data_len) {
        cyc_rd_len = 0;
        APP_LOG_DEBUG("cyc_rd_len: %u", cyc_rd_len);
    }

    return data_len;
}

int ufs_get_zone_data_sector_cnt(FlashZone flash_zone) {
    int ret = 0;

    ret = _read_flash_zone_info(flash_zone);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_rd: _read_flash_zone_info[%d] failed with 0x%02x", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint32_t       range_sector_op = 0; // 分区可写入开始扇区
    uint32_t       range_sector_ed = 0; // 分区可写入结束扇区
    uint32_t       range_addr_op   = 0; // 分区可写入开始地址
    uint32_t       range_addr_ed   = 0; // 分区可写入结束地址
    FlashZoneInfo* flash_zone_info = NULL;

    ret = _get_flash_zone_range_info(flash_zone,
                                     &range_sector_op,
                                     &range_sector_ed,
                                     &range_addr_op,
                                     &range_addr_ed,
                                     &flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_rd: _get_flash_zone_range_info[%d] failed with 0x%02x", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint32_t rd_addr        = flash_zone_info->rd_addr;
    uint32_t wt_addr        = flash_zone_info->wt_addr;
    uint32_t whole_data_len = 0;

    if (flash_zone_info->cycle_state == FS_ZONE_CYCLE_ON) {
        whole_data_len = (range_addr_ed - rd_addr) + (wt_addr - range_addr_op);
    } else {
        whole_data_len = wt_addr - rd_addr;
    }

    int data_sector_cnt = 0;

    if ((whole_data_len % FS_SECTOR_SIZE) > 0) {
        data_sector_cnt = whole_data_len / FS_SECTOR_SIZE + 1;
    } else if ((whole_data_len % FS_SECTOR_SIZE) == 0) {
        data_sector_cnt = whole_data_len / FS_SECTOR_SIZE;
    }

    return data_sector_cnt;
}

int ufs_test(void) {
    int ret = 0;

    uint8_t buffer[4096 * 2];
    memset(buffer, 0, sizeof(buffer));

    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = i & 0xff;
    }

    APP_LOG_INFO("===== write data ======");
    ret = ufs_write_data_mass(kFlashZone_Log, buffer, sizeof(buffer));
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_test: ufs_write_data_mass failed");
        return GUNTER_FAILURE;
    }

    int sector_cnt = ufs_get_zone_data_sector_cnt(kFlashZone_Log);
    APP_LOG_DEBUG("sector_cnt: %d", sector_cnt);

    memset(buffer, 0, sizeof(buffer));

    APP_LOG_INFO("===== read data ======");
    for (int i = 0; i < sector_cnt; i++) {
        ret = ufs_read_data(kFlashZone_Log, buffer, true);
        if (ret < 0) {
            APP_LOG_ERROR("ufs_test: ufs_read_data failed");
            return GUNTER_FAILURE;
        }

        data_stream_hex(buffer, ret);
    }

    return GUNTER_SUCCESS;
}
