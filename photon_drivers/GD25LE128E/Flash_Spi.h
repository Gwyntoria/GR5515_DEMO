#ifndef _FLASH_SPI_H_
#define _FLASH_SPI_H_

#include "stdint.h"
// GD25LE128E

#define GD25LE128E_OP_ADDR ((uint32_t)0x00000000) // opening address
#define GD25LE128E_ED_ADDR ((uint32_t)0x00FFFFFF) // ending address

#define GD25LE128E_TOTAL_SIZE     ((uint32_t)0x01000000) // Total size: 16 MB
#define GD25LE128E_BLOCK_64K_SIZE ((uint32_t)0x00010000) // Block size: 64KB
#define GD25LE128E_BLOCK_32K_SIZE ((uint32_t)0x00008000) // Block size: 32KB
#define GD25LE128E_SECTOR_SIZE    ((uint32_t)0x00001000) // Sector size: 4KB
#define GD25LE128E_PAGE_SIZE      ((uint32_t)0x00000100) // Page size: 256B

#define GD25LE128E_BLOCK_CNT  (GD25LE128E_TOTAL_SIZE / GD25LE128E_BLOCK_64K_SIZE) // Block num: 256
#define GD25LE128E_SECTOR_CNT (GD25LE128E_TOTAL_SIZE / GD25LE128E_SECTOR_SIZE)    // Sector num: 4096

#define GD25LE128E_SECTOR_MASK         (GD25LE128E_SECTOR_SIZE - 1)       // 扇区掩码
#define GD25LE128E_SECTOR_BASE(addr)   (addr & (~GD25LE128E_SECTOR_MASK)) // 扇区的基地址
#define GD25LE128E_SECTOR_OFFSET(addr) (addr & GD25LE128E_SECTOR_MASK)    // 扇区内的偏移

#define GD25LE128E_BLOCK_64K_MASK         (GD25LE128E_BLOCK_64K_SIZE - 1)         // 64k块区掩码
#define GD25LE128E_BLOCK_64K_BASE(addr)   (addr & (~(GD25LE128E_BLOCK_64K_MASK))) // 64k块区的基地址
#define GD25LE128E_BLOCK_64K_OFFSET(addr) (addr & GD25LE128E_BLOCK_64K_MASK)      // 64k块区内的偏移

#define GD25LE128E_BLOCK_32K_MASK         (GD25LE128E_BLOCK_32K_SIZE - 1)         // 32k块区掩码
#define GD25LE128E_BLOCK_32K_BASE(addr)   (addr & (~(GD25LE128E_BLOCK_32K_MASK))) // 32k块区的基地址
#define GD25LE128E_BLOCK_32K_OFFSET(addr) (addr & GD25LE128E_BLOCK_32K_MASK)      // 32k块区内的偏移

#define ELECTRONIC_ID 0x17
#define RESID0        0xc817
#define RESID1        0x17c8

#define GD25LE128E_WRDI 0x04 // Write Disable
#define GD25LE128E_WREN 0x06 // Write Enable

#define GD25LE128E_RDSR_L 0x05 // Read Status Register Low 8bit
#define GD25LE128E_RDSR_M 0x35 // Read Status Register Medium 8bit
#define GD25LE128E_RDSR_H 0x15 // Read Status Register High 8bit

#define GD25LE128E_WRSR_L 0x01 // Write Status Register Low 8bit
#define GD25LE128E_WRSR_H 0x11 // Write Status Register High 8bit

#define GD25LE128E_WEVSR 0x50 // Write Enable for Volatile Status Register

#define GD25LE128E_READ  0x03 // Read Data Bytes, need add 24bit address
#define GD25LE128E_FREAD 0x0B // Read Data Bytes at Higher Speed

#define GD25LE128E_REMS 0x90 // Read Manufacture ID/ Device ID
#define GD25LE128E_MID  0xC8 // Manufacture ID
#define GD25LE128E_DID  0x17 // Device ID

#define GD25LE128E_RDID 0x9F
#define GD25LE128E_ID   0xC86018

#define GD25LE128E_SE   0x20 // Sector Erase
#define GD25LE128E_BE64 0xD8 // 64KB Block Erase
#define GD25LE128E_CE   0x60 // Chip Erase

#define GD25LE128E_PP 0x02

#define FLASH_SUCCESS     0x00
#define FLASH_FAILURE     0xFF
#define FLASH_STATUS_FREE 0x00
#define FLASH_STATUS_BUSY 0xFF
#define FLASH_SIZE_ERROR  0x01
#define FLASH_DATA_EMPTY  0x02
#define FLASH_INIT_ERROR  0x03
#define FLASH_ID_ERROR    0x04
#define FLASH_INPUT_ERROR 0x05

/**
 * @brief Flash spi Initialization and check Flash ID precise
 *
 * @return Flash Configuar status
 */
int8_t flash_init(void);

/**
 * @brief Get Flash GD25LE128E Write statu
 *
 * @return If Flash write and other status
 */
int8_t flash_read_status(void);

/**
 * @brief Flash GD25LE128E Chip Erase
 *
 * @return return success
 */
int8_t flash_erase_chip(void);

/**
 * @brief Flash GD25LE128E Sector Erase
 *
 * @param address The starting address of the sector that needs to be erased (24 bits)
 * @return return success
 */
int8_t flash_erase_sector(uint32_t address);

/**
 * @brief Flash GD25LE128E Block Erase
 *
 * @param address The starting address of the block that needs to be erased (24 bits)
 * @return return success
 */
int8_t flash_erase_block_64k(uint32_t address);

/**
 * @brief Flash GD25LE128E write data to input address
 *
 * @param[in] address The starting address of the data that needs to be written (24 bits)
 * @param[in] data The buffer that holds the data to be written (<= 256 Bytes)
 * @param[in] data_size The data size (<= 256 Bytes)
 * @return if input pointer is null, return FLASH_DATA_EMPTY
 */
int8_t flash_write_data(uint32_t address, uint8_t* data, uint16_t data_size);

/**
 * @brief Flash GD25LE128E Read data from address
 *
 * @param[in] address The starting address of the data that needs to be read (24 bits)
 * @param[out] data The buffer that stores the data to be read (<= 256 Bytes)
 * @param[in] data_size The data size (<= 256 Bytes)
 * @return if input pointer is null, return FLASH_DATA_EMPTY
 *         if data_size > 256 ,return FLASH_SIZE_ERROR
 */
int8_t flash_read_data(uint32_t address, uint8_t* data, uint16_t data_size);

/**
 * @brief Flash GD25LE128E update sector data
 *
 * @param address The starting address of the data that needs to be updated (24 bits)
 * @param data The buffer that holds the data to be written (<= 256 Bytes)
 * @param data_size The data size (<= 256 Bytes)
 * @return if input pointer is null, return FLASH_DATA_EMPTY
 */
int8_t flash_update_sector_data(uint32_t address, uint8_t* data, uint16_t data_size);

/**
 * @brief test demo
 *
 */
int flash_func_test();

#endif
