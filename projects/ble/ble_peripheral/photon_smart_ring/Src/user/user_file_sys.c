#include "user_file_sys.h"

#include <stdio.h>
#include <string.h>

#include "app_log.h"

#include "GD25LE128E/Flash_Spi.h"
#include "user_app.h"
#include "user_common.h"

static uint8_t s_flash_usage_status = FS_USAGE_STATUS_OFF;

static FlashZoneConfig s_flash_zone_config;
static FlashZoneInfo   s_flash_zone_info_log;
static FlashZoneInfo   s_flash_zone_info_data;

static FlashZoneDataRange s_flash_zone_data_range_table[kFlashZoneMax] = {
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
    uint16_t sector_op = 0;
    uint16_t sector_ed = 0;
    uint32_t addr      = 0;

    switch (flash_zone) {
        case kFlashZoneConfig:
            sector_op = FS_SECTOR_OP_CFG;
            sector_ed = FS_SECTOR_ED_CFG;
            break;
        case kFlashZoneLog:
            sector_op = FS_SECTOR_OP_LOG + 1;
            sector_ed = FS_SECTOR_ED_LOG;
            break;

        case kFlashZoneData:
            sector_op = FS_SECTOR_OP_DAT + 1;
            sector_ed = FS_SECTOR_ED_DAT;
            break;

        default:
            APP_LOG_ERROR("flash_zone[%d] error", flash_zone);
            return GUNTER_ERR_INVALID_TYPE;
            // break;
    }

    for (int i = sector_op; i < sector_ed; i++) {
        addr = i * FS_SECTOR_SIZE;
        addr = FS_SECTOR_BASE(addr);

        int8_t flash_ret = flash_erase_sector(addr);
        if (flash_ret != FLASH_SUCCESS) {
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
    int     ret;
    int8_t  flash_ret;
    uint8_t flash_zone_config_buf[FS_CFG_DATA_LEN];

    memset(flash_zone_config_buf, 0, sizeof(flash_zone_config_buf));

    flash_ret = flash_read_data(FS_ADDR_OP_CFG, flash_zone_config_buf, FS_CFG_DATA_LEN);
    if (flash_ret != FLASH_SUCCESS) {
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
    int     ret;
    int8_t  flash_ret;
    uint8_t flash_zone_config_buf[FS_CFG_DATA_LEN];

    memset(flash_zone_config_buf, 0, sizeof(flash_zone_config_buf));

    ret = _serialize_config_data(flash_zone_config_buf, flash_zone_config);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs: _serialize_config_data failed");
        return GUNTER_FAILURE;
    }

    APP_LOG_INFO("update flash config:");
    data_stream_hex(flash_zone_config_buf, FS_CFG_DATA_LEN);

    flash_ret = flash_update_sector_data(FS_ADDR_OP_CFG, flash_zone_config_buf, FS_CFG_DATA_LEN);
    if (flash_ret != FLASH_SUCCESS) {
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
        APP_LOG_ERROR("_read_config failed");
        return GUNTER_FAILURE;
    }

    if (s_flash_zone_config.status != SS_DATA) {
        APP_LOG_INFO("Init flash config zone");

        s_flash_zone_config.status     = SS_DATA;
        s_flash_zone_config.pack_len   = FS_CFG_DATA_LEN;
        s_flash_zone_config.wt_addr    = FS_ADDR_OP_CFG + FS_CFG_DATA_LEN;
        s_flash_zone_config.ver_major  = VER_MAJOR;
        s_flash_zone_config.ver_minor  = VER_MINOR;
        s_flash_zone_config.ver_build  = VER_BUILD;
        s_flash_zone_config.ver_exten  = VER_EXTEN;
        s_flash_zone_config.activation = BLE_ACTIVA_OFF;

        ret = _update_config(&s_flash_zone_config);
        if (ret != GUNTER_SUCCESS) {
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
            if (ret != GUNTER_SUCCESS) {
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
    int8_t         flash_ret       = 0;
    uint32_t       addr            = 0;
    FlashZoneInfo* flash_zone_info = NULL;
    uint8_t        flash_zone_info_buf[FS_ZONE_INFO_LEN];

    memset(flash_zone_info_buf, 0, sizeof(flash_zone_info_buf));

    switch (flash_zone) {
        case kFlashZoneLog:
            addr            = FS_ADDR_OP_LOG;
            flash_zone_info = &s_flash_zone_info_log;
            break;

        case kFlashZoneData:
            addr            = FS_ADDR_OP_DTA;
            flash_zone_info = &s_flash_zone_info_data;
            break;

        default:
            APP_LOG_ERROR("FlashZone error");
            return GUNTER_ERR_INVALID_TYPE;
            // break;
    }

    flash_ret = flash_read_data(addr, flash_zone_info_buf, FS_ZONE_INFO_LEN);
    if (flash_ret != FLASH_SUCCESS) {
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
    int      ret       = 0;
    int8_t   flash_ret = 0;
    uint32_t addr      = 0;
    uint8_t  flash_zone_info_buf[FS_ZONE_INFO_LEN];

    memset(flash_zone_info_buf, 0, sizeof(flash_zone_info_buf));

    switch (flash_zone_info->type) {
        case kFlashZoneLog:
            addr = FS_ADDR_OP_LOG;
            break;

        case kFlashZoneData:
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

    flash_ret = flash_update_sector_data(addr, flash_zone_info_buf, FS_ZONE_INFO_LEN);
    if (flash_ret != FLASH_SUCCESS) {
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
        case kFlashZoneLog:
            flash_zone_info = &s_flash_zone_info_log;
            sector_op       = FS_SECTOR_OP_LOG;
            sector_ed       = FS_SECTOR_ED_LOG;
            rd_sector       = FS_SECTOR_OP_LOG + 1;
            wt_sector       = FS_SECTOR_OP_LOG + 1;
            rd_addr         = FS_ADDR_OP_LOG + FS_SECTOR_SIZE;
            wt_addr         = FS_ADDR_OP_LOG + FS_SECTOR_SIZE;
            break;

        case kFlashZoneData:
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

        ret = _update_flash_zone_info(flash_zone_info);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("_update_flash_zone_info[%d] failed!", flash_zone);
            return GUNTER_FAILURE;
        }
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

            ret = _update_flash_zone_info(flash_zone_info);
            if (ret != GUNTER_SUCCESS) {
                APP_LOG_ERROR("_update_flash_zone_info[%d] failed!", flash_zone);
                return GUNTER_FAILURE;
            }

            ret = _erase_flash_zone_data(flash_zone);
            if (ret != GUNTER_SUCCESS) {
                APP_LOG_ERROR("_erase_flash_zone_data[%d] failed with %d", flash_zone, ret);
                return GUNTER_FAILURE;
            }
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
    if (flash_zone != kFlashZoneLog &&
        flash_zone != kFlashZoneData) {
        APP_LOG_ERROR("FlashZone error");
        return GUNTER_ERR_INVALID_TYPE;
    }

    *range_sector_op = s_flash_zone_data_range_table[flash_zone].data_range_sector_op;
    *range_sector_ed = s_flash_zone_data_range_table[flash_zone].data_range_sector_ed;
    *range_addr_op   = s_flash_zone_data_range_table[flash_zone].data_range_addr_op;
    *range_addr_ed   = s_flash_zone_data_range_table[flash_zone].data_range_addr_ed;
    *flash_zone_info = s_flash_zone_data_range_table[flash_zone].flash_zone_info;

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

int _prepare_operation(FlashZone       flash_zone,
                       uint32_t*       range_sector_op,
                       uint32_t*       range_sector_ed,
                       uint32_t*       range_addr_op,
                       uint32_t*       range_addr_ed,
                       FlashZoneInfo** flash_zone_info) {
    int8_t flash_ret = 0;
    int    ret       = 0;

    flash_ret = flash_init();
    if (flash_ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: flash_init failed");
        return GUNTER_FAILURE;
    }

    ret = _read_flash_zone_info(flash_zone);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs: _read_flash_zone_info[%d] failed with %d", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    ret = _get_flash_zone_range_info(flash_zone,
                                     range_sector_op,
                                     range_sector_ed,
                                     range_addr_op,
                                     range_addr_ed,
                                     flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs: _get_flash_zone_range_info[%d] failed with %d", flash_zone, ret);
        return ret;
    }

    return GUNTER_SUCCESS;
}

int _write_data_per_page(uint32_t* addr, uint32_t whole_data_len, uint8_t* data, uint16_t* wt_page,
                         uint32_t range_addr_op, uint32_t range_addr_ed) {
    int8_t   flash_ret           = 0;
    uint32_t data_len            = 0;
    uint32_t wt_len[2]           = {0}; // 第一部分和第二部分数据长度(第一部分为写入地址到分区结尾的部分，第二部分为分区开始，写入剩余数据)
    uint32_t len                 = 0;
    uint32_t data_offset         = 0;
    uint32_t wt_addr             = *addr;
    uint32_t wt_addr_page_offset = FS_PAGE_OFFSET(wt_addr);
    uint32_t page_left           = FS_PAGE_SIZE - wt_addr_page_offset;

    APP_LOG_DEBUG("page_left: %u", page_left);
    APP_LOG_DEBUG("whole_data_len: %u", whole_data_len);

    if (wt_addr + whole_data_len <= range_addr_ed) {
        wt_len[0] = whole_data_len;
        wt_len[1] = 0;
    } else {
        wt_len[0] = range_addr_ed - wt_addr;
        wt_len[1] = whole_data_len - wt_len[0];
    }

    for (int i = 0; i < 2; i++) {
        if (wt_len[i] == 0) {
            continue;
        }

        // 第二部分从分区开头开始写入
        if (i == 1 && wt_len[i] > 0) {
            wt_addr = range_addr_op;
        }

        if (i == 0) {
            // 第一部分需要考虑当前页剩余空间
            if (wt_len[i] > page_left) {
                len = page_left;
            } else {
                len = wt_len[i];
            }
        } else {
            // 第二部分直接写入整页
            if (wt_len[i] >= FS_PAGE_SIZE) {
                len = FS_PAGE_SIZE;
            } else {
                len = wt_len[i];
            }
        }

        while (wt_len[i]) {
            APP_LOG_DEBUG("wt_len[%d]: %u", i, wt_len[i]);

            flash_ret = flash_write_data(wt_addr, data + data_offset, len);
            if (flash_ret != GUNTER_SUCCESS) {
                APP_LOG_ERROR("ufs_wt: flash_write_data failed with %d", flash_ret);
                break;
            }

            APP_LOG_DEBUG("wtite addr: %u", wt_addr);
            data_stream_hex(data + data_offset, len);

            wt_addr += len;
            data_offset += len;
            wt_len[i] -= len;
            data_len += len;

            APP_LOG_DEBUG("len: %u", len);
            APP_LOG_DEBUG("data_len: %u", data_len);

            if (wt_len[i] >= FS_PAGE_SIZE) {
                len = FS_PAGE_SIZE;
            } else {
                len = wt_len[i];
            }

        }
    }

    *addr = wt_addr;

    return data_len;
}

int _read_data_per_page(uint32_t* addr, uint32_t whole_data_len, uint8_t* buffer,
                        uint32_t range_addr_op, uint32_t range_addr_ed) {
    int8_t   flash_ret           = 0;
    uint32_t data_len            = 0;
    uint32_t rd_len[2]           = {0}; // 第一部分和第二部分数据长度(第一部分为读取地址到分区结尾的部分，第二部分为分区开始，读取剩余数据)
    uint32_t len                 = 0;   // 读取当前页数据长度
    uint32_t buffer_offset       = 0;
    uint32_t rd_addr             = *addr; // 读取地址
    uint32_t rd_addr_page_offset = FS_PAGE_OFFSET(rd_addr);
    uint32_t page_left           = FS_PAGE_SIZE - rd_addr_page_offset;

    if (rd_addr + whole_data_len <= range_addr_ed) {
        rd_len[0] = whole_data_len;
        rd_len[1] = 0;
    } else {
        rd_len[0] = range_addr_ed - rd_addr;
        rd_len[1] = whole_data_len - rd_len[0];
    }

    for (int i = 0; i < 2; i++) {
        if (rd_len[i] == 0) {
            continue;
        }

        // 第二部分从分区开头开始读取
        if (i == 1 && rd_len[i] > 0) {
            rd_addr = range_addr_op;
        }

        if (i == 0) {
            // 第一部分需要考虑当前页剩余空间
            if (rd_len[i] > page_left) {
                len = page_left;
            } else {
                len = rd_len[i];
            }
        } else {
            // 第二部分直接读取整页
            if (rd_len[i] >= FS_PAGE_SIZE) {
                len = FS_PAGE_SIZE;
            } else {
                len = rd_len[i];
            }
        }

        while (rd_len[i]) {
            APP_LOG_DEBUG("rd_len[%d]: %u", i, rd_len[i]);

            flash_ret = flash_read_data(rd_addr, buffer + buffer_offset, len);
            if (flash_ret != GUNTER_SUCCESS) {
                APP_LOG_ERROR("ufs_rd: flash_read_data failed with %d", flash_ret);
                break;
            }

            APP_LOG_DEBUG("read addr: %u", rd_addr);
            data_stream_hex(buffer + buffer_offset, len);

            rd_addr += len;
            buffer_offset += len;
            rd_len[i] -= len;
            data_len += len;

            APP_LOG_DEBUG("len: %u", len);
            APP_LOG_DEBUG("data_len: %u", data_len);

            if (rd_len[i] >= FS_PAGE_SIZE) {
                len = FS_PAGE_SIZE;
            } else {
                len = rd_len[i];
            }

        }
    }

    *addr = rd_addr;

    return data_len;
}

uint8_t ufs_is_flash_used(void) {
    return s_flash_usage_status;
}

void ufs_set_flash_used(void) {
    s_flash_usage_status = FS_USAGE_STATUS_ON;
}

void ufs_set_flash_unused(void) {
    s_flash_usage_status = FS_USAGE_STATUS_OFF;
}

int ufs_init(void) {
    int    ret       = 0;
    int8_t flash_ret = 0;

    flash_ret = flash_init();
    if (flash_ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: flash_init failed");
        return GUNTER_FAILURE;
    }

    memset(&s_flash_zone_config, 0, sizeof(FlashZoneConfig));
    memset(&s_flash_zone_info_log, 0, sizeof(FlashZoneInfo));
    memset(&s_flash_zone_info_data, 0, sizeof(FlashZoneInfo));

    ret = _init_config();
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_init_config failed with %#.8x", ret);
        return GUNTER_FAILURE;
    }

    ret = _init_flash_zone_info(kFlashZoneLog);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_init_flash_zone_info log failed!");
        return GUNTER_FAILURE;
    }

    ret = _init_flash_zone_info(kFlashZoneData);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("_init_flash_zone_info data failed!");
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

int ufs_reinit(void) {
    int8_t falsh_ret = 0;

    falsh_ret = flash_init();
    if (falsh_ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: flash_init failed");
        return GUNTER_FAILURE;
    }

    falsh_ret = flash_erase_chip();
    if (falsh_ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("ufs: flash_erase_chip failed");
        return GUNTER_FAILURE;
    }

    APP_LOG_INFO("flash_erase_chip");

    return GUNTER_SUCCESS;
}

/**
 * @brief Writes data to a specified flash zone.
 *
 * This function writes data to the specified flash zone. It performs necessary checks and operations
 * to ensure that the data is written correctly within the flash zone.
 *
 * @param flash_zone The flash zone to write data to.
 * @param data The data to be written.
 * @param len The length of the data to be written.
 * @return Returns GUNTER_SUCCESS if the data is written successfully, otherwise returns an error code.
 */
int ufs_write_zone_data(FlashZone flash_zone, uint8_t* data, uint32_t len) {
    if (data == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    if (len > FS_WRITE_MAX_SIZE) {
        APP_LOG_ERROR("ufs_wt: len(%u) is bigger than 4096", len);
        return GUNTER_ERR_INVALID_PARAM;
    }

    int            ret             = 0;
    int8_t         flash_ret       = 0;
    uint32_t       range_sector_op = 0; // 分区可写入开始扇区
    uint32_t       range_sector_ed = 0; // 分区可写入结束扇区
    uint32_t       range_addr_op   = 0; // 分区可写入开始地址
    uint32_t       range_addr_ed   = 0; // 分区可写入结束地址
    FlashZoneInfo* flash_zone_info = NULL;

    ret = _prepare_operation(flash_zone,
                             &range_sector_op,
                             &range_sector_ed,
                             &range_addr_op,
                             &range_addr_ed,
                             &flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_wt: _prepare_operation[%d] failed with %d", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint32_t rd_addr     = flash_zone_info->rd_addr;     // 可读取地址
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

        wt_addr_ed = range_addr_op + (wt_addr_ed - range_addr_ed);

        er_sector = 1;
        rd_sector += 1;
        rd_addr = rd_sector * FS_SECTOR_SIZE;
    }

    if (er_sector) {
        wt_addr_ed -= 1;

        uint32_t wt_addr_ed_sector_base = FS_SECTOR_BASE(wt_addr_ed);
        flash_erase_sector(wt_addr_ed_sector_base);
    }

    ret = _write_data_per_page(&wt_addr, len, data, &wt_page, range_addr_op, range_addr_ed);
    if (ret != len) {
        APP_LOG_ERROR("ufs_wt: _write_data_per_page failed with %d", ret);
        return GUNTER_FAILURE;
    }

    flash_zone_info->rd_sector   = rd_sector;
    flash_zone_info->wt_sector   = wt_sector;
    flash_zone_info->wt_page     = wt_page;
    flash_zone_info->rd_addr     = rd_sector * FS_SECTOR_SIZE;
    flash_zone_info->wt_addr     = wt_addr;
    flash_zone_info->cycle_state = cycle_state;

    ret = _update_flash_zone_info(flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_wt: _update_flash_zone_info failed with %d", ret);
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

/**
 * Writes data to the specified flash zone.
 *
 * @param flash_zone The flash zone to write data to.
 * @param data       Pointer to the data to be written.
 * @param len        Length of the data to be written.
 *
 * @return           Returns GUNTER_SUCCESS if the data was written successfully,
 *                   otherwise returns an error code.
 */
int ufs_write_zone_data_mass(FlashZone flash_zone, uint8_t* data, uint32_t len) {
    if (data == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    int16_t  ret         = 0;
    uint32_t wt_len      = 0;
    uint32_t data_offset = 0;

    while (len) {
        if (len >= FS_WRITE_MAX_SIZE) {
            wt_len = FS_WRITE_MAX_SIZE;
        } else {
            wt_len = len;
        }

        ret = ufs_write_zone_data(flash_zone, data + data_offset, wt_len);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("ufs_wt_m: ufs_write_zone_data failed with %d", ret);
            return ret;
        }

        data_offset += wt_len;
        len -= wt_len;
    }

    return GUNTER_SUCCESS;
}

/**
 * Calculates the size of the readable data in the specified flash zone.
 *
 * @param flash_zone The flash zone to calculate the readable data size for.
 * @return The size of the readable data in the flash zone, or GUNTER_FAILURE if an error occurs.
 */
int ufs_get_readable_zone_data_size(FlashZone flash_zone) {
    int            ret             = 0;
    uint32_t       range_sector_op = 0; // 分区可写入开始扇区
    uint32_t       range_sector_ed = 0; // 分区可写入结束扇区
    uint32_t       range_addr_op   = 0; // 分区可写入开始地址
    uint32_t       range_addr_ed   = 0; // 分区可写入结束地址
    FlashZoneInfo* flash_zone_info = NULL;

    ret = _prepare_operation(flash_zone,
                             &range_sector_op,
                             &range_sector_ed,
                             &range_addr_op,
                             &range_addr_ed,
                             &flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_get_size: _prepare_operation[%d] failed with %d", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint32_t rd_addr        = flash_zone_info->rd_addr;
    uint32_t wt_addr        = flash_zone_info->wt_addr;
    uint32_t whole_data_len = 0;

    APP_LOG_DEBUG("rd_addr: %u", rd_addr);
    APP_LOG_DEBUG("wt_addr: %u", wt_addr);

    if (rd_addr > wt_addr) {
        whole_data_len = (range_addr_ed - rd_addr) + (wt_addr - range_addr_op);
    } else {
        whole_data_len = wt_addr - rd_addr;
    }

    return whole_data_len;
}

/**
 * Calculates the number of sectors required to store readable zone data in flash memory.
 *
 * @param flash_zone The flash zone where the data will be stored.
 * @param data_len   Pointer to the length of the data. If NULL, the function will calculate the length.
 *
 * @return The number of sectors required to store the data.
 */
int ufs_get_readable_zone_data_sector_cnt(FlashZone flash_zone, int* whole_data_len) {
    int cnt = 0;
    int len = 0;

    if (whole_data_len != NULL) {
        len = *whole_data_len;
    } else {
        len = ufs_get_readable_zone_data_size(flash_zone);
    }

    if ((len % FS_SECTOR_SIZE) > 0) {
        cnt = len / FS_SECTOR_SIZE + 1;
    } else {
        cnt = len / FS_SECTOR_SIZE;
    }

    return cnt;
}

/**
 * @brief Reads data from a specified flash zone.
 *
 * This function reads data from the specified flash zone and stores it in the provided buffer.
 *
 * If whole is true, the buffer size must be large enough to store the entire data in the flash.
 *
 * If pointer len is not NULL, and the whole is false, the function will read the data with the length specified.
 * If pointer len is NULL, and the whole is false, the function will read entire data.
 *
 * @param flash_zone The flash zone to read data from.
 * @param buffer     The buffer to store the read data.
 * @param whole      Flag indicating whether to read the entire data or a portion of it.
 * @param erase      Flag indicating whether to erase the read data after reading.
 *
 * @return The length of the read data, or an error code if the operation fails.
 */
int ufs_read_zone_data(FlashZone flash_zone, uint8_t* buffer, const uint32_t* len, bool whole, bool erase) {
    if (buffer == NULL) {
        return GUNTER_ERR_NULL_POINTER;
    }

    int            ret             = 0;
    uint32_t       range_sector_op = 0; // 分区可写入开始扇区
    uint32_t       range_sector_ed = 0; // 分区可写入结束扇区
    uint32_t       range_addr_op   = 0; // 分区可写入开始地址
    uint32_t       range_addr_ed   = 0; // 分区可写入结束地址
    FlashZoneInfo* flash_zone_info = NULL;

    ret = _prepare_operation(flash_zone,
                             &range_sector_op,
                             &range_sector_ed,
                             &range_addr_op,
                             &range_addr_ed,
                             &flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_rd_m: _prepare_operation[%d] failed with %d", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint32_t rd_addr             = flash_zone_info->rd_addr;     // 读取地址
    uint32_t wt_addr             = flash_zone_info->wt_addr;     // 写入地址
    uint32_t cycle_state         = flash_zone_info->cycle_state; // 写入地址
    uint32_t rd_addr_sector_base = FS_SECTOR_BASE(rd_addr);      // 读取地址的扇区基地址
    uint32_t wt_addr_sector_base = FS_SECTOR_BASE(wt_addr);      // 写入地址的扇区基地址
    uint32_t addr                = 0;
    uint32_t data_len            = 0; // 读取内容长度

    if (len != NULL) {
        data_len = *len;
    } else {
        if (rd_addr > wt_addr) {
            data_len = (range_addr_ed - rd_addr) + (wt_addr - range_addr_op);
        } else {
            data_len = wt_addr - rd_addr;
        }
    }

    if (!whole) {
        ret = _read_data_per_page(&rd_addr, data_len, buffer, range_addr_op, range_addr_ed);
        if (ret != data_len) {
            APP_LOG_ERROR("ufs_rd: _read_data_per_page failed with %d", ret);
            return GUNTER_FAILURE;
        }

    } else {
        // * 读取整个数据
        if (rd_addr > wt_addr) {
            data_len = (range_addr_ed - rd_addr) + (wt_addr - range_addr_op);
        } else {
            data_len = wt_addr - rd_addr;
        }

        if (*len != data_len) {
            APP_LOG_ERROR("ufs_rd: len(%u) is not equal to data_len(%u)", *len, data_len);
            return GUNTER_ERR_INVALID_PARAM;
        }

        ret = _read_data_per_page(&rd_addr, data_len, buffer, range_addr_op, range_addr_ed);
        if (ret != data_len) {
            APP_LOG_ERROR("ufs_rd: _read_data_per_page failed with %d", ret);
            return GUNTER_FAILURE;
        }
    }

    if (erase) {
        flash_zone_info->rd_addr     = rd_addr;
        flash_zone_info->rd_sector   = FS_SECTOR_BASE(rd_addr);
        flash_zone_info->cycle_state = (rd_addr < wt_addr) ? FS_ZONE_CYCLE_OFF : FS_ZONE_CYCLE_ON;

        ret = _update_flash_zone_info(flash_zone_info);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("ufs_rd: _update_flash_zone_info failed with %d", ret);
            return GUNTER_FAILURE;
        }
    }

    return data_len;
}

/**
 * Erases the data in a specified flash zone.
 *
 * @param flash_zone The flash zone to erase.
 * @return Returns GUNTER_SUCCESS if the flash zone is successfully erased, GUNTER_FAILURE otherwise.
 */
int ufs_erase_zone_data(FlashZone flash_zone) {
    int            ret             = 0;
    int8_t         flash_ret       = 0;
    uint32_t       range_sector_op = 0; // 分区可写入开始扇区
    uint32_t       range_sector_ed = 0; // 分区可写入结束扇区
    uint32_t       range_addr_op   = 0; // 分区可写入开始地址
    uint32_t       range_addr_ed   = 0; // 分区可写入结束地址
    FlashZoneInfo* flash_zone_info = NULL;

    ret = _prepare_operation(flash_zone,
                             &range_sector_op,
                             &range_sector_ed,
                             &range_addr_op,
                             &range_addr_ed,
                             &flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_er: _prepare_operation[%d] failed with %d", flash_zone, ret);
        return GUNTER_FAILURE;
    }

    uint8_t sector_cnt = range_sector_ed - range_sector_op;

    for (int i = 0; i < sector_cnt; i++) {
        flash_ret = flash_erase_sector(range_addr_op + i * FS_SECTOR_SIZE);
        if (flash_ret != FLASH_SUCCESS) {
            APP_LOG_ERROR("ufs: flash_erase_sector failed");
            return GUNTER_FAILURE;
        }
    }

    flash_zone_info->rd_sector   = range_sector_op;
    flash_zone_info->wt_sector   = range_sector_op;
    flash_zone_info->wt_page     = 0;
    flash_zone_info->rd_addr     = range_addr_op;
    flash_zone_info->wt_addr     = range_addr_op;
    flash_zone_info->cycle_state = FS_ZONE_CYCLE_OFF;

    ret = _update_flash_zone_info(flash_zone_info);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs: _update_flash_zone_info failed with %d", ret);
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}

int ufs_test(void) {
    int ret = 0;

    uint8_t buffer[4096 * 2];
    memset(buffer, 0, sizeof(buffer));

    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = i & 0xff;
    }

    APP_LOG_INFO("===== write data ======");
    ret = ufs_write_zone_data_mass(kFlashZoneData, buffer, sizeof(buffer));
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("ufs_test: ufs_write_zone_data_mass failed");
        return GUNTER_FAILURE;
    }

    memset(buffer, 0, sizeof(buffer));

    uint32_t len = ufs_get_readable_zone_data_size(kFlashZoneData);
    APP_LOG_INFO("ufs_test: len = %u", len);

    APP_LOG_INFO("===== read data ======");
    ret = ufs_read_zone_data(kFlashZoneData, buffer, &len, true, false);
    if (ret != len) {
        APP_LOG_ERROR("ufs_test: ufs_read_zone_data failed");
        return GUNTER_FAILURE;
    }

    return GUNTER_SUCCESS;
}
