
#include "user_3x2x_log.h"

#include <stdint.h>
#include <string.h>

#include "app_log.h"
#include "ring_buffer.h"
#include "user_common.h"

#define LOG_BUF_SIZE (1024)

static uint8_t log_buf_hr[LOG_BUF_SIZE];
static uint8_t log_buf_hrv[LOG_BUF_SIZE];
static uint8_t log_buf_spo2[LOG_BUF_SIZE];
static uint8_t log_buf_adt[LOG_BUF_SIZE];

typedef struct log_ring_t {
    ring_buffer_t ring_buffer;
    uint16_t      log_cnt;
} log_ring_t;

static log_ring_t log_ring_hr;
static log_ring_t log_ring_hrv;
static log_ring_t log_ring_spo2;
static log_ring_t log_ring_adt;

void user_3x2x_log_init(void) {
    memset(&log_ring_hr, 0, sizeof(log_ring_t));
    memset(&log_ring_hrv, 0, sizeof(log_ring_t));
    memset(&log_ring_spo2, 0, sizeof(log_ring_t));
    memset(&log_ring_adt, 0, sizeof(log_ring_t));

    ring_buffer_init(&log_ring_hr.ring_buffer, log_buf_hr, LOG_BUF_SIZE);
    ring_buffer_init(&log_ring_hrv.ring_buffer, log_buf_hrv, LOG_BUF_SIZE);
    ring_buffer_init(&log_ring_spo2.ring_buffer, log_buf_spo2, LOG_BUF_SIZE);
    ring_buffer_init(&log_ring_adt.ring_buffer, log_buf_adt, LOG_BUF_SIZE);
}

int user_3x2x_log_cache_hr(log_hr_t* p_hr) {
    uint32_t ring_ret = 0;

    if (ring_buffer_is_reach_left_threshold(&log_ring_hr.ring_buffer, sizeof(log_hr_t))) {
        APP_LOG_INFO("log_ring_hr.ring_buffer is full");
        return GUNTER_SUCCESS;
    }

    ring_ret = ring_buffer_write(&log_ring_hr.ring_buffer, (uint8_t*)p_hr, sizeof(log_hr_t));
    if (ring_ret != sizeof(log_hr_t)) {
        APP_LOG_ERROR("ring_buffer_write failed with %#.4x", ring_ret);
        return GUNTER_FAILURE;
    }

    log_ring_hr.log_cnt++;

    return GUNTER_SUCCESS;
}

int user_3x2x_log_cache_hrv(log_hrv_t* p_hrv) {
    uint32_t ring_ret = 0;

    if (ring_buffer_is_reach_left_threshold(&log_ring_hrv.ring_buffer, sizeof(log_hrv_t))) {
        APP_LOG_INFO("log_ring_hrv.ring_buffer is full");
        return GUNTER_SUCCESS;
    }

    ring_ret = ring_buffer_write(&log_ring_hrv.ring_buffer, (uint8_t*)p_hrv, sizeof(log_hrv_t));
    if (ring_ret != sizeof(log_hrv_t)) {
        APP_LOG_ERROR("ring_buffer_write failed with %#.4x", ring_ret);
        return GUNTER_FAILURE;
    }

    log_ring_hrv.log_cnt++;

    return GUNTER_SUCCESS;
}

int user_3x2x_log_cache_spo2(log_spo2_t* p_spo2) {
    uint32_t ring_ret = 0;

    if (ring_buffer_is_reach_left_threshold(&log_ring_spo2.ring_buffer, sizeof(log_spo2_t))) {
        APP_LOG_INFO("log_ring_spo2.ring_buffer is full");
        return GUNTER_SUCCESS;
    }

    ring_ret = ring_buffer_write(&log_ring_spo2.ring_buffer, (uint8_t*)p_spo2, sizeof(log_spo2_t));
    if (ring_ret != sizeof(log_spo2_t)) {
        APP_LOG_ERROR("ring_buffer_write failed with %#.4x", ring_ret);
        return GUNTER_FAILURE;
    }

    log_ring_spo2.log_cnt++;

    return GUNTER_SUCCESS;
}

int user_3x2x_log_cache_adt(log_adt_t* p_adt) {
    uint32_t ring_ret = 0;

    if (ring_buffer_is_reach_left_threshold(&log_ring_adt.ring_buffer, sizeof(log_adt_t))) {
        APP_LOG_INFO("log_ring_adt.ring_buffer is full");
        return GUNTER_SUCCESS;
    }

    ring_ret = ring_buffer_write(&log_ring_adt.ring_buffer, (uint8_t*)p_adt, sizeof(log_adt_t));
    if (ring_ret != sizeof(log_adt_t)) {
        APP_LOG_ERROR("ring_buffer_write failed with %#.4x", ring_ret);
        return GUNTER_FAILURE;
    }

    log_ring_adt.log_cnt++;

    return GUNTER_SUCCESS;
}

int user_3x2x_log_print_hr(void) {
    uint32_t ring_ret = 0;
    log_hr_t log_hr;

    if (log_ring_hr.log_cnt == 0) {
        // APP_LOG_INFO("log_ring_hr is empty");
        return GUNTER_SUCCESS;
    }

    for (int i = 0; i < log_ring_hr.log_cnt; i++) {
        ring_ret = ring_buffer_read(&log_ring_hr.ring_buffer, (uint8_t*)&log_hr, sizeof(log_hr_t));
        if (ring_ret != sizeof(log_hr_t)) {
            APP_LOG_ERROR("ring_buffer_read failed with %#.4x", ring_ret);
            return GUNTER_FAILURE;
        }

        APP_LOG_INFO("hr: %d, confidence: %d", log_hr.hr, log_hr.confidence);
    }

    log_ring_hr.log_cnt = 0;

    return GUNTER_SUCCESS;
}

int user_3x2x_log_print_hrv(void) {
    uint32_t  ring_ret = 0;
    log_hrv_t log_hrv;

    if (log_ring_hrv.log_cnt == 0) {
        // APP_LOG_INFO("log_ring_hrv is empty");
        return GUNTER_SUCCESS;
    }

    for (int i = 0; i < log_ring_hrv.log_cnt; i++) {
        ring_ret = ring_buffer_read(&log_ring_hrv.ring_buffer, (uint8_t*)&log_hrv, sizeof(log_hrv_t));
        if (ring_ret != sizeof(log_hrv_t)) {
            APP_LOG_ERROR("ring_buffer_read failed with %#.4x", ring_ret);
            return GUNTER_FAILURE;
        }

        APP_LOG_INFO("rri_0: %d, rri_1: %d, rri_2: %d, rri_3: %d, num: %d, confidence: %d",
                     log_hrv.rri_0, log_hrv.rri_1, log_hrv.rri_2, log_hrv.rri_3, log_hrv.num, log_hrv.confidence);
    }

    log_ring_hrv.log_cnt = 0;

    return GUNTER_SUCCESS;
}

int user_3x2x_log_print_spo2(void) {
    uint32_t   ring_ret = 0;
    log_spo2_t log_spo2;

    if (log_ring_spo2.log_cnt == 0) {
        // APP_LOG_INFO("log_ring_spo2 is empty");
        return GUNTER_SUCCESS;
    }

    for (int i = 0; i < log_ring_spo2.log_cnt; i++) {
        ring_ret = ring_buffer_read(&log_ring_spo2.ring_buffer, (uint8_t*)&log_spo2, sizeof(log_spo2_t));
        if (ring_ret != sizeof(log_spo2_t)) {
            APP_LOG_ERROR("ring_buffer_read failed with %#.4x", ring_ret);
            return GUNTER_FAILURE;
        }

        APP_LOG_INFO("spo2: %d%%, confidence: %d", log_spo2.spo2, log_spo2.confidence);
    }

    log_ring_spo2.log_cnt = 0;

    return GUNTER_SUCCESS;
}

int user_3x2x_log_print_adt(void) {
    uint32_t  ring_ret = 0;
    log_adt_t log_adt;

    if (log_ring_adt.log_cnt == 0) {
        // APP_LOG_INFO("log_ring_adt is empty");
        return GUNTER_SUCCESS;
    }

    for (int i = 0; i < log_ring_adt.log_cnt; i++) {
        ring_ret = ring_buffer_read(&log_ring_adt.ring_buffer, (uint8_t*)&log_adt, sizeof(log_adt_t));
        if (ring_ret != sizeof(log_adt_t)) {
            APP_LOG_ERROR("ring_buffer_read failed with %#.4x", ring_ret);
            return GUNTER_FAILURE;
        }

        APP_LOG_INFO("adt: %d, confidence: %d", log_adt.adt, log_adt.confidence);
    }

    log_ring_adt.log_cnt = 0;

    return GUNTER_SUCCESS;
}
