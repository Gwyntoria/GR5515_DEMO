#ifndef USER_FILE_SYS_H
#define USER_FILE_SYS_H

#include <stdbool.h>

#include "GD25LE128E/Flash_Spi.h"

#define FS_SECTOR_OP 0
#define FS_SECTOR_ED (GD25LE128E_SECTOR_CNT - 1)

// config [0, 1) 4K
#define FS_SECTOR_OP_CFG 0
#define FS_SECTOR_ED_CFG 1
// log [2, 255) 1M
#define FS_SECTOR_OP_LOG 2
#define FS_SECTOR_ED_LOG 255
// data [256, 1280) 4M
#define FS_SECTOR_OP_DAT 256
#define FS_SECTOR_ED_DAT 1280

#define FS_BLOCK_SIZE  GD25LE128E_BLOCK_64K_SIZE // 64 KB
#define FS_SECTOR_SIZE GD25LE128E_SECTOR_SIZE    // 4 KB
#define FS_PAGE_SIZE   GD25LE128E_PAGE_SIZE      // 256 B

#define FS_PAGE_BASE(addr)   GD25LE128E_PAGE_BASE(addr)   // 页区的基地址
#define FS_PAGE_OFFSET(addr) GD25LE128E_PAGE_OFFSET(addr) // 页区内的偏移

#define FS_SECTOR_BASE(addr)   GD25LE128E_SECTOR_BASE(addr)   // 扇区的基地址
#define FS_SECTOR_OFFSET(addr) GD25LE128E_SECTOR_OFFSET(addr) // 扇区内的偏移

#define FS_BLOCK_64K_BASE(addr)   GD25LE128E_BLOCK_64K_BASE(addr)   // 块区的基地址
#define FS_BLOCK_64K_OFFSET(addr) GD25LE128E_BLOCK_64K_OFFSET(addr) // 块区内的偏移

#define FS_ADDR_OP_CFG (GD25LE128E_SECTOR_SIZE * FS_SECTOR_OP_CFG)
#define FS_ADDR_ED_CFG (GD25LE128E_SECTOR_SIZE * FS_SECTOR_ED_CFG)

#define FS_ADDR_OP_LOG (GD25LE128E_SECTOR_SIZE * FS_SECTOR_OP_LOG)
#define FS_ADDR_ED_LOG (GD25LE128E_SECTOR_SIZE * FS_SECTOR_ED_LOG)

#define FS_ADDR_OP_DTA (GD25LE128E_SECTOR_SIZE * FS_SECTOR_OP_DAT)
#define FS_ADDR_ED_DTA (GD25LE128E_SECTOR_SIZE * FS_SECTOR_ED_DAT)

#define FS_WRITE_MAX_SIZE GD25LE128E_SECTOR_SIZE // 4KB

#define FS_CFG_DATA_LEN  12
#define FS_ZONE_INFO_LEN 23

#define FS_ZONE_CYCLE_OFF 0
#define FS_ZONE_CYCLE_ON  1

typedef enum SectorStatus {
    SS_UNINIT  = 0xFF,
    SS_EMPTY   = 0x1B,
    SS_DATA    = 0x1C,
    SS_BAD     = 0x1D,
    SS_INVALID = 0x1E,
} SectorStatus;

typedef enum FlashZone {
    kFlashZone_Config,
    kFlashZone_Log,
    kFlashZone_SensorData,
    kFlashZone_Max
} FlashZone;

typedef struct FlashZoneConfig {
    uint8_t  status;    // @ref PageStatus
    uint16_t pack_len;  // config信息长度
    uint32_t wt_addr;   // config 所在扇区可继续写入的地址（reserved）
    uint8_t  ver_major; // 固件版本号
    uint8_t  ver_minor;
    uint8_t  ver_build;
    uint8_t  ver_exten;
    uint8_t  activation; // 激活状态

} FlashZoneConfig;

typedef struct FlashZoneInfo {
    uint8_t  status;      // @ref PageStatus
    uint8_t  type;        // @ref FlashZone
    uint16_t pack_len;    // 分区信息数据长度
    uint16_t sector_op;   // 分区起始扇区
    uint16_t sector_ed;   // 分区结束扇区
    uint16_t rd_sector;   // 读取扇区标志
    uint16_t wt_sector;   // 写入扇区标志
    uint16_t wt_page;     // @ref GD25LE128E_PAGE_CNT_PER_SECTOR
    uint32_t rd_addr;     // 起始可读地址
    uint32_t wt_addr;     // 起始写入地址
    uint8_t  cycle_state; // 分区循环写入状态

} FlashZoneInfo;

typedef struct FlashZoneDataRange {
    FlashZoneInfo* flash_zone_info;      // 分区信息
    uint32_t       data_range_sector_op; // 分区可写入开始扇区
    uint32_t       data_range_sector_ed; // 分区可写入结束扇区
    uint32_t       data_range_addr_op;   // 分区可写入开始地址
    uint32_t       data_range_addr_ed;   // 分区可写入结束地址
} FlashZoneDataRange;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int ufs_init(void);
int ufs_reinit(void);

int ufs_write_data(FlashZone flash_zone, uint8_t* data, uint32_t len);
int ufs_write_data_mass(FlashZone flash_zone, uint8_t* data, uint32_t len);
int ufs_read_data(FlashZone flash_zone, uint8_t* buffer, bool whole);
int ufs_get_zone_data_sector_cnt(FlashZone flash_zone);

int ufs_test(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
