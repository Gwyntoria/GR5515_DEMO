#include "user_hrv.h"

#include <math.h>

#include "app_log.h"
#include "ring_buffer.h"
#include "user_common.h"

#define HRV_BUFFER_SIZE 256

typedef struct hrv_data_warehouse {
    ring_buffer_t hrv_buffer;
    uint16_t      rri_data_count;
} hrv_data_warehouse_t;

static uint8_t rri_data[HRV_BUFFER_SIZE];

static hrv_data_warehouse_t hrv_data_warehouse;

void user_hrv_init(void) {
    ring_buffer_init(&hrv_data_warehouse.hrv_buffer, rri_data, HRV_BUFFER_SIZE);
    hrv_data_warehouse.rri_data_count = 0;
}

/**
 * @brief Adds HRV data to the HRV buffer.
 *
 * This function adds the given RRI data to the HRV buffer in the HRV data warehouse.
 * If the HRV buffer is already full, an error message is logged and an error code is returned.
 *
 * @param rri_data The RRI data to be added to the HRV buffer.
 * @return Returns GUNTER_SUCCESS if the data is successfully added, GUNTER_FAILURE if there is an error writing to the buffer,
 * or GUNTER_ERR_INSUFFICIENT if the HRV buffer is already full.
 */
int user_hrv_add_data(int rri_data) {
    if (ring_buffer_is_reach_left_threshold(&hrv_data_warehouse.hrv_buffer, 4) == true) {
        APP_LOG_ERROR("HRV buffer is full");
        return GUNTER_ERR_INSUFFICIENT;
    }

    if (ring_buffer_write(&hrv_data_warehouse.hrv_buffer, (uint8_t*)&rri_data, sizeof(rri_data)) == sizeof(rri_data)) {
        hrv_data_warehouse.rri_data_count++;
        return GUNTER_SUCCESS;
    } else {
        return GUNTER_FAILURE;
    }
}

/**
 * @brief Retrieves HRV (Heart Rate Variability) data from the HRV data warehouse.
 *
 * This function retrieves HRV data from the HRV data warehouse and stores it in the provided array.
 *
 * @param rri_data Pointer to the array where the retrieved HRV data will be stored.
 * @return Returns GUNTER_SUCCESS if HRV data is successfully retrieved, GUNTER_FAILURE if there was an error, or GUNTER_ERR_INSUFFICIENT if there is no HRV data available.
 */
int user_hrv_get_data(int* rri_data) {
    if (ring_buffer_items_count_get(&hrv_data_warehouse.hrv_buffer) == 0) {
        APP_LOG_ERROR("No HRV data available");
        return GUNTER_ERR_INSUFFICIENT;
    }

    if (ring_buffer_read(&hrv_data_warehouse.hrv_buffer, (uint8_t*)rri_data, sizeof(*rri_data)) == sizeof(*rri_data)) {
        hrv_data_warehouse.rri_data_count--;
        return GUNTER_SUCCESS;
    } else {
        return GUNTER_FAILURE;
    }
}

/**
 * @brief Get the count of RRI (R-R Interval) data.
 *
 * This function returns the count of RRI data stored in the hrv_data_warehouse.
 *
 * @return The count of RRI data.
 */
uint16_t user_hrv_get_rri_data_count(void) {
    return hrv_data_warehouse.rri_data_count;
}

/**
 * @brief Cleans the HRV data by clearing the HRV buffer and resetting the RRI data count.
 *
 * This function clears the HRV buffer and sets the RRI data count to zero, effectively cleaning the HRV data.
 */
void user_hrv_clean_data(void) {
    ring_buffer_clean(&hrv_data_warehouse.hrv_buffer);
    hrv_data_warehouse.rri_data_count = 0;
}

/**
 * Calculates the average of the HRV (Heart Rate Variability) data stored in the ring buffer.
 *
 * @param hrv_data_average Pointer to store the calculated average HRV data.
 * @return GUNTER_SUCCESS if the average is calculated successfully, GUNTER_ERR_INSUFFICIENT if there is no HRV data available, or GUNTER_FAILURE if there is an error in retrieving the HRV data.
 */
int user_hrv_get_data_average(int* hrv_data_average) {
    if (ring_buffer_items_count_get(&hrv_data_warehouse.hrv_buffer) == 0) {
        APP_LOG_ERROR("No HRV data available");
        return GUNTER_ERR_INSUFFICIENT;
    }

    int hrv_data_sum   = 0;
    int rri_data_count = user_hrv_get_rri_data_count();
    int rri_data;

    for (int i = 0; i < rri_data_count; i++) {
        if (ring_buffer_pick(&hrv_data_warehouse.hrv_buffer, (uint8_t*)&rri_data, sizeof(rri_data)) == sizeof(rri_data)) {
            hrv_data_sum += rri_data;
        } else {
            return GUNTER_FAILURE;
        }
    }

    *hrv_data_average = hrv_data_sum / rri_data_count;

    ring_buffer_clean(&hrv_data_warehouse.hrv_buffer);

    return GUNTER_SUCCESS;
}

/**
 * Calculates HRV (Heart Rate Variability) and respiratory rate based on the provided RRI (R-R Interval) data.
 *
 * @param rmssd Pointer to store the calculated RMSSD (Root Mean Square of Successive Differences).
 * @param respiratory_rate Pointer to store the calculated respiratory rate.
 * @return GUNTER_SUCCESS if the calculation is successful, GUNTER_ERR_INSUFFICIENT if there is insufficient data.
 */
int user_hrv_calculate_hrv_rr(uint16_t* rmssd, uint16_t* respiratory_rate) {
    if (ring_buffer_items_count_get(&hrv_data_warehouse.hrv_buffer) == 0) {
        APP_LOG_ERROR("No HRV data available");
        return GUNTER_ERR_INSUFFICIENT;
    }

    int rri_data_count = user_hrv_get_rri_data_count();

    if (rri_data_count < 5) {
        APP_LOG_ERROR("HRV data count is less than 5");
        return GUNTER_ERR_INSUFFICIENT;
    }

    int rri_data[rri_data_count];
    int rri_data_diff[rri_data_count - 1];
    int rri_data_diff_sum = 0;
    int breath_count      = 0;
    int breath_duration   = 0;

    for (int i = 0; i < rri_data_count; i++) {
        if (ring_buffer_pick(&hrv_data_warehouse.hrv_buffer, (uint8_t*)&rri_data[i], sizeof(rri_data[i])) != sizeof(rri_data[i])) {
            return GUNTER_FAILURE;
        }

        breath_duration += rri_data[i];
    }

    for (int i = 1; i < rri_data_count; i++) {
        rri_data_diff[i - 1] = rri_data[i] - rri_data[i - 1];
        rri_data_diff_sum += rri_data_diff[i - 1] * rri_data_diff[i - 1];
    }

    *rmssd = (uint16_t)sqrt(rri_data_diff_sum / (rri_data_count - 1));

    for (int i = 1; i < rri_data_count - 1; i++) {
        if (rri_data_diff[i] > 0 && rri_data_diff[i - 1] < 0) {
            breath_count++;
        }
    }

    *respiratory_rate = (uint16_t)(breath_count * 60 / (breath_duration / 1000));

    return GUNTER_SUCCESS;
}
