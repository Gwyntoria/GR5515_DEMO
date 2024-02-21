/**
 * @file Flash_Spi.c 
 * @author Gwyntoria (karlmfloating@gmail.com)
 * @brief flash 读写
 * @version 0.1
 * @date 2024-01-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Flash_Spi.h"
#include "app_spi.h"

static void flash_spi_callback(app_spi_evt_t *p_evt)
{
    if (p_evt->type == APP_SPI_EVT_TX_CPLT)
    {
        printf("p_evt->type == APP_SPI_EVT_TX_CPLT\r\n");
    }
    if (p_evt->type == APP_SPI_EVT_RX_DATA)
    {
        printf("p_evt->type == APP_SPI_EVT_RX_DATA\r\n");
    }
    if (p_evt->type == APP_SPI_EVT_ERROR)
    {
        printf("p_evt->type == APP_SPI_EVT_ERROR\r\n");
    }
    if (p_evt->type == APP_SPI_EVT_TX_RX)
    {
        printf("p_evt->type == APP_SPI_EVT_TX_RX\r\n");
    }
}

int8_t flash_config(void)
{
    uint8_t  Flash_id[3] = {0};
    uint32_t Flashid     = 0;
    uint16_t ret         = 0;
    // set up struct
    app_spi_params_t flash_struct;
    flash_struct.id = APP_SPI_ID_MASTER; // set up spi id master
    // CS configuration
    flash_struct.pin_cfg.cs.type   = APP_IO_TYPE_NORMAL;
    flash_struct.pin_cfg.cs.mux    = APP_IO_MUX_0;
    flash_struct.pin_cfg.cs.pin    = APP_IO_PIN_17;
    flash_struct.pin_cfg.cs.pull   = APP_IO_NOPULL;
    flash_struct.pin_cfg.cs.enable = APP_SPI_PIN_ENABLE;
    // CLK configuration
    flash_struct.pin_cfg.clk.type   = APP_IO_TYPE_NORMAL;
    flash_struct.pin_cfg.clk.mux    = APP_IO_MUX_0;
    flash_struct.pin_cfg.clk.pin    = APP_IO_PIN_24;
    flash_struct.pin_cfg.clk.pull   = APP_IO_NOPULL;
    flash_struct.pin_cfg.clk.enable = APP_SPI_PIN_ENABLE;
    // MOSI configuration
    flash_struct.pin_cfg.mosi.type   = APP_IO_TYPE_NORMAL;
    flash_struct.pin_cfg.mosi.mux    = APP_IO_MUX_0;
    flash_struct.pin_cfg.mosi.pin    = APP_IO_PIN_25;
    flash_struct.pin_cfg.mosi.pull   = APP_IO_NOPULL;
    flash_struct.pin_cfg.mosi.enable = APP_SPI_PIN_ENABLE;
    // MISO configuration
    flash_struct.pin_cfg.miso.type   = APP_IO_TYPE_NORMAL;
    flash_struct.pin_cfg.miso.mux    = APP_IO_MUX_0;
    flash_struct.pin_cfg.miso.pin    = APP_IO_PIN_16;
    flash_struct.pin_cfg.miso.pull   = APP_IO_NOPULL;
    flash_struct.pin_cfg.miso.enable = APP_SPI_PIN_ENABLE;
    // set use_mode.type = interrupt
    flash_struct.use_mode.type = APP_SPI_TYPE_INTERRUPT;

    flash_struct.init.data_size          = SPI_DATASIZE_8BIT;           // datasize = 8bit
    flash_struct.init.clock_polarity     = LL_SSI_SCPOL_LOW;            // flash supported spi mode 0 and 3
    flash_struct.init.clock_phase        = SPI_PHASE_1EDGE;             // pol and pha both 0 or 1
    flash_struct.init.baudrate_prescaler = (SystemCoreClock / 4000000); // speed = 400K
    flash_struct.init.ti_mode            = SPI_TIMODE_DISABLE;          // DISABLE ti mode
    flash_struct.init.slave_select       = SPI_SLAVE_SELECT_0;          // set up cs0

    ret = app_spi_init(&flash_struct, flash_spi_callback);
    if (ret != 0)
	{
		printf("FLASH_INIT_ERROR!\r\n");
        return FLASH_INIT_ERROR;
	}
    flash_read_rfid(Flash_id);
    Flashid = Flash_id[0];
    Flashid = Flashid << 8 | Flash_id[1];
    Flashid = Flashid << 8 | Flash_id[2];
    if (Flashid == GD25LE128E_ID)
    {
		printf("FLASH_SUCCESS!\r\n");
        return FLASH_SUCCESS;
        
    }
	printf("FLASH_ID_ERROR!\r\n");
    return FLASH_ID_ERROR;
}

/**
 * @brief 
 * 
 * @param flash_id 
 * @return int8_t 
 */
int8_t flash_read_rfid(uint8_t *flash_id) // flash_id must > 3byte
{
    uint8_t Send_Data   = GD25LE128E_RDID; // 0x9F
    uint8_t Get_Data[4] = {0};

    app_spi_transmit_receive_async(APP_SPI_ID_MASTER, &Send_Data, Get_Data, 4);
    // printf("flash_read_rfid: Get_Data[0] = 0x%x , Get_Data[1] = 0x%x , Get_Data[2] = 0x%x , Get_Data[3] =
    // 0x%x\r\n",Get_Data[0],Get_Data[1],Get_Data[2],Get_Data[3]);
    if (flash_id != NULL)
    {
        for (int i = 0; i < 3; i++)
        {
            flash_id[i] = Get_Data[i + 1];
        }
        return FLASH_SUCCESS;
    }

    return FLASH_INPUT_ERROR;
}

int8_t flash_read_status(void)
{
    uint8_t Send_Reg    = GD25LE128E_RDSR_L;
    uint8_t Get_Arry[2] = {0};

    app_spi_transmit_receive_sync(APP_SPI_ID_MASTER, &Send_Reg, Get_Arry, 2, 0x1000);
    // printf("GD25LE128E_RDSR_L = 0x%x \r\n",Get_Arry[1]);
    if ((Get_Arry[1] & 0xFC) == 0xFF)
    {
        return FLASH_STATUS_BUSY;
    }
    else
        return FLASH_STATUS_FREE;
}

int8_t flash_chip_erase(void)
{
    uint8_t Send_Cmd = GD25LE128E_WREN;
    uint8_t ret      = 0;

    app_spi_transmit_sync(APP_SPI_ID_MASTER, &Send_Cmd, 1, 0x1000);

    Send_Cmd = GD25LE128E_CE;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, &Send_Cmd, 1, 0x1000);

    ret = flash_read_status();
    while (ret)
    {
        delay_ms(1000);
        ret = flash_read_status();
    }

    return FLASH_SUCCESS;
}

int8_t flash_sector_erase(uint16_t Sector)
{
    uint8_t Send_Cmd[4] = {0};
    int     ret         = 0;

    Send_Cmd[0] = GD25LE128E_WREN;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, Send_Cmd, 1, 0x1000);

    Send_Cmd[0] = GD25LE128E_SE;
    Send_Cmd[1] = (uint8_t)(Sector >> 4);
    Send_Cmd[2] = (uint8_t)(Sector << 4);
    Send_Cmd[3] = 0;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, Send_Cmd, 4, 0x1000);

    ret = flash_read_status();
    while (ret)
    {
        delay_ms(30);
        ret = flash_read_status();
    }

    return FLASH_SUCCESS;
}

int8_t flash_block_erase(uint8_t Block)
{
    uint8_t Send_Cmd[4] = {0};
    int     ret         = 0;

    Send_Cmd[0] = GD25LE128E_WREN;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, Send_Cmd, 1, 0x1000);

    Send_Cmd[0] = GD25LE128E_BE64;
    Send_Cmd[1] = Block;
    Send_Cmd[2] = 0;
    Send_Cmd[3] = 0;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, Send_Cmd, 4, 0x1000);

    ret = flash_read_status();
    while (ret)
    {
        delay_ms(100);
        ret = flash_read_status();
    }

    return FLASH_SUCCESS;
}

int8_t flash_write_data(uint32_t address, uint8_t *data, uint16_t datasize)
{
    if (data == NULL)
        return FLASH_DATA_EMPTY;

    uint8_t  Send_Reg[260];
    uint16_t i = 0;

    Send_Reg[0] = GD25LE128E_WREN;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, Send_Reg, 1, 0x1000);

    Send_Reg[0] = GD25LE128E_PP;
    Send_Reg[1] = (uint8_t)(address >> 16);
    Send_Reg[2] = (uint8_t)(address >> 8);
    Send_Reg[3] = (uint8_t)address;
    for (i = 0; i < datasize; i++)
    {
        Send_Reg[i + 4] = data[i];
    }
    app_spi_transmit_sync(APP_SPI_ID_MASTER, Send_Reg, datasize + 4, 0x1000);

    i = flash_read_status();
    while (i)
    {
        delay_ms(30);
        i = flash_read_status();
    }
    return FLASH_SUCCESS;
}

int8_t flash_read_data(uint32_t address, uint8_t *data, uint16_t datasize)
{
    if (data == NULL)
        return FLASH_DATA_EMPTY;

    uint8_t  Send_Reg[4] = {0};
    uint8_t  Get_Data[260];
    uint16_t i = 0;

    if (datasize > 256)
        return FLASH_SIZE_ERROR;

    address     = address & 0xFFFFFF;
    Send_Reg[0] = GD25LE128E_READ;
    Send_Reg[1] = (uint8_t)(address >> 16);
    Send_Reg[2] = (uint8_t)(address >> 8);
    Send_Reg[3] = (uint8_t)address;

    app_spi_transmit_receive_sync(APP_SPI_ID_MASTER, Send_Reg, Get_Data, datasize + 4, 0x1000);
    printf("Flash_Read_Address = ");
    for (i = 0; i < datasize; i++)
    {
        data[i] = Get_Data[4 + i];
        printf("data[%d] = 0x%02x  ", i, Get_Data[4 + i]);
    }
    printf("\r\n");
    return FLASH_SUCCESS;
}

// void FlashReadWrite_Test(void) // Flash Write and Read operation
// {
// 	uint8_t Send_Reg = 0;
// 	uint8_t Send_Arry[16] = {0};
// 	uint8_t Get_Arry[16] = {0};
// 	uint8_t Read_cmd[4] = {0};
// 	uint32_t flash_add = 0x00000000;
// 	int i = 0;
// 	Send_Reg = GD25LE128E_WREN;
// 	app_spi_transmit_sync(APP_SPI_ID_MASTER, &Send_Reg, 1, 0x1000);

// 	Send_Arry[0] = GD25LE128E_PP;
// 	Send_Arry[1] = (uint8_t)flash_add >> 16;
// 	Send_Arry[2] = (uint8_t)flash_add >> 8;
// 	Send_Arry[3] = (uint8_t)flash_add;
// 	for (i = 4; i < 16; i++)
// 	{
// 		Send_Arry[i] = i;
// 	}
// 	app_spi_transmit_sync(APP_SPI_ID_MASTER, Send_Arry, 16, 0x1000);

// 	i = flash_read_status();
// 	while (i)
// 	{
// 		delay_ms(30);
// 		i = flash_read_status();
// 	}

// 	Read_cmd[0] = GD25LE128E_READ;
// 	Read_cmd[1] = (uint8_t)flash_add >> 16;
// 	Read_cmd[2] = (uint8_t)flash_add >> 8;
// 	Read_cmd[3] = (uint8_t)flash_add;
// 	app_spi_transmit_receive_sync(APP_SPI_ID_MASTER, Read_cmd, Get_Arry, 10, 0x1000);
// 	printf("GD25LE128E_READ1 = ");
// 	for (i = 4; i < 10; i++)
// 	{
// 		printf("Get_Arry[%d] = 0x%02x ", i, Get_Arry[i]);
// 	}
// 	printf("\r\n");

// 	Read_cmd[0] = GD25LE128E_READ;
// 	Read_cmd[1] = (uint8_t)flash_add >> 16;
// 	Read_cmd[2] = (uint8_t)flash_add >> 8;
// 	Read_cmd[3] = 0x01;
// 	app_spi_transmit_receive_sync(APP_SPI_ID_MASTER, Read_cmd, Get_Arry, 9, 0x1000);
// 	printf("GD25LE128E_READ2 = ");
// 	for (i = 4; i < 9; i++)
// 	{
// 		printf("Get_Arry[%d] = 0x%02x ", i, Get_Arry[i]);
// 	}
// 	printf("\r\n");

// 	flash_sector_erase(0);

// 	Read_cmd[0] = GD25LE128E_READ;
// 	Read_cmd[1] = (uint8_t)flash_add >> 16;
// 	Read_cmd[2] = (uint8_t)flash_add >> 8;
// 	Read_cmd[3] = (uint8_t)flash_add;
// 	app_spi_transmit_receive_sync(APP_SPI_ID_MASTER, Read_cmd, Get_Arry, 10, 0x1000);
// 	printf("GD25LE128E_READ3 = ");
// 	for (i = 4; i < 10; i++)
// 	{
// 		printf("Get_Arry[%d] = 0x%02x ", i, Get_Arry[i]);
// 	}
// 	printf("\r\n");
// }
