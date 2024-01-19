/**
 *****************************************************************************************
 *
 * @file gus.h
 *
 * @brief Goodix UART Service API
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/**
 * @addtogroup BLE_SRV BLE Services
 * @{
 * @brief Definitions and prototypes for the BLE Service interface.
 */

/**
 * @defgroup BLE_SDK_HEALTH Goodix UART Service (HEALTH)
 * @{
 * @brief Definitions and prototypes for the HEALTH interface.
 */

#ifndef __HEALTH_H__
#define __HEALTH_H__

#include "custom_config.h"
#include "gr55xx_sys.h"

/**
 * @defgroup HEALTH_MACRO Defines
 * @{
 */
#define HEALTH_CONNECTION_MAX \
    (10 < CFG_MAX_CONNECTIONS ? 10 : CFG_MAX_CONNECTIONS) /**< Maximum number of Goodix UART Service connections. */
#define FLOW_ON              0x01                         /**< Indicate that HEALTH can receive data from peer. */
#define FLOW_OFF             0x00                         /**< Indicate that HEALTH can not receive data from peer. */
#define HEALTH_MAX_DATA_LEN  247                          /**< Maximum length of application data packet which is transmitted via HEALTH. */
#define HEALTH_FLOW_CTRL_LEN 1                            /**< Maximum length of ble flow control data packet which is transmitted via HEALTH. */
#define HEALTH_SERVICE_UUID  0xFB, 0x34, 0x9B, 0x5f, 0x80, 0x00, 0x00, 0x80, \
                             0x00, 0x10, 0x00, 0x00, 0x0E, 0x19, 0x00, 0x01 /**< The UUID of Goodix UART Service for setting advertising data. */

/** @} */

/**
 * @defgroup HEALTH_ENUM Enumerations
 * @{
 */
/**@brief Goodix UART Service event types. */
typedef enum {
    HEALTH_EVT_INVALID, /**< Invalid HEALTH event. */

    // HEALTH_EVT_TX_PORT_OPENED,   /**< Tx port has been opened. */
    // HEALTH_EVT_TX_PORT_CLOSED,   /**< Tx port has been closed. */
    // HEALTH_EVT_TX_DATA_SENT,     /**< The data from the application has been sent, and the service is ready to accept new
    //                                 data from the application. */

    HEALTH_EVT_HR_PORT_OPENED,
    HEALTH_EVT_HR_PORT_CLOSED,
    HEALTH_EVT_HR_DATA_SENT,

    HEALTH_EVT_HRV_PORT_OPENED,
    HEALTH_EVT_HRV_PORT_CLOSED,
    HEALTH_EVT_HRV_DATA_SENT,

    HEALTH_EVT_SPO2_PORT_OPENED,
    HEALTH_EVT_SPO2_PORT_CLOSED,
    HEALTH_EVT_SPO2_DATA_SENT,

    HEALTH_EVT_RR_PORT_OPENED,
    HEALTH_EVT_RR_PORT_CLOSED,
    HEALTH_EVT_RR_DATA_SENT,

    HEALTH_EVT_RX_DATA_RECEIVED, /**< The data from the peer has been received. */
} health_evt_type_t;
/** @} */

/**
 * @defgroup HEALTH_STRUCT Structures
 * @{
 */
/**@brief Goodix UART Service event. */
typedef struct {
    health_evt_type_t evt_type; /**< The HEALTH event. */
    uint8_t           conn_idx; /**< The index of the connection for the data transmission. */
    uint8_t*          p_data;   /**< Pointer to the buffer within received data. */
    uint16_t          length;   /**< Length of received data. */
} health_evt_t;
/** @} */

/**
 * @defgroup HEALTH_TYPEDEF Typedefs
 * @{
 */
/**@brief Goodix UART Service event handler type. */
typedef void (*health_evt_handler_t)(health_evt_t* p_evt);
/** @} */

/**
 * @addtogroup HEALTH_STRUCT Structures
 * @{
 */
/**@brief Goodix UART Service init stucture. This contains all option and data needed for initialization of the service.
 */
typedef struct {
    health_evt_handler_t evt_handler; /**< Goodix UART Service event handler which must be provided by the application
                                         to send and receive the data. */
} health_init_t;
/** @} */

/**
 * @defgroup HEALTH_FUNCTION Functions
 * @{
 */
/**
 *****************************************************************************************
 * @brief Initialize a Goodix UART Service instance and add in the database.
 *
 * @param[in] p_health_init: Pointer to Goodix UART Service initialization variables.
 *
 * @return Result of service initialization.
 *****************************************************************************************
 */
sdk_err_t health_service_init(health_init_t* p_health_init);

/**
 *****************************************************************************************
 * @brief Send heart rate data to peer device.
 *
 * @param[in] conn_idx: Index of the connection.
 * @param[in] p_data:   Pointer to sent data.
 * @param[in] length:   Length of sent data.
 *
 * @return Result of sending data.
 *****************************************************************************************
 */
sdk_err_t health_hr_data_send(uint8_t conn_idx, uint8_t* p_data, uint16_t length);

/**
 *****************************************************************************************
 * @brief Send heart rate data to peer device.
 *
 * @param[in] conn_idx: Index of the connection.
 * @param[in] p_data:   Pointer to sent data.
 * @param[in] length:   Length of sent data.
 *
 * @return Result of sending data.
 *****************************************************************************************
 */
sdk_err_t health_hrv_data_send(uint8_t conn_idx, uint8_t* p_data, uint16_t length);

/**
 *****************************************************************************************
 * @brief Send spo2 data to peer device.
 *
 * @param[in] conn_idx: Index of the connection.
 * @param[in] p_data:   Pointer to sent data.
 * @param[in] length:   Length of sent data.
 *
 * @return Result of sending data.
 *****************************************************************************************
 */
sdk_err_t health_spo2_data_send(uint8_t conn_idx, uint8_t* p_data, uint16_t length);

/**
 *****************************************************************************************
 * @brief Send rr data to peer device.
 *
 * @param[in] conn_idx: Index of the connection.
 * @param[in] p_data:   Pointer to sent data.
 * @param[in] length:   Length of sent data.
 *
 * @return Result of sending data.
 *****************************************************************************************
 */
sdk_err_t health_rr_data_send(uint8_t conn_idx, uint8_t* p_data, uint16_t length);

/** @} */

#endif
/** @} */
/** @} */
