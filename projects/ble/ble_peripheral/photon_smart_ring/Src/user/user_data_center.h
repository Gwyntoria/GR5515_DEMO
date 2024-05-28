#ifndef USER_DATA_CENTER_H
#define USER_DATA_CENTER_H

#include <stdbool.h>
#include <stdint.h>

#include "ring_buffer.h"

#define S2F_BUFFER_SIZE          (1024 * 4)
#define FRAME_HEADER_SIZE        2
#define FRAME_DATA_SIZE          12
#define FRAME_SIZE               (FRAME_HEADER_SIZE + FRAME_DATA_SIZE) // 14
#define S2F_FRAME_CNT_THRSHD     ((S2F_BUFFER_SIZE / FRAME_SIZE) - 5) // 287
#define DATA_PACKET_HEADER_SIZE  8
#define DATA_PACKET_CHEKSUM_SIZE 4

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

struct DataCenterS2f {
    DataCenter data_center;

    /**
    @brief Function pointer for receiving sensor data.
     *
     * This function pointer is used to receive sensor data from the data center.
     * It takes a pointer to a DataCenterS2f structure, a sensor type, and a sensor value as parameters.
     * The function should return an integer value.
     *
     * @param data_center_s2f Pointer to the DataCenterS2f structure.
     * @param type The type of the sensor data.
     * @param value The value of the sensor data.
     * @return int The return value of the function.
     */
    int (*recv_sensor_func)(DataCenterS2f* data_center_s2f, uint8_t type, uint16_t value);

    /**
     * @brief Function pointer for sending data to flash.
     *
     * This function pointer is used to send data to the flash memory.
     * It takes a pointer to a DataCenterS2f structure as a parameter and returns an integer.
     *
     * @param data_center_s2f Pointer to the DataCenterS2f structure containing the data to be sent.
     * @return Integer value indicating the status of the send operation.
     */
    int (*send_flash_func)(DataCenterS2f* data_center_s2f);
};

struct DataCenterF2b {
    DataCenter data_center;

    /**
     * @brief Function pointer for allocating memory in the data center.
     *
     * This function pointer is used to allocate memory in the data center for a given length.
     *
     * @param data_center_f2b Pointer to the DataCenterF2b structure.
     * @param len Length of the memory to be allocated.
     * @return int Returns 0 on success, or a negative value on failure.
     */
    int (*alloc_mem_func)(DataCenterF2b* data_center_f2b, uint16_t len);

    /**
     * Function pointer to free memory in the data center.
     *
     * This function pointer is used to free memory allocated by the data center.
     *
     * @param data_center_f2b The data center object to free memory from.
     * @return The result of the memory freeing operation.
     */
    int (*free_mem_func)(DataCenterF2b* data_center_f2b);

    /**
     * @brief Function pointer for receiving data from flash memory.
     *
     * This function pointer is used to receive data from flash memory and perform optional erase operation.
     *
     * @param data_center_f2b Pointer to the DataCenterF2b structure.
     * @param len Length of the data to be received.
     * @param whole Flag indicating whether to receive the whole data.
     * @param erase Flag indicating whether to perform erase operation before receiving data.
     * @return int Returns the status of the receive operation.
     */
    int (*recv_flash_func)(DataCenterF2b* data_center_f2b, uint16_t len, bool whole, bool erase);

    /**
     * @brief Function pointer for sending BLE data.
     *
     * This function pointer is used to send BLE data using the provided buffer and length of flash data.
     * In function, the data will be added packet header and checksum.
     *
     * @param data_center_f2b Pointer to the DataCenterF2b structure.
     * @param buffer Pointer to the data buffer.
     * @param len Length of the data to be sent.
     * @param sequence Sequence number of the packet.
     * @param is_last_packet Flag indicating whether the packet is the last one.
     * @return int Returns the status of the send operation.
     */
    int (*send_ble_func)(DataCenterF2b* data_center_f2b, uint8_t* buffer, uint16_t len, uint8_t sequence, bool is_last_packet);

    /**
     * @brief Function pointer to retrieve the size of data in the DataCenterF2b structure.
     *
     * This function pointer is used to get the size of data stored in the DataCenterF2b structure.
     *
     * @param data_center_f2b Pointer to the DataCenterF2b structure.
     * @return The size of data in bytes.
     */
    uint32_t (*get_data_size_func)(DataCenterF2b* data_center_f2b);
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

int  udc_init(void);
void udc_deinit(void);

int udc_test(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif