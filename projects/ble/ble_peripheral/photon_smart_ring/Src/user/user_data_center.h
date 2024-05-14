#ifndef USER_DATA_CENTER_H
#define USER_DATA_CENTER_H

#include <stdbool.h>
#include <stdint.h>

#include "ring_buffer.h"

typedef enum DataType {
    kDataTypeReserved,
    kDataTypeHr,
    kDataTypeHrv,
    kDataTypeSpo2,
    kDataTypeRr,
    kDataTypeStep,
    kDataTypeBattery,
    kDataTypeTemp,
    kDataTypeTime,
} DataType;

typedef struct DataCenter {
    ring_buffer_t ring_t;
    uint16_t      length;
    uint16_t      frame_cnt;
} DataCenter;

typedef struct DataCenterS2f DataCenterS2f;
typedef struct DataCenterF2b DataCenterF2b;

typedef int (*recv_sensor_func)(DataCenterS2f* data_center_s2f, uint8_t type, uint16_t value);
typedef int (*send_flash_func)(DataCenterS2f* data_center_s2f);

typedef int (*alloc_mem_func)(DataCenterF2b* data_center_f2b, uint16_t len);
typedef int (*free_mem_func)(DataCenterF2b* data_center_f2b);
typedef int (*recv_flash_func)(DataCenterF2b* data_center_f2b, uint16_t len, bool erase);
typedef int (*send_ble_func)(DataCenterF2b* data_center_f2b, uint8_t* buffer, uint16_t len);
typedef uint32_t (*get_data_size_func)(DataCenterF2b* data_center_f2b);

struct DataCenterS2f {
    DataCenter       data_center;
    recv_sensor_func recv_sensor;
    send_flash_func  send_flash;
};

struct DataCenterF2b {
    DataCenter         data_center;
    alloc_mem_func     alloc_mem;
    free_mem_func      free_mem;
    recv_flash_func    recv_flash;
    send_ble_func      send_ble;
    get_data_size_func get_data_size;
};

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

DataCenterS2f* get_data_center_s2f(void);
DataCenterF2b* get_data_center_f2b(void);

void destroy_data_center_s2f(void);
void destroy_data_center_f2b(void);

int  udm_init(void);
void udm_deinit(void);

int udm_test(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif