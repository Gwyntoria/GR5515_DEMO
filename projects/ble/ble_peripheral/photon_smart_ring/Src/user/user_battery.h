#ifndef USER_BATTERY_H
#define USER_BATTERY_H

#include "STNS01/stns01.h"

#define VOL_CHAN_CHARGING    STNS01_CHANNEL_CHARGING
#define VOL_CHAN_DISCHARGING STNS01_CHANNEL_BATTERY

/**
 * @defgroup BATTERY_CHARGING_VOLTAGE
 * 
 */
#define BAT_VOL_CHARGE_FULL  (4.300L) // 100
#define BAT_VOL_CHARGE_HIGH  (4.121L) // 80
#define BAT_VOL_CHARGE_LOW   (3.837L) // 20
#define BAT_VOL_CHARGE_EMPTY (3.450) // 0

/**
 * @defgroup BATTERY_DISCHARGING_VOLTAGE
 * 
 */
#define BAT_VOL_DISCHARGE_FULL  (4.162L) // 100
#define BAT_VOL_DISCHARGE_HIGH  (3.943L) // 80
#define BAT_VOL_DISCHARGE_LOW   (3.758L) // 20
#define BAT_VOL_DISCHARGE_EMPTY (3.350L) // 0

/**
 * @defgroup BATTERY_LEVEL
 * 
 */
#define BATTERY_LEVEL_FULL   (0x00U)
#define BATTERY_LEVEL_HIGH   (0x01U)
#define BATTERY_LEVEL_NORMAL (0x02U)
#define BATTERY_LEVEL_LOW    (0x03U)
#define BATTERY_LEVEL_EMPTY  (0x04U)

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

uint16_t get_charging_battery_level();
uint16_t get_discharging_battery_level();

uint16_t get_battery_percentage(uint8_t channel);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif