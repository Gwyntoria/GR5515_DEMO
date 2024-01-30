/**
 * @file ST25DV.h
 * @author chen (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _ST25DV_H_
#define _ST25DV_H_

#include "stdint.h"


/** @brief ST25DV I2C ID Pick. */
#define ST25DV_I2C_ID                       APP_I2C_ID_0

#define WLCCAP_NDEF_PROTOCOL_VERSION        0x01
/* Registers fields definitions */
/* MB_MODE */
#define ST25DV_MB_MODE_RW_SHIFT              (0)
#define ST25DV_MB_MODE_RW_FIELD              0xFE
#define ST25DV_MB_MODE_RW_MASK               0x01

/* MB_LEN_Dyn */
#define ST25DV_MBLEN_DYN_MBLEN_SHIFT         (0)
#define ST25DV_MBLEN_DYN_MBLEN_FIELD         0x00
#define ST25DV_MBLEN_DYN_MBLEN_MASK          0xFF

/* MB_CTRL_Dyn */
#define ST25DV_MB_CTRL_DYN_MBEN_SHIFT        (0)
#define ST25DV_MB_CTRL_DYN_MBEN_FIELD        0xFE
#define ST25DV_MB_CTRL_DYN_MBEN_MASK         0x01
#define ST25DV_MB_CTRL_DYN_HOSTPUTMSG_SHIFT  (1)
#define ST25DV_MB_CTRL_DYN_HOSTPUTMSG_FIELD  0xFD
#define ST25DV_MB_CTRL_DYN_HOSTPUTMSG_MASK   0x02
#define ST25DV_MB_CTRL_DYN_RFPUTMSG_SHIFT    (2)
#define ST25DV_MB_CTRL_DYN_RFPUTMSG_FIELD    0xFB
#define ST25DV_MB_CTRL_DYN_RFPUTMSG_MASK     0x04
#define ST25DV_MB_CTRL_DYN_STRESERVED_SHIFT  (3)
#define ST25DV_MB_CTRL_DYN_STRESERVED_FIELD  0xF7
#define ST25DV_MB_CTRL_DYN_STRESERVED_MASK   0x08
#define ST25DV_MB_CTRL_DYN_HOSTMISSMSG_SHIFT (4)
#define ST25DV_MB_CTRL_DYN_HOSTMISSMSG_FIELD 0xEF
#define ST25DV_MB_CTRL_DYN_HOSTMISSMSG_MASK  0x10
#define ST25DV_MB_CTRL_DYN_RFMISSMSG_SHIFT   (5)
#define ST25DV_MB_CTRL_DYN_RFMISSMSG_FIELD   0xDF
#define ST25DV_MB_CTRL_DYN_RFMISSMSG_MASK    0x20
#define ST25DV_MB_CTRL_DYN_CURRENTMSG_SHIFT  (6)
#define ST25DV_MB_CTRL_DYN_CURRENTMSG_FIELD  0x3F
#define ST25DV_MB_CTRL_DYN_CURRENTMSG_MASK   0xC0


/* MB_WDG */
#define ST25DV_MB_WDG_DELAY_SHIFT            (0)
#define ST25DV_MB_WDG_DELAY_FIELD            0xF8
#define ST25DV_MB_WDG_DELAY_MASK             0x07

/* GPO */
#define ST25DV_GPO_RFUSERSTATE_SHIFT         (0)
#define ST25DV_GPO_RFUSERSTATE_FIELD         0xFE
#define ST25DV_GPO_RFUSERSTATE_MASK          0x01
#define ST25DV_GPO_RFACTIVITY_SHIFT          (1)
#define ST25DV_GPO_RFACTIVITY_FIELD          0xFD
#define ST25DV_GPO_RFACTIVITY_MASK           0x02
#define ST25DV_GPO_RFINTERRUPT_SHIFT         (2)
#define ST25DV_GPO_RFINTERRUPT_FIELD         0xFB
#define ST25DV_GPO_RFINTERRUPT_MASK          0x04
#define ST25DV_GPO_FIELDCHANGE_SHIFT         (3)
#define ST25DV_GPO_FIELDCHANGE_FIELD         0xF7
#define ST25DV_GPO_FIELDCHANGE_MASK          0x08
#define ST25DV_GPO_RFPUTMSG_SHIFT            (4)
#define ST25DV_GPO_RFPUTMSG_FIELD            0xEF
#define ST25DV_GPO_RFPUTMSG_MASK             0x10
#define ST25DV_GPO_RFGETMSG_SHIFT            (5)
#define ST25DV_GPO_RFGETMSG_FIELD            0xDF
#define ST25DV_GPO_RFGETMSG_MASK             0x20
#define ST25DV_GPO_RFWRITE_SHIFT             (6)
#define ST25DV_GPO_RFWRITE_FIELD             0xBF
#define ST25DV_GPO_RFWRITE_MASK              0x40
#define ST25DV_GPO_ENABLE_SHIFT              (7)
#define ST25DV_GPO_ENABLE_FIELD              0x7F
#define ST25DV_GPO_ENABLE_MASK               0x80
#define ST25DV_GPO_ALL_SHIFT                 (0)
#define ST25DV_GPO_ALL_MASK                  0xFF

/* GPO_Dyn */
#define ST25DV_GPO_DYN_RFUSERSTATE_SHIFT     (0)
#define ST25DV_GPO_DYN_RFUSERSTATE_FIELD     0xFE
#define ST25DV_GPO_DYN_RFUSERSTATE_MASK      0x01
#define ST25DV_GPO_DYN_RFACTIVITY_SHIFT      (1)
#define ST25DV_GPO_DYN_RFACTIVITY_FIELD      0xFD
#define ST25DV_GPO_DYN_RFACTIVITY_MASK       0x02
#define ST25DV_GPO_DYN_RFINTERRUPT_SHIFT     (2)
#define ST25DV_GPO_DYN_RFINTERRUPT_FIELD     0xFB
#define ST25DV_GPO_DYN_RFINTERRUPT_MASK      0x04
#define ST25DV_GPO_DYN_FIELDCHANGE_SHIFT     (3)
#define ST25DV_GPO_DYN_FIELDCHANGE_FIELD     0xF7
#define ST25DV_GPO_DYN_FIELDCHANGE_MASK      0x08
#define ST25DV_GPO_DYN_RFPUTMSG_SHIFT        (4)
#define ST25DV_GPO_DYN_RFPUTMSG_FIELD        0xEF
#define ST25DV_GPO_DYN_RFPUTMSG_MASK         0x10
#define ST25DV_GPO_DYN_RFGETMSG_SHIFT        (5)
#define ST25DV_GPO_DYN_RFGETMSG_FIELD        0xDF
#define ST25DV_GPO_DYN_RFGETMSG_MASK         0x20
#define ST25DV_GPO_DYN_RFWRITE_SHIFT         (6)
#define ST25DV_GPO_DYN_RFWRITE_FIELD         0xBF
#define ST25DV_GPO_DYN_RFWRITE_MASK          0x40
#define ST25DV_GPO_DYN_ENABLE_SHIFT          (7)
#define ST25DV_GPO_DYN_ENABLE_FIELD          0x7F
#define ST25DV_GPO_DYN_ENABLE_MASK           0x80
#define ST25DV_GPO_DYN_ALL_SHIFT             (0)
#define ST25DV_GPO_DYN_ALL_MASK              0xFF

/* ITTIME */
#define ST25DV_ITTIME_DELAY_SHIFT            (0)
#define ST25DV_ITTIME_DELAY_FIELD            0xFC
#define ST25DV_ITTIME_DELAY_MASK             0x03

/* ITSTS_Dyn */
#define ST25DV_ITSTS_DYN_RFUSERSTATE_SHIFT       (0)
#define ST25DV_ITSTS_DYN_RFUSERSTATE_FIELD       0xFE
#define ST25DV_ITSTS_DYN_RFUSERSTATE_MASK        0x01
#define ST25DV_ITSTS_DYN_RFACTIVITY_SHIFT        (1)
#define ST25DV_ITSTS_DYN_RFACTIVITY_FIELD        0xFD
#define ST25DV_ITSTS_DYN_RFACTIVITY_MASK         0x02
#define ST25DV_ITSTS_DYN_RFINTERRUPT_SHIFT       (2)
#define ST25DV_ITSTS_DYN_RFINTERRUPT_FIELD       0xFB
#define ST25DV_ITSTS_DYN_RFINTERRUPT_MASK        0x04
#define ST25DV_ITSTS_DYN_FIELDFALLING_SHIFT      (3)
#define ST25DV_ITSTS_DYN_FIELDFALLING_FIELD      0xF7
#define ST25DV_ITSTS_DYN_FIELDFALLING_MASK       0x08
#define ST25DV_ITSTS_DYN_FIELDRISING_SHIFT       (4)
#define ST25DV_ITSTS_DYN_FIELDRISING_FIELD       0xEF
#define ST25DV_ITSTS_DYN_FIELDRISING_MASK        0x10
#define ST25DV_ITSTS_DYN_RFPUTMSG_SHIFT          (5)
#define ST25DV_ITSTS_DYN_RFPUTMSG_FIELD          0xDF
#define ST25DV_ITSTS_DYN_RFPUTMSG_MASK           0x20
#define ST25DV_ITSTS_DYN_RFGETMSG_SHIFT          (6)
#define ST25DV_ITSTS_DYN_RFGETMSG_FIELD          0xBF
#define ST25DV_ITSTS_DYN_RFGETMSG_MASK           0x40
#define ST25DV_ITSTS_DYN_RFWRITE_SHIFT           (7)
#define ST25DV_ITSTS_DYN_RFWRITE_FIELD           0x7F
#define ST25DV_ITSTS_DYN_RFWRITE_MASK            0x80

/* EH_MODE */
#define ST25DV_EH_MODE_SHIFT                 (0)
#define ST25DV_EH_MODE_FIELD                 0xFE
#define ST25DV_EH_MODE_MASK                  0x01

/* EH_CTRL_Dyn */
#define ST25DV_EH_CTRL_DYN_EH_EN_SHIFT       (0)
#define ST25DV_EH_CTRL_DYN_EH_EN_FIELD       0xFE
#define ST25DV_EH_CTRL_DYN_EH_EN_MASK        0x01
#define ST25DV_EH_CTRL_DYN_EH_ON_SHIFT       (1)
#define ST25DV_EH_CTRL_DYN_EH_ON_FIELD       0xFD
#define ST25DV_EH_CTRL_DYN_EH_ON_MASK        0x02
#define ST25DV_EH_CTRL_DYN_FIELD_ON_SHIFT    (2)
#define ST25DV_EH_CTRL_DYN_FIELD_ON_FIELD    0xFB
#define ST25DV_EH_CTRL_DYN_FIELD_ON_MASK     0x04
#define ST25DV_EH_CTRL_DYN_VCC_ON_SHIFT      (3)
#define ST25DV_EH_CTRL_DYN_VCC_ON_FIELD      0xF7
#define ST25DV_EH_CTRL_DYN_VCC_ON_MASK       0x08
#define ST25DV_EH_CTRL_DYN_ALL_SHIFT         (0)
#define ST25DV_EH_CTRL_DYN_ALL_MASK          0x0F

/* RF_MNGT */
#define ST25DV_RF_MNGT_RFDIS_SHIFT           (0)
#define ST25DV_RF_MNGT_RFDIS_FIELD           0xFE
#define ST25DV_RF_MNGT_RFDIS_MASK            0x01
#define ST25DV_RF_MNGT_RFSLEEP_SHIFT         (1)
#define ST25DV_RF_MNGT_RFSLEEP_FIELD         0xFD
#define ST25DV_RF_MNGT_RFSLEEP_MASK          0x02
#define ST25DV_RF_MNGT_ALL_SHIFT             (0)
#define ST25DV_RF_MNGT_ALL_MASK              0x03

/* RF_MNGT_Dyn */
#define ST25DV_RF_MNGT_DYN_RFDIS_SHIFT       (0)
#define ST25DV_RF_MNGT_DYN_RFDIS_FIELD       0xFE
#define ST25DV_RF_MNGT_DYN_RFDIS_MASK        0x01
#define ST25DV_RF_MNGT_DYN_RFSLEEP_SHIFT     (1)
#define ST25DV_RF_MNGT_DYN_RFSLEEP_FIELD     0xFD
#define ST25DV_RF_MNGT_DYN_RFSLEEP_MASK      0x02
#define ST25DV_RF_MNGT_DYN_ALL_SHIFT         (0)
#define ST25DV_RF_MNGT_DYN_ALL_MASK          0x03

/* RFA1SS */
#define ST25DV_RFA1SS_PWDCTRL_SHIFT           (0)
#define ST25DV_RFA1SS_PWDCTRL_FIELD           0xFC
#define ST25DV_RFA1SS_PWDCTRL_MASK            0x03
#define ST25DV_RFA1SS_RWPROT_SHIFT            (2)
#define ST25DV_RFA1SS_RWPROT_FIELD            0xF3
#define ST25DV_RFA1SS_RWPROT_MASK             0x0C
#define ST25DV_RFA1SS_ALL_SHIFT               (0)
#define ST25DV_RFA1SS_ALL_MASK                0x0F

/* RFA2SS */
#define ST25DV_RFA2SS_PWDCTRL_SHIFT           (0)
#define ST25DV_RFA2SS_PWDCTRL_FIELD           0xFC
#define ST25DV_RFA2SS_PWDCTRL_MASK            0x03
#define ST25DV_RFA2SS_RWPROT_SHIFT            (2)
#define ST25DV_RFA2SS_RWPROT_FIELD            0xF3
#define ST25DV_RFA2SS_RWPROT_MASK             0x0C
#define ST25DV_RFA2SS_ALL_SHIFT               (0)
#define ST25DV_RFA2SS_ALL_MASK                0x0F

/* RFA3SS */
#define ST25DV_RFA3SS_PWDCTRL_SHIFT           (0)
#define ST25DV_RFA3SS_PWDCTRL_FIELD           0xFC
#define ST25DV_RFA3SS_PWDCTRL_MASK            0x03
#define ST25DV_RFA3SS_RWPROT_SHIFT            (2)
#define ST25DV_RFA3SS_RWPROT_FIELD            0xF3
#define ST25DV_RFA3SS_RWPROT_MASK             0x0C
#define ST25DV_RFA3SS_ALL_SHIFT               (0)
#define ST25DV_RFA3SS_ALL_MASK                0x0F

/* RFA4SS */
#define ST25DV_RFA4SS_PWDCTRL_SHIFT           (0)
#define ST25DV_RFA4SS_PWDCTRL_FIELD           0xFC
#define ST25DV_RFA4SS_PWDCTRL_MASK            0x03
#define ST25DV_RFA4SS_RWPROT_SHIFT            (2)
#define ST25DV_RFA4SS_RWPROT_FIELD            0xF3
#define ST25DV_RFA4SS_RWPROT_MASK             0x0C
#define ST25DV_RFA4SS_ALL_SHIFT               (0)
#define ST25DV_RFA4SS_ALL_MASK                0x0F

/* I2CSS */
#define ST25DV_I2CSS_PZ1_SHIFT              (0)
#define ST25DV_I2CSS_PZ1_FIELD              0xFC
#define ST25DV_I2CSS_PZ1_MASK               0x03
#define ST25DV_I2CSS_PZ2_SHIFT              (2)
#define ST25DV_I2CSS_PZ2_FIELD              0xF3
#define ST25DV_I2CSS_PZ2_MASK               0x0C
#define ST25DV_I2CSS_PZ3_SHIFT              (4)
#define ST25DV_I2CSS_PZ3_FIELD              0xCF
#define ST25DV_I2CSS_PZ3_MASK               0x30
#define ST25DV_I2CSS_PZ4_SHIFT              (6)
#define ST25DV_I2CSS_PZ4_FIELD              0x3F
#define ST25DV_I2CSS_PZ4_MASK               0xC0

/* LOCKCCFILE */
#define ST25DV_LOCKCCFILE_BLCK0_SHIFT        (0)
#define ST25DV_LOCKCCFILE_BLCK0_FIELD        0xFE
#define ST25DV_LOCKCCFILE_BLCK0_MASK         0x01
#define ST25DV_LOCKCCFILE_BLCK1_SHIFT        (1)
#define ST25DV_LOCKCCFILE_BLCK1_FIELD        0xFD
#define ST25DV_LOCKCCFILE_BLCK1_MASK         0x02
#define ST25DV_LOCKCCFILE_ALL_SHIFT          (0)
#define ST25DV_LOCKCCFILE_ALL_MASK           0x03

/* LOCKCFG */
#define ST25DV_LOCKCFG_B0_SHIFT              (0)
#define ST25DV_LOCKCFG_B0_FIELD              0xFE
#define ST25DV_LOCKCFG_B0_MASK               0x01

/* I2C_SSO_Dyn */
#define ST25DV_I2C_SSO_DYN_I2CSSO_SHIFT      (0)
#define ST25DV_I2C_SSO_DYN_I2CSSO_FIELD      0xFE
#define ST25DV_I2C_SSO_DYN_I2CSSO_MASK       0x01

/**
 * @brief  NFCTAG status enumerator definition.
 */
#define NFCTAG_OK      (0)
#define NFCTAG_ERROR   (-1)
#define NFCTAG_BUSY    (-2)
#define NFCTAG_TIMEOUT (-3)
#define NFCTAG_NACK    (-102)



int8_t ST25DV_ReadUid(void);
int8_t ST25DV_WriteData(uint16_t User_memory_add, uint8_t *pData, uint16_t size);
int8_t ST25DV_ReadData(uint16_t User_memory_add, uint8_t *pData, uint16_t size);
int8_t ST25DV_ReadRegData(uint16_t Reg_add, uint8_t * pData, uint16_t size);

#endif
