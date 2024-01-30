#ifndef _FLASH_SPI_H_
#define _FLASH_SPI_H_

#include "stdint.h"
// GD25LE128E

#define FlashSize       0x1000000                  // 16 MB
#define Block_Offset    0x10000                    // 64K Block size
#define Block32K_Offset 0x8000                     // 32K Block size
#define Sector_Offset   0x1000                     // 4K Sector size
#define Page_Offset     0x0100                     // 256 Byte Page size
#define Block_Num       (FlashSize / Block_Offset) // 256page

#define ElectronicID 0x17
#define RESID0       0xc817
#define RESID1       0x17c8

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

#define Flash_Success    0x00
#define Flash_StatuFree  0x00
#define Flash_SizeError  0x01
#define Flash_DataEmpty  0x02
#define Flash_InitError  0x03
#define Flash_IdError    0x04
#define Flash_InputError 0x05
#define Flash_StatuBusy  0x06

/**
 ****************************************************************************************
 * @brief 		Flash spi Initialization and check Flash ID precise
 * @Function   	int8_t Flash_Config(void)
 *
 * @param[in]  	None
 *
 * @return 		int8_t              : Flash Configuar status
 ****************************************************************************************
 */
int8_t Flash_Config(void);

/**
 ****************************************************************************************
 * @brief 		Get Flash GD25LE128E ID
 * @Function   	int8_t Read_RDID(uint8_t *flash_id);
 *
 * @param[in]  	uint8_t *flash_id	: input Point must > 3 Byte
 *
 * @return 		int8_t				: get Point is status
 ****************************************************************************************
 */
int8_t Read_RDID(uint8_t *flash_id);

/**
 ****************************************************************************************
 * @brief 		Get Flash GD25LE128E Write statu
 * @Function    int8_t Flash_Statu_W(void);
 *
 * @param[in]  	None
 *
 * @return 		int8_t				: If Flash write and other status
 ****************************************************************************************
 */
int8_t Flash_Statu_W(void);

/**
 ****************************************************************************************
 * @brief 		Flash GD25LE128E Chip Erase
 * @Function    int8_t Flash_Chip_Erase(void);
 *
 * @param[in]  	None
 *
 * @return 		int8_t              : return success
 ****************************************************************************************
 */
int8_t Flash_Chip_Erase(void);

/**
 ****************************************************************************************
 * @brief 		Flash GD25LE128E Sector Erase
 * @Function    int8_t Flash_Sector_Erase(uint16_t page);
 *
 * @param[in]  	uint16_t page 		: Enter Page and Erase this Page Data (0 - 4095)
 *
 * @return 		int8_t              : return success
 ****************************************************************************************
 */
int8_t Flash_Sector_Erase(uint16_t page);

/**
 ****************************************************************************************
 * @brief 		Flash GD25LE128E Block Erase
 * @Function    int8_t Flash_Block_Erase(uint8_t Block);
 *
 * @param[in]  	uint16_t Block 		: Enter Block and Erase this Block Data (0 - 127)
 *
 * @return 		int8_t              : return success
 ****************************************************************************************
 */
int8_t Flash_Block_Erase(uint8_t Block);

/**
 ****************************************************************************************
 * @brief 		Flash GD25LE128E write data to input address
 * @Function    int8_t Flash_Write_Data(uint32_t address, uint8_t *data, uint16_t datasize);
 *
 * @param[in]  	uint32_t address 	: Enter want memory data to address (24 bit)
 *              uint8_t *data       : Enter Data to memory              (<= 256Byte)
 *              uint16_t datasize   : Enter DataSize                    (<= 256)
 *
 * @return 		int8_t              : if input point is null, return Flash_DataEmpty
 ****************************************************************************************
 */
int8_t Flash_Write_Data(uint32_t address, uint8_t *data, uint16_t datasize);

/**
 ****************************************************************************************
 * @brief 		Flash GD25LE128E Read data from address
 * @Function    int8_t Flash_Write_Data(uint32_t address, uint8_t *data, uint16_t datasize);
 *
 * @param[in]  	uint32_t address 	: read data address                 (24 bit)
 *              uint8_t *data       : read data                         (<= 256Byte)
 *              uint16_t datasize   : Enter DataSize                    (<= 256)
 *
 * @return 		int8_t              : if input point is null, return Flash_DataEmpty
 *                                    if DataSize > 256 ,return Flash_SizeError
 ****************************************************************************************
 */
int8_t Flash_Read_Data(uint32_t address, uint8_t *data, uint16_t datasize);

#endif
