/**
 * @file ST25DV_Reg.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _ST25DV_REG_H_
#define _ST25DV_REG_H_

/**
 * @brief ST25DV address
 * 
 */
/** @brief ST25DV user memory address. */
#define ST25DV_User_ADD                     0x53
/** @brief ST25DV system area address. */
#define ST25DV_SYS_ADD                      0x57
/** @brief ST25DV max write byte. */
#define ST25DV_MAX_WRITE_BYTE               256


/**
 * @brief ST25DV Unique identifier
 */
#define ST25DV04K_IE_UID    		        0x24
#define ST25DV64K_IE_UID    		        0x26
#define ST25DV04K_JF_UID    		        0x25
#define ST25DV16K_JF_UID    		        0x27
#define ST25DV64K_JF_UID    		        0x27
#define ST25DV04KC_IE_UID 			        0x50
#define ST25DV64KC_IE_UID			        0x51
#define ST25DV04KC_JF_UID			        0x52
#define ST25DV64KC_JF_UID			        0x53


/* Exported constants --------------------------------------------------------*/
/* Registers i2c address */
#define ST25DV_Reg_len					    2
/** @brief ST25DV GPO register address. */
#define ST25DV_GPO_REG                      0x0000
/** @brief ST25DV IT duration register address. */
#define ST25DV_ITTIME_REG                   0x0001
/** @brief ST25DV Energy Harvesting register address. */
#define ST25DV_EH_MODE_REG                  0x0002
/** @brief ST25DV RF management register address. */
#define ST25DV_RF_MNGT_REG                  0x0003
/** @brief ST25DV Area 1 security register address. */
#define ST25DV_RFA1SS_REG                   0x0004
/** @brief ST25DV Area 1 end address register address. */
#define ST25DV_ENDA1_REG                    0x0005
/** @brief ST25DV Area 2 security register address. */
#define ST25DV_RFA2SS_REG                   0x0006
/** @brief ST25DV Area 2 end address register address. */
#define ST25DV_ENDA2_REG                    0x0007
/** @brief ST25DV Area 3 security register address. */
#define ST25DV_RFA3SS_REG                   0x0008
/** @brief ST25DV Area 3 end address register address. */
#define ST25DV_ENDA3_REG                    0x0009
/** @brief ST25DV Area 4 security register address. */
#define ST25DV_RFA4SS_REG                   0x000A
/** @brief ST25DV I2C security register address. */
#define ST25DV_I2CSS_REG                    0x000B
/** @brief ST25DV Capability Container lock register address. */
#define ST25DV_LOCKCCFILE_REG               0x000C
/** @brief ST25DV Mailbox mode register address. */
#define ST25DV_MB_MODE_REG                  0x000D
/** @brief ST25DV Mailbox Watchdog register address. */
#define ST25DV_MB_WDG_REG                   0x000E
/** @brief ST25DV Configuration lock register address. */
#define ST25DV_LOCKCFG_REG                  0x000F
/** @brief ST25DV DSFID lock register address. */
#define ST25DV_LOCKDSFID_REG                0x0010
/** @brief ST25DV AFI lock register address. */
#define ST25DV_LOCKAFI_REG                  0x0011
/** @brief ST25DV DSFID register address. */
#define ST25DV_DSFID_REG                    0x0012
/** @brief ST25DV AFI register address. */
#define ST25DV_AFI_REG                      0x0013
/** @brief ST25DV Memory size register address. */
#define ST25DV_MEM_SIZE_LSB_REG             0x0014
/** @brief ST25DV Memory size register address. */
#define ST25DV_MEM_SIZE_MSB_REG             0x0015
/** @brief ST25DV Block size register address. */
#define ST25DV_BLK_SIZE_REG                 0x0016
//** @brief ST25DV ICref register address. */
#define ST25DV_ICREF_REG                    0x0017
/** @brief ST25DV UID register address. */
#define ST25DV_UID_REG                      0x0018
/** @brief ST25DV IC revision register address. */
#define ST25DV_ICREV_REG                    0x0020
/** @brief ST25DV I2C password register address. */
#define ST25DV_I2CPASSWD_REG                0x0900

/* Dynamic Registers i2c address */
/** @brief ST25DV GPO dynamic register address. */
#define ST25DV_GPO_DYN_REG                  0x2000
/** @brief ST25DV Energy Harvesting control dynamic register address. */
#define ST25DV_EH_CTRL_DYN_REG              0x2002
/** @brief ST25DV RF management dynamic register address. */
#define ST25DV_RF_MNGT_DYN_REG              0x2003
/** @brief ST25DV I2C secure session opened dynamic register address. */
#define ST25DV_I2C_SSO_DYN_REG              0x2004
/** @brief ST25DV Interrupt status dynamic register address. */
#define ST25DV_ITSTS_DYN_REG                0x2005
/** @brief ST25DV Mailbox control dynamic register address. */
#define ST25DV_MB_CTRL_DYN_REG              0x2006
/** @brief ST25DV Mailbox message length dynamic register address. */
#define ST25DV_MBLEN_DYN_REG                0x2007
/** @brief ST25DV Mailbox buffer address. */
#define ST25DV_MAILBOX_RAM_REG              0x2008

#endif
