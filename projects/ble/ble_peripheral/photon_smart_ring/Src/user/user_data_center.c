#include "user_data_center.h"

#include "app_log.h"
#include "hal_flash.h"
#include "ring_buffer.h"

#include "user_common.h"
#include "user_file_sys.h"
#include "user_rtc.h"

#define DC_LOG_OUTPUT 0

#define INTER_MEM_ADDR_DATA 0x010F0000
#define INTER_MEM_SIZE_DATA 0x04

typedef struct DataFrame {
    uint8_t  type;
    uint8_t  year;
    uint8_t  month;
    uint8_t  date;
    uint8_t  hour;
    uint8_t  min;
    uint8_t  sec;
    uint16_t ms;
    uint8_t  week;
    uint16_t value;
} DataFrame;

typedef struct DataPacketHeader {
    uint8_t  seq;
    uint8_t  data_len_pack;
    uint16_t data_len_whole;
    uint16_t frame_cnt;
    uint16_t reserved;
} DataPacketHeader;

static uint8_t FRAME_HEADER[FRAME_HEADER_SIZE] = {0x40, 0x90};

static DataCenterS2f* s_data_center_s2f = NULL; // sensor to flash
static DataCenterF2b* s_data_center_f2b = NULL; // flash to bluetooth

void _serialize_data_frame(DataFrame* data_frame, uint8_t* buffer) {
    uint16_t index = 0;

    buffer[index++] = data_frame->type;
    buffer[index++] = data_frame->year;
    buffer[index++] = data_frame->month;
    buffer[index++] = data_frame->date;
    buffer[index++] = data_frame->hour;
    buffer[index++] = data_frame->min;
    buffer[index++] = data_frame->sec;
    buffer[index++] = (data_frame->ms >> 8) & 0xFF;
    buffer[index++] = data_frame->ms & 0xFF;
    buffer[index++] = data_frame->week;
    buffer[index++] = (data_frame->value >> 8) & 0xFF;
    buffer[index++] = data_frame->value & 0xFF;
}

void _deserialize_data_frame(DataFrame* data_frame, uint8_t* buffer) {
    uint16_t index = 0;

    data_frame->type  = buffer[index++];
    data_frame->year  = buffer[index++];
    data_frame->month = buffer[index++];
    data_frame->date  = buffer[index++];
    data_frame->hour  = buffer[index++];
    data_frame->min   = buffer[index++];
    data_frame->sec   = buffer[index++];
    data_frame->ms    = (buffer[index++] << 8) | buffer[index++];
    data_frame->week  = buffer[index++];
    data_frame->value = (buffer[index++] << 8) | buffer[index++];
}

void _serialize_data_packet_header(DataPacketHeader* data_packet_header, uint8_t* buffer) {
    uint16_t index = 0;

    buffer[index++] = data_packet_header->seq;
    buffer[index++] = data_packet_header->data_len_pack;
    buffer[index++] = (data_packet_header->data_len_whole >> 8) & 0xFF;
    buffer[index++] = data_packet_header->data_len_whole & 0xFF;
    buffer[index++] = (data_packet_header->frame_cnt >> 8) & 0xFF;
    buffer[index++] = data_packet_header->frame_cnt & 0xFF;
    buffer[index++] = (data_packet_header->reserved >> 8) & 0xFF;
    buffer[index++] = data_packet_header->reserved & 0xFF;
}

void _deserialize_data_packet_header(DataPacketHeader* data_packet_header, uint8_t* buffer) {
    uint16_t index = 0;

    data_packet_header->seq            = buffer[index++];
    data_packet_header->data_len_pack  = buffer[index++];
    data_packet_header->data_len_whole = (buffer[index++] << 8) | buffer[index++];
    data_packet_header->frame_cnt      = (buffer[index++] << 8) | buffer[index++];
    data_packet_header->reserved       = (buffer[index++] << 8) | buffer[index++];
}

void _serialize_data_manager_info(DataCenter* data_manager, uint8_t* buffer) {
    uint16_t index = 0;

    write_big_endian_2(buffer + index, data_manager->length);
    index += 2;

    write_big_endian_2(buffer + index, data_manager->frame_cnt);
    index += 2;
}

void _deserialize_data_manager_info(DataCenter* data_manager, uint8_t* buffer) {
    uint16_t index = 0;

    data_manager->length = read_big_endian_2(buffer + index);
    index += 2;

    data_manager->frame_cnt = read_big_endian_2(buffer + index);
    index += 2;
}

int _calculate_frame_cnt_in_buffer(uint8_t* buffer, uint16_t buffer_size) {
    uint16_t frame_num = 0;

    frame_num = count_sequence_in_buffer(buffer, (size_t)buffer_size, FRAME_HEADER, (size_t)FRAME_HEADER_SIZE);

#if DC_LOG_OUTPUT
    APP_LOG_DEBUG("Frame number in buffer: %d", frame_num);
#endif

    return frame_num;
}

int _recv_data_from_sensor(DataCenterS2f* data_center_s2f, uint8_t type, uint16_t value) {
    if (data_center_s2f == NULL) {
        APP_LOG_ERROR("Invalid data center s2f");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (data_center_s2f->data_center.frame_cnt >= S2F_FRAME_CNT_THRSHD) {
        APP_LOG_INFO("Data frame count [%u] is more than threshold, space will fill up soon",
                     data_center_s2f->data_center.frame_cnt);
        return GUNTER_ERR_INSUFFICIENT;
    }

    DataFrame      data_frame;
    app_rtc_time_t rtc_time;

    memset(&data_frame, 0, sizeof(DataFrame));
    memset(&rtc_time, 0, sizeof(app_rtc_time_t));

    if (rtc_get_current_local_time(&rtc_time) != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Get current local time failed");
        return GUNTER_FAILURE;
    }

    data_frame.type  = type;
    data_frame.year  = rtc_time.year;
    data_frame.month = rtc_time.mon;
    data_frame.date  = rtc_time.date;
    data_frame.hour  = rtc_time.hour;
    data_frame.min   = rtc_time.min;
    data_frame.sec   = rtc_time.sec;
    data_frame.ms    = rtc_time.ms;
    data_frame.week  = rtc_time.week;
    data_frame.value = value;

    uint8_t buffer[FRAME_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // write frame header
    memcpy(buffer, FRAME_HEADER, FRAME_HEADER_SIZE);

    _serialize_data_frame(&data_frame, buffer + FRAME_HEADER_SIZE);

    // write frame data
    uint32_t ring_ret = 0;

    ring_ret = ring_buffer_write(&data_center_s2f->data_center.ring_t, buffer, FRAME_SIZE);
    if (ring_ret != FRAME_SIZE) {
        APP_LOG_ERROR("Write data frame to queue failed with %d", ring_ret);
        return GUNTER_FAILURE;
    }

    data_center_s2f->data_center.length += FRAME_SIZE;
    data_center_s2f->data_center.frame_cnt++;

    return GUNTER_SUCCESS;
}

int _send_data_into_flash(DataCenterS2f* data_center_s2f) {
    if (data_center_s2f == NULL) {
        APP_LOG_ERROR("Invalid data center s2f");
        return GUNTER_ERR_NULL_POINTER;
    }

    int      ret      = 0;
    uint32_t ring_ret = 0;

    ring_ret = ring_buffer_items_count_get(&data_center_s2f->data_center.ring_t);
    if (ring_ret == 0) {
        // APP_LOG_INFO("No data to write");
        return GUNTER_SUCCESS;
    }

    uint8_t* buffer = (uint8_t*)sys_malloc(data_center_s2f->data_center.length);
    if (buffer == NULL) {
        APP_LOG_ERROR("Memory allocation failed");
        return GUNTER_ERR_NULL_POINTER;
    }

    ring_ret = ring_buffer_read(&data_center_s2f->data_center.ring_t, buffer, data_center_s2f->data_center.length);
    if (ring_ret != data_center_s2f->data_center.length) {
        APP_LOG_ERROR("Read data from ring_t buffer failed with %d, whole_len", ring_ret, data_center_s2f->data_center.length);
        return GUNTER_FAILURE;
    }

    // APP_LOG_INFO("Write data to flash: %d", data_center_s2f->data_center.length);

    if (data_center_s2f->data_center.length <= 0x1000) {
        ret = ufs_write_zone_data(kFlashZoneData, buffer, data_center_s2f->data_center.length);
    } else {
        ret = ufs_write_zone_data_mass(kFlashZoneData, buffer, data_center_s2f->data_center.length);
    }

    sys_free(buffer);
    buffer = NULL;

    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Write data to flash failed");
        return ret;
    }

    ring_buffer_clean(&data_center_s2f->data_center.ring_t);
    data_center_s2f->data_center.length    = 0;
    data_center_s2f->data_center.frame_cnt = 0;
}

int _alloc_data_center_f2b_mem(DataCenterF2b* data_center_f2b, uint16_t len) {
    if (data_center_f2b == NULL) {
        APP_LOG_ERROR("Invalid data center f2b");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (data_center_f2b->data_center.ring_t.p_buffer != NULL) {
        APP_LOG_ERROR("Data center f2b buffer is not NULL");
        return GUNTER_ERR_INVALID_PARAM;
    }

    uint32_t mem_size = (uint32_t)len + 1;

#if DC_LOG_OUTPUT
    // APP_LOG_DEBUG("mem_size: %u", mem_size);
#endif

    uint8_t* buffer = (uint8_t*)sys_malloc(mem_size);
    if (buffer == NULL) {
        APP_LOG_ERROR("Memory allocation failed");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (ring_buffer_init(&data_center_f2b->data_center.ring_t, buffer, mem_size) != true) {
        APP_LOG_ERROR("Init ring_t buffer failed");
        return GUNTER_FAILURE;
    }

    mem_size = ring_buffer_surplus_space_get(&data_center_f2b->data_center.ring_t);

#if DC_LOG_OUTPUT
    // APP_LOG_DEBUG("mem_size: %d", mem_size);
    // APP_LOG_DEBUG("ring size: %d", data_center_f2b->data_center.ring_t.buffer_size);
#endif

    data_center_f2b->data_center.length    = 0;
    data_center_f2b->data_center.frame_cnt = 0;

    return GUNTER_SUCCESS;
}

int _free_data_center_f2b_mem(DataCenterF2b* data_center_f2b) {
    if (data_center_f2b == NULL) {
        APP_LOG_ERROR("Invalid data center f2b");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (data_center_f2b->data_center.ring_t.p_buffer == NULL) {
        APP_LOG_ERROR("Data center f2b buffer is NULL");
        return GUNTER_ERR_NULL_POINTER;
    }

    ring_buffer_clean(&data_center_f2b->data_center.ring_t);
    sys_free(data_center_f2b->data_center.ring_t.p_buffer);
    data_center_f2b->data_center.ring_t.p_buffer    = NULL;
    data_center_f2b->data_center.ring_t.buffer_size = 0;
    data_center_f2b->data_center.length             = 0;
    data_center_f2b->data_center.frame_cnt          = 0;

    return GUNTER_SUCCESS;
}

int _recv_data_from_flash(DataCenterF2b* data_center_f2b, uint16_t len, bool whole, bool erase) {
    if (data_center_f2b == NULL) {
        APP_LOG_ERROR("Invalid data center f2b");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (data_center_f2b->data_center.ring_t.p_buffer == NULL) {
        APP_LOG_ERROR("Data center f2b buffer is NULL");
        return GUNTER_ERR_NULL_POINTER;
    }

    int      ret      = 0;
    uint32_t ring_ret = 0;

    uint8_t* buffer = (uint8_t*)sys_malloc((uint32_t)len);
    if (buffer == NULL) {
        APP_LOG_ERROR("Memory allocation failed");
        return GUNTER_ERR_NULL_POINTER;
    }

#if DC_LOG_OUTPUT
    APP_LOG_INFO("Read data from flash: %d", len);
#endif

    ret = ufs_read_zone_data(kFlashZoneData, buffer, (uint32_t*)&len, whole, erase);
    if (ret != len) {
        APP_LOG_ERROR("Read data from flash failed with %d", ret);
        return GUNTER_FAILURE;
    }

    uint32_t ring_size = ring_buffer_surplus_space_get(&data_center_f2b->data_center.ring_t);

#if DC_LOG_OUTPUT
    APP_LOG_DEBUG("ring buffer size: %d", ring_size);
#endif

    if (ring_size < len) {
        APP_LOG_ERROR("Ring buffer size is not enough");
        return GUNTER_ERR_INSUFFICIENT;
    }

    ring_ret = ring_buffer_write(&data_center_f2b->data_center.ring_t, buffer, (uint32_t)len);
    if (ring_ret != len) {
        APP_LOG_ERROR("Write data to ring_t buffer failed with %d", ring_ret);
        return GUNTER_FAILURE;
    }

    data_center_f2b->data_center.length += len;
    data_center_f2b->data_center.frame_cnt = _calculate_frame_cnt_in_buffer(buffer, len);

    sys_free(buffer);
    buffer = NULL;

    return GUNTER_SUCCESS;
}

int _send_data_to_ble(DataCenterF2b* data_center_f2b, uint8_t* buffer, uint8_t len, uint8_t sequence, uint16_t len_whole) {
    if (data_center_f2b == NULL) {
        APP_LOG_ERROR("Invalid data center f2b");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (data_center_f2b->data_center.ring_t.p_buffer == NULL) {
        APP_LOG_ERROR("Data center f2b buffer is NULL");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (buffer == NULL) {
        APP_LOG_ERROR("Invalid buffer address");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (len > data_center_f2b->data_center.ring_t.buffer_size) {
        APP_LOG_ERROR("Data length is more than buffer size");
        return GUNTER_ERR_INVALID_PARAM;
    }

    uint8_t* data_buffer = (uint8_t*)sys_malloc(len);
    if (data_buffer == NULL) {
        APP_LOG_ERROR("Memory allocation failed");
        return GUNTER_ERR_NULL_POINTER;
    }

    uint32_t ring_ret = 0;

    ring_ret = ring_buffer_read(&data_center_f2b->data_center.ring_t, data_buffer, len);
    if (ring_ret != len) {
        APP_LOG_ERROR("Read data from ring_t buffer failed with %d", ring_ret);
        return GUNTER_FAILURE;
    }

    uint16_t frame_cnt = (uint16_t)_calculate_frame_cnt_in_buffer(data_buffer, (uint16_t)len);

    DataPacketHeader data_packet_header;
    memset(&data_packet_header, 0, sizeof(DataPacketHeader));

    data_packet_header.seq            = sequence;
    data_packet_header.data_len_pack  = len;
    data_packet_header.data_len_whole = len_whole;
    data_packet_header.frame_cnt      = frame_cnt;
    data_packet_header.reserved       = 0;

    // write data packet header
    _serialize_data_packet_header(&data_packet_header, buffer);

    // write data packet body
    memcpy(buffer + DATA_PACKET_HEADER_SIZE, data_buffer, len);
    sys_free(data_buffer);
    data_buffer = NULL;

    // write data packet checksum
    int checksum = get_checksum_crc32(buffer, len + DATA_PACKET_HEADER_SIZE);
    write_big_endian_4(buffer + len + DATA_PACKET_HEADER_SIZE, checksum);

    data_center_f2b->data_center.length -= len;
    data_center_f2b->data_center.frame_cnt -= frame_cnt;

    return GUNTER_SUCCESS;
}

uint32_t _get_center_f2b_data_size(DataCenterF2b* data_center_f2b) {
    if (data_center_f2b == NULL) {
        APP_LOG_ERROR("Invalid data center f2b");
        return GUNTER_ERR_NULL_POINTER;
    }

    if (data_center_f2b->data_center.ring_t.p_buffer == NULL) {
        APP_LOG_ERROR("Data center f2b buffer is NULL");
        return GUNTER_ERR_NULL_POINTER;
    }

    return ring_buffer_items_count_get(&data_center_f2b->data_center.ring_t);
}

DataCenterS2f* get_data_center_s2f(void) {
    if (s_data_center_s2f != NULL) {
        return s_data_center_s2f;
    }

    s_data_center_s2f = (DataCenterS2f*)sys_malloc(sizeof(DataCenterS2f));
    if (s_data_center_s2f == NULL) {
        APP_LOG_ERROR("Memory allocation failed");
        return NULL;
    }

    memset(s_data_center_s2f, 0, sizeof(DataCenterS2f));

    uint8_t* buffer = (uint8_t*)sys_malloc(S2F_BUFFER_SIZE);
    if (buffer == NULL) {
        APP_LOG_ERROR("Memory allocation failed");
        return NULL;
    }

    if (ring_buffer_init(&s_data_center_s2f->data_center.ring_t, buffer, S2F_BUFFER_SIZE) != true) {
        APP_LOG_ERROR("Init ring_t buffer failed");
        return NULL;
    }

    return s_data_center_s2f;
}

void destroy_data_center_s2f(void) {
    if (s_data_center_s2f != NULL) {
        ring_buffer_clean(&s_data_center_s2f->data_center.ring_t);
        sys_free(s_data_center_s2f->data_center.ring_t.p_buffer);
        s_data_center_s2f->data_center.ring_t.p_buffer = NULL;
        sys_free(s_data_center_s2f);
        s_data_center_s2f = NULL;
    }
}

DataCenterF2b* get_data_center_f2b(void) {
    if (s_data_center_f2b != NULL) {
        return s_data_center_f2b;
    }

    s_data_center_f2b = (DataCenterF2b*)sys_malloc(sizeof(DataCenterF2b));
    if (s_data_center_f2b == NULL) {
        APP_LOG_ERROR("Memory allocation failed");
        return NULL;
    }

    memset(s_data_center_f2b, 0, sizeof(DataCenterF2b));

    return s_data_center_f2b;
}

void destroy_data_center_f2b(void) {
    if (s_data_center_f2b != NULL) {
        ring_buffer_clean(&s_data_center_f2b->data_center.ring_t);
        sys_free(s_data_center_f2b->data_center.ring_t.p_buffer);
        s_data_center_f2b->data_center.ring_t.p_buffer = NULL;
        sys_free(s_data_center_f2b);
        s_data_center_f2b = NULL;
    }
}

int udc_init(void) {
    s_data_center_s2f = get_data_center_s2f();
    if (s_data_center_s2f == NULL) {
        APP_LOG_ERROR("Get data center s2f failed");
        return GUNTER_FAILURE;
    }

    s_data_center_f2b = get_data_center_f2b();
    if (s_data_center_f2b == NULL) {
        APP_LOG_ERROR("Get data center f2b failed");
        return GUNTER_FAILURE;
    }

    s_data_center_s2f->recv_sensor_func = _recv_data_from_sensor;
    s_data_center_s2f->send_flash_func  = _send_data_into_flash;

    s_data_center_f2b->alloc_mem_func     = _alloc_data_center_f2b_mem;
    s_data_center_f2b->free_mem_func      = _free_data_center_f2b_mem;
    s_data_center_f2b->recv_flash_func    = _recv_data_from_flash;
    s_data_center_f2b->send_ble_func      = _send_data_to_ble;
    s_data_center_f2b->get_data_size_func = _get_center_f2b_data_size;

    APP_LOG_INFO("udc_init success");
    return GUNTER_SUCCESS;
}

void udc_deinit(void) {
    destroy_data_center_s2f();
    destroy_data_center_f2b();

    APP_LOG_INFO("udc_deinit success");
}

int udc_test(void) {
    int ret = 0;

    // * DataCenterS2f
    DataCenterS2f* data_center_s2f = get_data_center_s2f();
    if (data_center_s2f == NULL) {
        APP_LOG_ERROR("Get data center s2f failed");
        return GUNTER_FAILURE;
    }

    // S2F数据中心接收从传感器发送的数据
    for (int i = 0; i < 100; i++) {
        ret = data_center_s2f->recv_sensor_func(data_center_s2f, kDataTypeHr, i);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("Receive sensor data failed");
            return GUNTER_FAILURE;
        }
    }

    // S2F数据中心将数据写入到Flash
    ret = data_center_s2f->send_flash_func(data_center_s2f);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Send data to flash failed");
        return GUNTER_FAILURE;
    }

    // * DataCenterF2b
    DataCenterF2b* data_center_f2b = get_data_center_f2b();
    if (data_center_f2b == NULL) {
        APP_LOG_ERROR("Get data center f2b failed");
        return GUNTER_FAILURE;
    }

    // 获取Flash中可读取数据的大小
    int flash_data_len = ufs_get_readable_zone_data_size(kFlashZoneData);
    if (flash_data_len < 0) {
        APP_LOG_ERROR("Get readable zone data size failed");
        return GUNTER_FAILURE;
    }

#if DC_LOG_OUTPUT
    // APP_LOG_DEBUG("FLASH: Data length: %d", flash_data_len);
#endif

    // F2B数据中心分配内存
    ret = data_center_f2b->alloc_mem_func(data_center_f2b, (uint16_t)flash_data_len);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Alloc memory failed");
        return GUNTER_FAILURE;
    }

    // F2B数据中心从Flash中读取数据
    ret = data_center_f2b->recv_flash_func(data_center_f2b, (uint16_t)flash_data_len, true , false);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Receive data from flash failed with %d", ret);
        return GUNTER_FAILURE;
    }

    uint32_t center_data_len = data_center_f2b->get_data_size_func(data_center_f2b);

#if DC_LOG_OUTPUT
    // APP_LOG_DEBUG("CENTER: Data length: %d", center_data_len);
#endif

    uint32_t pack_len = FRAME_SIZE * 10 + DATA_PACKET_HEADER_SIZE + DATA_PACKET_CHEKSUM_SIZE;
    uint8_t* buffer   = (uint8_t*)sys_malloc(pack_len);
    if (buffer == NULL) {
        APP_LOG_ERROR("Memory allocation failed");
        return GUNTER_ERR_NULL_POINTER;
    }

    // F2B数据中心发送数据到蓝牙
    for (int i = 0; i < 10; i++) {
        ret = data_center_f2b->send_ble_func(data_center_f2b, buffer, FRAME_SIZE * 10, i, i == 9 ? true : false);
        if (ret != GUNTER_SUCCESS) {
            APP_LOG_ERROR("Send data to bluetooth failed");
            return GUNTER_FAILURE;
        }

        APP_LOG_DEBUG("Send data to bluetooth: %d", i);
        data_stream_hex(buffer, pack_len);

        memset(buffer, 0, pack_len);
    }

    sys_free(buffer);
    buffer = NULL;

    // F2B数据中心释放内存
    ret = data_center_f2b->free_mem_func(data_center_f2b);
    if (ret != GUNTER_SUCCESS) {
        APP_LOG_ERROR("Free memory failed");
        return GUNTER_FAILURE;
    }

    udc_deinit();

    ufs_erase_zone_data(kFlashZoneData);

    APP_LOG_INFO("udc_test success");

    return GUNTER_SUCCESS;
}
