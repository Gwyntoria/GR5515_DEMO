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
#include "gr55xx_sys.h"

#include "app_spi.h"
#include "user_common.h"

#define FLASH_OPERATE_LOCK()   GLOBAL_EXCEPTION_DISABLE()
#define FLASH_OPERATE_UNLOCK() GLOBAL_EXCEPTION_ENABLE()

static void flash_spi_callback(app_spi_evt_t* p_evt) {
    switch (p_evt->type) {
        case APP_SPI_EVT_TX_CPLT:
            // APP_LOG_INFO("flash_spi_callback APP_SPI_EVT_TX_CPLT");
            break;

        case APP_SPI_EVT_RX_DATA:
            // APP_LOG_INFO("flash_spi_callback APP_SPI_EVT_RX_DATA");
            break;

        case APP_SPI_EVT_ERROR:
            // APP_LOG_INFO("flash_spi_callback APP_SPI_EVT_ERROR");
            break;

        case APP_SPI_EVT_TX_RX:
            // APP_LOG_INFO("flash_spi_callback APP_SPI_EVT_TX_RX");
            break;

        default:
            break;
    }
}

/**
 * @brief Get Flash GD25LE128E ID
 *
 * @param[in] flash_id input Point must > 3 Byte
 * @return get Point is status
 */
int8_t flash_read_rfid(uint8_t* flash_id) // flash_id must > 3byte
{
    uint8_t Send_Data   = GD25LE128E_RDID; // 0x9F
    uint8_t Get_Data[4] = {0};

    FLASH_OPERATE_LOCK();

    app_spi_transmit_receive_sync(APP_SPI_ID_MASTER, &Send_Data, Get_Data, 4, 0x10);

    FLASH_OPERATE_UNLOCK();
    // APP_LOG_DEBUG("flash_read_rfid: Get_Data[0] = 0x%x , Get_Data[1] = 0x%x , Get_Data[2] = 0x%x , Get_Data[3] =
    // 0x%x",Get_Data[0],Get_Data[1],Get_Data[2],Get_Data[3]);
    if (flash_id != NULL) {
        for (int i = 0; i < 3; i++) {
            flash_id[i] = Get_Data[i + 1];
        }
        return FLASH_SUCCESS;
    }

    return FLASH_INPUT_ERROR;
}

int8_t flash_init(void) {
    uint16_t         ret         = 0;
    uint32_t         Flashid     = 0;
    uint8_t          Flash_id[3] = {0};
    // set up struct
    app_spi_params_t flash_struct;
    flash_struct.id                  = APP_SPI_ID_MASTER; // set up spi id master
    // CS configuration
    flash_struct.pin_cfg.cs.type     = APP_IO_TYPE_NORMAL;
    flash_struct.pin_cfg.cs.mux      = APP_IO_MUX_0;
    flash_struct.pin_cfg.cs.pin      = APP_IO_PIN_17;
    flash_struct.pin_cfg.cs.pull     = APP_IO_NOPULL;
    flash_struct.pin_cfg.cs.enable   = APP_SPI_PIN_ENABLE;
    // CLK configuration
    flash_struct.pin_cfg.clk.type    = APP_IO_TYPE_NORMAL;
    flash_struct.pin_cfg.clk.mux     = APP_IO_MUX_0;
    flash_struct.pin_cfg.clk.pin     = APP_IO_PIN_24;
    flash_struct.pin_cfg.clk.pull    = APP_IO_NOPULL;
    flash_struct.pin_cfg.clk.enable  = APP_SPI_PIN_ENABLE;
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
    flash_struct.use_mode.type       = APP_SPI_TYPE_INTERRUPT;

    flash_struct.init.data_size          = SPI_DATASIZE_8BIT;           // data_size = 8bit
    flash_struct.init.clock_polarity     = LL_SSI_SCPOL_LOW;            // flash supported spi mode 0 and 3
    flash_struct.init.clock_phase        = SPI_PHASE_1EDGE;             // pol and pha both 0 or 1
    flash_struct.init.baudrate_prescaler = (SystemCoreClock / 4000000); // speed = 400K
    flash_struct.init.ti_mode            = SPI_TIMODE_DISABLE;          // DISABLE ti mode
    flash_struct.init.slave_select       = SPI_SLAVE_SELECT_0;          // set up cs0

    ret = app_spi_init(&flash_struct, flash_spi_callback);
    if (ret != 0) {
        APP_LOG_ERROR("flash init error!");
        return FLASH_INIT_ERROR;
    }

    flash_read_rfid(Flash_id);
    Flashid = Flash_id[0];
    Flashid = Flashid << 8 | Flash_id[1];
    Flashid = Flashid << 8 | Flash_id[2];
    if (Flashid == GD25LE128E_ID) {
        // APP_LOG_INFO("flash init success!");
        return FLASH_SUCCESS;
    }
    APP_LOG_ERROR("flash id error!");
    return FLASH_ID_ERROR;
}

int8_t flash_read_status(void) {
    uint8_t rd_buf[2] = {0};
    uint8_t send_cmd  = GD25LE128E_RDSR_L;

    FLASH_OPERATE_LOCK();
    
    app_spi_transmit_receive_sync(APP_SPI_ID_MASTER, &send_cmd, rd_buf, 2, 0x10);
    // APP_LOG_DEBUG("GD25LE128E_RDSR_L = 0x%x ",rd_buf[1]);

    FLASH_OPERATE_UNLOCK();

    if ((rd_buf[1] & 0x01) == 1) {
        return FLASH_STATUS_BUSY;
    } else {
        return FLASH_STATUS_FREE;
    }
}

int8_t flash_erase_chip(void) {
    int     ret = 0;
    uint8_t send_cmd;

    FLASH_OPERATE_LOCK();

    send_cmd = GD25LE128E_WREN;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, &send_cmd, 1, 0x10);
    send_cmd = GD25LE128E_CE;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, &send_cmd, 1, 0x10);

    FLASH_OPERATE_UNLOCK();

    ret = flash_read_status();
    while (ret) {
        delay_ms(500);
        ret = flash_read_status();
    }

    return FLASH_SUCCESS;
}

int8_t flash_erase_sector(uint32_t address) {
    int     ret         = 0;
    uint8_t send_cmd[4] = {0};

    FLASH_OPERATE_LOCK();

    send_cmd[0] = GD25LE128E_WREN;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, send_cmd, 1, 0x1000);

    send_cmd[0] = GD25LE128E_SE;
    send_cmd[1] = (uint8_t)(address >> 16);
    send_cmd[2] = (uint8_t)(address >> 8);
    send_cmd[3] = (uint8_t)address;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, send_cmd, 4, 0x1000);

    FLASH_OPERATE_UNLOCK();

    ret = flash_read_status();
    while (ret) {
        delay_ms(30);
        ret = flash_read_status();
    }

    return FLASH_SUCCESS;
}

int8_t flash_erase_block_64k(uint32_t address) {
    int     ret         = 0;
    uint8_t send_cmd[4] = {0};

    FLASH_OPERATE_LOCK();

    send_cmd[0] = GD25LE128E_WREN;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, send_cmd, 1, 0x1000);

    send_cmd[0] = GD25LE128E_BE64;
    send_cmd[1] = (uint8_t)(address >> 16);
    send_cmd[2] = (uint8_t)(address >> 8);
    send_cmd[3] = (uint8_t)address;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, send_cmd, 4, 0x1000);

    FLASH_OPERATE_UNLOCK();

    ret = flash_read_status();
    while (ret) {
        delay_ms(100);
        ret = flash_read_status();
    }

    return FLASH_SUCCESS;
}

int8_t flash_write_data(uint32_t address, uint8_t* data, uint16_t data_size) {
    if (data == NULL)
        return FLASH_DATA_EMPTY;

    if (data_size > 256)
        return FLASH_SIZE_ERROR;

    int8_t  ret = 0;
    uint8_t send_cmd[4];
    
    FLASH_OPERATE_LOCK();

    send_cmd[0] = GD25LE128E_WREN;
    app_spi_transmit_sync(APP_SPI_ID_MASTER, send_cmd, 1, 0x1000);

    send_cmd[0] = GD25LE128E_PP;
    send_cmd[1] = (uint8_t)(address >> 16);
    send_cmd[2] = (uint8_t)(address >> 8);
    send_cmd[3] = (uint8_t)address;

    uint8_t* wt_buf = sys_malloc(data_size + 4);
    memset(wt_buf, 0, data_size + 4);

    memcpy(wt_buf, send_cmd, 4);
    memcpy(wt_buf + 4, data, data_size);

    app_spi_transmit_sync(APP_SPI_ID_MASTER, wt_buf, data_size + 4, 0x1000);
    sys_free(wt_buf);

    FLASH_OPERATE_UNLOCK();

    ret = flash_read_status();
    while (ret) {
        delay_ms(30);
        ret = flash_read_status();
    }
    return FLASH_SUCCESS;
}

int8_t flash_read_data(uint32_t address, uint8_t* data, uint16_t data_size) {
    if (data == NULL)
        return FLASH_DATA_EMPTY;

    if (data_size > 256)
        return FLASH_SIZE_ERROR;

    uint8_t send_cmd[4] = {0};

    FLASH_OPERATE_LOCK();

    address     = address & 0xFFFFFF;
    send_cmd[0] = GD25LE128E_READ;
    send_cmd[1] = (uint8_t)(address >> 16);
    send_cmd[2] = (uint8_t)(address >> 8);
    send_cmd[3] = (uint8_t)address;

    uint8_t* rd_buf = sys_malloc(data_size + 4);
    memset(rd_buf, 0, data_size + 4);

    app_spi_transmit_receive_sync(APP_SPI_ID_MASTER, send_cmd, rd_buf, data_size + 4, 0x1000);
    memcpy(data, rd_buf + 4, data_size);
    sys_free(rd_buf);

    FLASH_OPERATE_UNLOCK();

    // printf("flash_read_data:");
    // for (uint16_t i = 0; i < data_size; i++) {
    //     printf("%02x ", data[i]);
    // }
    // printf("\r\n");

    return FLASH_SUCCESS;
}

int8_t flash_update_sector_data(uint32_t address, uint8_t* data, uint16_t data_size) {
    int8_t ret = 0;

    ret = flash_erase_sector(address);
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("flash_erase_sector[%u] error", address);
        return FLASH_FAILURE;
    }

    ret = flash_write_data(address, data, data_size);
    if (ret != FLASH_SUCCESS) {
        APP_LOG_ERROR("flash_write_data error with %d", ret);
        return FLASH_FAILURE;
    }

    return FLASH_SUCCESS;
}

int flash_func_test() {
    uint8_t  buf_w[5];
    uint8_t  buf_r[5];
    uint32_t addr = GD25LE128E_SECTOR_SIZE * 1;

    memset(buf_w, 0, sizeof(buf_w));
    memset(buf_r, 0, sizeof(buf_r));

    flash_init();

    flash_erase_chip();
    // flash_erase_sector(addr);

    flash_read_data(addr, buf_r, sizeof(buf_r));
    data_stream_hex(buf_r, sizeof(buf_r));

    buf_w[0] = 0x02;
    buf_w[1] = 0x12;
    buf_w[2] = 0x22;
    buf_w[3] = 0x32;
    buf_w[4] = 0x42;

    flash_write_data(addr, buf_w, sizeof(buf_w));

    flash_read_data(addr, buf_r, sizeof(buf_r));
    data_stream_hex(buf_r, sizeof(buf_r));

    flash_erase_sector(addr);

    // delay_ms(1000);

    buf_w[0] = 0xA2;
    buf_w[1] = 0xB2;
    buf_w[2] = 0xC2;
    buf_w[3] = 0xD2;
    buf_w[4] = 0xE2;

    flash_write_data(addr, buf_w, sizeof(buf_w));

    flash_read_data(addr, buf_r, sizeof(buf_r));
    data_stream_hex(buf_r, sizeof(buf_r));

    flash_erase_chip();

    uint8_t rd_buf[256];
    memset(rd_buf, 0x23, sizeof(rd_buf));
    flash_write_data(GD25LE128E_SECTOR_SIZE * 0 + 192, rd_buf, 256); // 依然会写满第一页，页地址到0xFF后再写入的地址是0x00

    flash_read_data(GD25LE128E_SECTOR_SIZE * 0, rd_buf, sizeof(rd_buf));
    data_stream_hex(rd_buf, sizeof(rd_buf));

    delay_ms(10);
    flash_read_data(GD25LE128E_SECTOR_SIZE * 1, rd_buf, sizeof(rd_buf));
    data_stream_hex(rd_buf, sizeof(rd_buf));

    flash_erase_chip();
}