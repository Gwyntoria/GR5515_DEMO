/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_reg_array.c
 * 
 * @brief   gh3x2x driver lib reg config array
 * 
 * @author  Gooidx Iot Team
 * 
 */

#include "gh3x2x_demo_config.h"
#include "gh3x2x_demo_inner.h"



const STGh3x2xReg gh3x2x_reg_list0[] =
{
#define S003_USE 0
#define EVK_USE 1
#if S003_USE
    {0x0004, 0x001F}, {0x0006, 0x0005}, {0x000A, 0x00C8}, {0x000E, 0x000D}, {0x0100, 0x0100}, {0x0102, 0x0302},
    {0x0104, 0x0504}, {0x010A, 0x277C}, {0x0110, 0x0F14}, {0x0112, 0x5555}, {0x0114, 0x00AA}, {0x011E, 0x001F},
    {0x0120, 0x001F}, {0x0122, 0x0CFF}, {0x0126, 0x2744}, {0x0128, 0x0001}, {0x012A, 0x0000}, {0x012C, 0x0D15},
    {0x012E, 0x4442}, {0x0130, 0x0002}, {0x013A, 0x031F}, {0x013C, 0x031F}, {0x013E, 0x0CFF}, {0x0142, 0x2744},
    {0x0144, 0x0001}, {0x0146, 0x0000}, {0x0148, 0x0D15}, {0x014A, 0x4442}, {0x014C, 0x0002}, {0x0156, 0x021F},
    {0x0158, 0x021F}, {0x015A, 0x0CFF}, {0x015E, 0x2744}, {0x0160, 0x0001}, {0x0162, 0x0000}, {0x0164, 0x0C10},
    {0x0166, 0x4442}, {0x0168, 0x0002}, {0x0172, 0x0207}, {0x0174, 0x0207}, {0x017A, 0x2744}, {0x017C, 0x0001},
    {0x017E, 0x0000}, {0x0180, 0x0000}, {0x0182, 0x4442}, {0x0184, 0x0002}, {0x018E, 0x0400}, {0x0190, 0x0400},
    {0x0196, 0xC704}, {0x0198, 0x0001}, {0x019A, 0x0000}, {0x019C, 0x0C10}, {0x019E, 0x4442}, {0x01A0, 0x0002},
    {0x01A6, 0x0003}, {0x01AA, 0x0207}, {0x01AC, 0x0207}, {0x01EC, 0x0122}, {0x01EE, 0x01BF}, {0x01F0, 0x01BF},
    {0x01F2, 0x0098}, {0x01F4, 0x001A}, {0x01F6, 0x0098}, {0x0200, 0x0120}, {0x0280, 0x0A00}, {0x0282, 0xF530},
    {0x0284, 0x9388}, {0x0286, 0xC45C}, {0x0288, 0xA328}, {0x0408, 0x0001}, {0x0410, 0xDDD0}, {0x0412, 0x0086},
    {0x0414, 0xD090}, {0x0416, 0x0083}, {0x0422, 0x0011}, {0x0424, 0x0901}, {0x0426, 0x0102}, {0x0428, 0x0102},
    {0x0502, 0x4C60}, {0x0504, 0x000A}, {0x0588, 0x0008}, {0x0692, 0x0233}, {0x0694, 0x0400}, {0x0698, 0x2310},
    {0x0700, 0x0000}, {0x1000, 0x0000}, {0x1002, 0x3197}, {0x1004, 0x0000}, {0x1006, 0x5682}, {0x1008, 0x2B12},
    {0x100A, 0x0001}, {0x10E0, 0x0202}, {0x10E2, 0x0202}, {0x10E4, 0x02FF}, {0x10E6, 0xFFFF}, {0x10E8, 0x0202},
    {0x10EA, 0x0202}, {0x10EC, 0x0202}, {0x10EE, 0x0202}, {0x1120, 0x00C8}, {0x1122, 0x0005}, {0x1124, 0x0000},
    {0x1140, 0x0000}, {0x1142, 0x0019}, {0x1144, 0x0001}, {0x1160, 0x0007}, {0x1162, 0x0000}, {0x1164, 0x0402},
    {0x1166, 0x4568}, {0x1168, 0x9940}, {0x116A, 0x00D9}, {0x116C, 0x8800}, {0x116E, 0x0093}, {0x1170, 0x5C00},
    {0x1172, 0x00C4}, {0x1174, 0x2800}, {0x1176, 0x00A3}, {0x2000, 0x0001}, {0x2002, 0x00A2}, {0x2022, 0x0004},
    {0x2024, 0x0901}, {0x2026, 0x1911}, {0x20CC, 0x0002}, {0x20CE, 0x4223}, {0x2132, 0x0003}, {0x2134, 0x6201},
    {0x2136, 0x0085}, {0x2880, 0x0005}, {0x2882, 0x0019}, {0x288C, 0x0019}, {0x2892, 0x0019}, {0x3000, 0x0401},
    {0x3002, 0x0000}, {0x3004, 0xDDD0}, {0x3006, 0x0086}, {0xFFFF, 0x0001}
#elif EVK_USE
    // {0x0004, 0x001F}, {0x0006, 0x0005}, {0x000A, 0x00C8}, {0x000E, 0x000D}, {0x0100, 0x0100}, {0x0102, 0x0302},
    // {0x0104, 0x0504}, {0x010A, 0x277C}, {0x010C, 0x0102}, {0x0110, 0x0F14}, {0x0112, 0x5555}, {0x0114, 0x00AA},
    // {0x011E, 0x0019}, {0x0122, 0x0699}, {0x0126, 0x2744}, {0x0128, 0x0002}, {0x012A, 0x0000}, {0x012C, 0x0D16},
    // {0x012E, 0x4442}, {0x0130, 0x0002}, {0x013A, 0x023F}, {0x013C, 0x0400}, {0x013E, 0x0699}, {0x0142, 0x2744},
    // {0x0144, 0x0002}, {0x0146, 0x0000}, {0x0148, 0x0D16}, {0x014A, 0x4442}, {0x014C, 0x0002}, {0x0156, 0x0133},
    // {0x0158, 0x0400}, {0x015A, 0x0699}, {0x015E, 0x2744}, {0x0160, 0x0002}, {0x0162, 0x0000}, {0x0164, 0x0C10},
    // {0x0166, 0x4442}, {0x0168, 0x0002}, {0x0172, 0x010A}, {0x0174, 0x0400}, {0x017A, 0x2744}, {0x017C, 0x0002},
    // {0x017E, 0x0000}, {0x0180, 0x0000}, {0x0182, 0x4442}, {0x0184, 0x0002}, {0x018E, 0x0400}, {0x0190, 0x0400},
    // {0x0196, 0xC704}, {0x0198, 0x0002}, {0x019A, 0x0000}, {0x019C, 0x0C10}, {0x019E, 0x4442}, {0x01A0, 0x0002},
    // {0x01A6, 0x0003}, {0x01AA, 0x010C}, {0x01AC, 0x0400}, {0x01EC, 0x0122}, {0x01EE, 0x02FA}, {0x01F0, 0x02FA},
    // {0x01F2, 0x0098}, {0x01F4, 0x001A}, {0x01F6, 0x0098}, {0x0200, 0x0120}, {0x0280, 0x0A00}, {0x0282, 0xF530},
    // {0x0284, 0x9388}, {0x0286, 0xC45C}, {0x0288, 0xA328}, {0x0408, 0x0001}, {0x0410, 0xDDD0}, {0x0412, 0x0086},
    // {0x0414, 0xD090}, {0x0416, 0x0083}, {0x0422, 0x0011}, {0x0424, 0x0901}, {0x0426, 0x0102}, {0x0428, 0x0102},
    // {0x0502, 0x4F60}, {0x0504, 0x000A}, {0x0588, 0x0008}, {0x0692, 0x0233}, {0x0694, 0x0400}, {0x0698, 0x2310},
    // {0x0700, 0x0000}, {0x1000, 0x0000}, {0x1002, 0x3197}, {0x1004, 0x0000}, {0x1006, 0xA4AB}, {0x1008, 0x2B1C},
    // {0x100A, 0x0001}, {0x10E0, 0x0002}, {0x10E2, 0x0000}, {0x10E4, 0x00FF}, {0x10E6, 0xFFFF}, {0x10E8, 0x0202},
    // {0x10EA, 0x0202}, {0x10EC, 0x0202}, {0x10EE, 0x0202}, {0x1120, 0x00C8}, {0x1122, 0x0005}, {0x1124, 0x0000},
    // {0x1140, 0x0000}, {0x1142, 0x00C8}, {0x1144, 0x0001}, {0x1160, 0x0007}, {0x1162, 0x0000}, {0x1164, 0x0402},
    // {0x1166, 0x4568}, {0x1168, 0x9940}, {0x116A, 0x00D9}, {0x116C, 0x8800}, {0x116E, 0x0093}, {0x1170, 0x5C00},
    // {0x1172, 0x00C4}, {0x1174, 0x2800}, {0x1176, 0x00A3}, {0x2000, 0x0001}, {0x2002, 0x00A2}, {0x2022, 0x0004},
    // {0x2024, 0x0901}, {0x2026, 0x1911}, {0x20CC, 0x0002}, {0x20CE, 0x4223}, {0x2132, 0x0003}, {0x2134, 0x6201},
    // {0x2136, 0x0085}, {0x2880, 0x0005}, {0x2882, 0x0019}, {0x288C, 0x0019}, {0x2892, 0x0019}, {0x3000, 0x0401},
    // {0x3002, 0x0000}, {0x3004, 0xDDD0}, {0x3006, 0x0086}, {0xFFFF, 0x0001}

    {0x0004, 0x001F}, {0x0006, 0x0005}, {0x000A, 0x00C8}, {0x000E, 0x000D}, {0x0100, 0x0100}, {0x0102, 0x0302},
    {0x0104, 0x0504}, {0x010A, 0x274C}, {0x010C, 0x0401}, {0x010E, 0x0000}, {0x0110, 0x0F14}, {0x0112, 0x4455},
    {0x0114, 0x000A}, {0x011E, 0x0219}, {0x0120, 0x0219}, {0x0122, 0x0C99}, {0x0126, 0x274C}, {0x0128, 0x0401},
    {0x012A, 0x0000}, {0x012C, 0x0D16}, {0x012E, 0x4422}, {0x0130, 0x000A}, {0x013A, 0x031F}, {0x013C, 0x0400},
    {0x013E, 0x0C99}, {0x0142, 0x274C}, {0x0144, 0x0401}, {0x0146, 0x0000}, {0x0148, 0x0D16}, {0x014A, 0x4422},
    {0x014C, 0x000A}, {0x0156, 0x0019}, {0x0158, 0x0400}, {0x015A, 0x0C99}, {0x015E, 0x274C}, {0x0160, 0x0401},
    {0x0162, 0x0000}, {0x0164, 0x0C10}, {0x0166, 0x4422}, {0x0168, 0x000A}, {0x0172, 0x000C}, {0x0174, 0x0400},
    {0x017A, 0x274C}, {0x017C, 0x0401}, {0x017E, 0x0000}, {0x0180, 0x0000}, {0x0182, 0x4422}, {0x0184, 0x000A},
    {0x0190, 0x0400}, {0x0196, 0xC74C}, {0x0198, 0x0401}, {0x019A, 0x0000}, {0x019C, 0x0C10}, {0x019E, 0x4422},
    {0x01A0, 0x000A}, {0x01A6, 0x0003}, {0x01AA, 0x000C}, {0x01AC, 0x0400}, {0x01EC, 0x0122}, {0x01EE, 0x02FA},
    {0x01F0, 0x02FA}, {0x01F2, 0x0098}, {0x01F4, 0x001A}, {0x01F6, 0x0098}, {0x0200, 0x0120}, {0x0280, 0x0A00},
    {0x0282, 0xF530}, {0x0284, 0x9388}, {0x0286, 0xC45C}, {0x0288, 0xA328}, {0x0408, 0x0001}, {0x0410, 0xDDD0},
    {0x0412, 0x0086}, {0x0414, 0xD090}, {0x0416, 0x0083}, {0x0422, 0x0011}, {0x0424, 0x0901}, {0x0426, 0x0102},
    {0x0428, 0x0102}, {0x0502, 0x4F60}, {0x0504, 0x000A}, {0x0588, 0x0008}, {0x0692, 0x0233}, {0x0694, 0x0400},
    {0x0698, 0x2310}, {0x0700, 0x0000}, {0x1000, 0x0000}, {0x1002, 0x3197}, {0x1004, 0x0000}, {0x1006, 0xBA08},
    {0x1008, 0x2CC2}, {0x100A, 0x0001}, {0x10E0, 0x0002}, {0x10E2, 0x0000}, {0x10E4, 0x0000}, {0x10E6, 0xFFFF},
    {0x10E8, 0x0202}, {0x10EA, 0x0202}, {0x10EC, 0x0202}, {0x10EE, 0x0202}, {0x1120, 0x00C8}, {0x1122, 0x0005},
    {0x1124, 0x0000}, {0x1140, 0x0000}, {0x1142, 0x00C8}, {0x1144, 0x0001}, {0x1160, 0x0007}, {0x1162, 0x0000},
    {0x1164, 0x0402}, {0x1166, 0x4568}, {0x1168, 0x9940}, {0x116A, 0x00D9}, {0x116C, 0x8800}, {0x116E, 0x0093},
    {0x1170, 0x5C00}, {0x1172, 0x00C4}, {0x1174, 0x2800}, {0x1176, 0x00A3}, {0x2000, 0x0002}, {0x2002, 0xAAA2},
    {0x2022, 0x0002}, {0x2024, 0x0901}, {0x2044, 0x0002}, {0x2046, 0x0901}, {0x20CC, 0x0004}, {0x20CE, 0x2B23},
    {0x20D0, 0x4A42}, {0x2132, 0x0006}, {0x2134, 0x0901}, {0x2136, 0x6A62}, {0x2138, 0x8D85}, {0x2880, 0x0005},
    {0x2882, 0x0019}, {0x2884, 0x0019}, {0x288C, 0x0019}, {0x2892, 0x0019}, {0x3000, 0x0401}, {0x3002, 0x0000},
    {0x3004, 0xDDD0}, {0x3006, 0x0086}, {0xFFFF, 0x0001}
#else
    {0, 0}
#endif
};

#if (__GH3X2X_CFG_LIST_MAX_NUM__ > 1)

const STGh3x2xReg gh3x2x_reg_list1[] =
{
// ECG
#if EVK_USE
    {0x0004, 0x0013}, {0x0006, 0x0001}, {0x000A, 0x005F}, {0x000E, 0x000D}, {0x0074, 0x0050}, {0x0100, 0x0100},
    {0x010A, 0x0162}, {0x010C, 0x0000}, {0x010E, 0x0000}, {0x0110, 0x0000}, {0x011E, 0x0400}, {0x0120, 0x0400},
    {0x0126, 0xC704}, {0x0128, 0x0002}, {0x012A, 0x0000}, {0x012C, 0x0C10}, {0x012E, 0x4442}, {0x0130, 0x0002},
    {0x0136, 0x0003}, {0x013A, 0x010C}, {0x013C, 0x0400}, {0x01EA, 0x0033}, {0x01EC, 0x0030}, {0x01EE, 0x0098},
    {0x0200, 0x1020}, {0x0402, 0x5555}, {0x0404, 0x4120}, {0x0406, 0x0041}, {0x0408, 0x0001}, {0x0410, 0xDDD0},
    {0x0412, 0x0086}, {0x0414, 0xD090}, {0x0416, 0x0083}, {0x0422, 0x0011}, {0x0424, 0x0901}, {0x0426, 0x0109},
    {0x0428, 0x0108}, {0x0480, 0x6550}, {0x0502, 0x4C66}, {0x0588, 0x0008}, {0x0692, 0x0233}, {0x0694, 0x0404},
    {0x0698, 0x3310}, {0x069A, 0x1101}, {0x0700, 0x0000}, {0x0400, 0x5557}, {0x1000, 0x0000}, {0x1002, 0x3197},
    {0x1004, 0x0000}, {0x1006, 0xC215}, {0x1008, 0x2B1C}, {0x100A, 0x0001}, {0x10E0, 0x00FF}, {0x10E2, 0xFFFF},
    {0x10E4, 0xFFFF}, {0x10E6, 0xFFFF}, {0x10E8, 0x0202}, {0x10EA, 0x0202}, {0x10EC, 0x0202}, {0x10EE, 0x0202},
    {0x1120, 0x005F}, {0x1122, 0x0005}, {0x1124, 0x0000}, {0x2000, 0x0001}, {0x2002, 0x0022}, {0x20EE, 0x0001},
    {0x20F0, 0x001C}, {0x2880, 0x0005}, {0x288E, 0x01F4}, {0x3000, 0x0101}, {0x3002, 0x0000}, {0x3004, 0xDDD0},
    {0x3006, 0x0086}, {0x4500, 0x46C9}, {0x4502, 0x0F0F}, {0x4504, 0x0002}, {0x4506, 0x0320}, {0xFFFF, 0x0001}
#elif
    {0,0}
#endif
};
#endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 1) */

#if (__GH3X2X_CFG_LIST_MAX_NUM__ > 2)
const STGh3x2xReg gh3x2x_reg_list2[] =
{
    {0,0},

};
#endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 2) */

#if (__GH3X2X_CFG_LIST_MAX_NUM__ > 3)
const STGh3x2xReg gh3x2x_reg_list3[] =
{
    {0,0},
};
#endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 3) */

#if (__GH3X2X_CFG_LIST_MAX_NUM__ > 4)
const STGh3x2xReg gh3x2x_reg_list4[] =
{
    {0,0},
};
#endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 4) */

#if (__GH3X2X_CFG_LIST_MAX_NUM__ > 5)
const STGh3x2xReg gh3x2x_reg_list5[] =
{
    {0,0},
};
#endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 5) */

#if (__GH3X2X_CFG_LIST_MAX_NUM__ > 6)
const STGh3x2xReg gh3x2x_reg_list6[] =
{
    {0,0},
};
#endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 6) */

#if (__GH3X2X_CFG_LIST_MAX_NUM__ > 7)
const STGh3x2xReg gh3x2x_reg_list7[] =
{
    {0,0},
};
#endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 7) */

const STGh3x2xInitConfig g_stGh3x2xCfgListArr[__GH3X2X_CFG_LIST_MAX_NUM__] = 
{
    {gh3x2x_reg_list0,sizeof(gh3x2x_reg_list0) / sizeof(gh3x2x_reg_list0[0])},
    #if (__GH3X2X_CFG_LIST_MAX_NUM__ > 1)
    {gh3x2x_reg_list1,sizeof(gh3x2x_reg_list1) / sizeof(gh3x2x_reg_list1[0])},
    #endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 1) */
    #if (__GH3X2X_CFG_LIST_MAX_NUM__ > 2)
    {gh3x2x_reg_list2,sizeof(gh3x2x_reg_list2) / sizeof(gh3x2x_reg_list2[0])},
    #endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 2) */
    #if (__GH3X2X_CFG_LIST_MAX_NUM__ > 3)
    {gh3x2x_reg_list3,sizeof(gh3x2x_reg_list3) / sizeof(gh3x2x_reg_list3[0])},
    #endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 3) */
    #if (__GH3X2X_CFG_LIST_MAX_NUM__ > 4)
    {gh3x2x_reg_list4,sizeof(gh3x2x_reg_list4) / sizeof(gh3x2x_reg_list4[0])},
    #endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 4) */
    #if (__GH3X2X_CFG_LIST_MAX_NUM__ > 5)
    {gh3x2x_reg_list5,sizeof(gh3x2x_reg_list5) / sizeof(gh3x2x_reg_list5[0])},
    #endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 5) */
    #if (__GH3X2X_CFG_LIST_MAX_NUM__ > 6)
    {gh3x2x_reg_list6,sizeof(gh3x2x_reg_list6) / sizeof(gh3x2x_reg_list6[0])},
    #endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 6) */
    #if (__GH3X2X_CFG_LIST_MAX_NUM__ > 7)
    {gh3x2x_reg_list7,sizeof(gh3x2x_reg_list7) / sizeof(gh3x2x_reg_list7[0])},
    #endif /* (__GH3X2X_CFG_LIST_MAX_NUM__ > 7) */
};


#if (__GH3X2X_CASCADE_EN__)
const STGh3x2xReg stGh3x2xEcgCascadeSlaveRealReglist[] = 
{

/***Ecg Cascade Slaver: No lead detected****/
#if 0
    {0x0000, 0x06B0}, {0x0004, 0x001F}, {0x000A, 0x0064}, {0x000E, 0x000D}, {0x0100, 0x0800}, {0x010A, 0x01E2},
    {0x010E, 0x0004}, {0x01EA, 0x0033}, {0x01EC, 0x0030}, {0x0200, 0x1020}, {0x0208, 0x0001}, {0x0480, 0x6550},
    {0x0502, 0x4060}, {0x0588, 0x0008}, {0x0692, 0x0233}, {0x0694, 0x0404}, {0x0698, 0x3310}, {0x069A, 0x1101},
    {0x0700, 0x0000},
#endif
    
/***Ecg Cascade Slaver: lead detected****/
#if 1
    //internal 32K :{0x0000, 0x0630},  {0x0074, 0x0050},
    //external 32K :{0x0000, 0x06b0},  {0x0074, 0x00D0},
    #if __GH3X2X_ECG_RTYPE_5M__
    {0x0000, 0x0630}, {0x0004, 0x0013}, {0x000A, 0x005F}, {0x000E, 0x000D}, {0x0074, 0x0050}, {0x0100, 0x0100},
    {0x010A, 0x0162}, {0x010C, 0x0000}, {0x010E, 0x0000}, {0x0110, 0x0000}, {0x0120, 0x0400}, {0x0126, 0xC73C},
    {0x012C, 0x0C10}, {0x012E, 0x2222}, {0x0130, 0x00AA}, {0x0136, 0x00FF}, {0x013C, 0x0400}, {0x01EA, 0x0033},
    {0x01EC, 0x0030}, {0x01EE, 0x0098}, {0x0200, 0x1020}, {0x0402, 0x5555}, {0x0404, 0x4130}, {0x0406, 0x0041},
    {0x0408, 0x0001}, {0x0426, 0x0101}, {0x0428, 0x0101}, {0x0480, 0x6550}, {0x0502, 0x4C66}, {0x0588, 0x0008},
    {0x0692, 0x0233}, {0x0694, 0x0404}, {0x0698, 0x3310}, {0x069A, 0x1101}, {0x0700, 0x0000}, {0x0400, 0x5554},
    {0x1000, 0x0000}, {0x1002, 0x3190}, {0x1004, 0x0000}, {0x1006, 0x72E8}, {0x1008, 0x292F}, {0x100A, 0x0001},
    {0x10E0, 0x0000}, {0x10E2, 0xFFFF}, {0x10E4, 0xFFFF}, {0x10E6, 0xFFFF}, {0x10E8, 0x0202}, {0x10EA, 0x0202},
    {0x10EC, 0x0202}, {0x10EE, 0x0202}, {0x1120, 0x005F}, {0x1122, 0x0005}, {0x1124, 0x0000}, {0x2000, 0x0004},
    {0x2002, 0x2D25}, {0x2004, 0x3D35}, {0x20EE, 0x0001}, {0x20F0, 0x001C}, {0x2880, 0x0005}, {0x288E, 0x01F4},
    {0x3000, 0x0101}, {0x3002, 0x0000}, {0x3004, 0x0000}, {0x3006, 0x0000}, {0x4500, 0x46C9}, {0x4502, 0x0F0F},
    {0x4504, 0x000A}, {0x4506, 0x0320}, {0x45C0, 0x0000}, {0x45C2, 0x0000}, {0x45C4, 0x0000}, {0x45C6, 0x0000},
    {0xFFFF, 0x0001}, 
    #else
    {0x0000, 0x0630}, {0x0004, 0x0013}, {0x000A, 0x005F}, {0x000E, 0x000D}, {0x0074, 0x0050}, {0x0100, 0x0100},
    {0x010A, 0x0162}, {0x010C, 0x0000}, {0x010E, 0x0000}, {0x0110, 0x0000}, {0x0120, 0x0400}, {0x0126, 0xC73C},
    {0x012C, 0x0C10}, {0x012E, 0x2222}, {0x0130, 0x00AA}, {0x0136, 0x00FF}, {0x013C, 0x0400}, {0x01EA, 0x0033},
    {0x01EC, 0x0030}, {0x01EE, 0x0098}, {0x0200, 0x1020}, {0x0402, 0x5555}, {0x0404, 0x4120}, {0x0406, 0x0041},
    {0x0408, 0x0001}, {0x0426, 0x0101}, {0x0428, 0x0101}, {0x0480, 0x6550}, {0x0502, 0x4C66}, {0x0588, 0x0008},
    {0x0692, 0x0233}, {0x0694, 0x0404}, {0x0698, 0x3310}, {0x069A, 0x1101}, {0x0700, 0x0000}, {0x0400, 0x5554},
    {0x1000, 0x0000}, {0x1002, 0x3190}, {0x1004, 0x0000}, {0x1006, 0x2184}, {0x1008, 0x293A}, {0x100A, 0x0001},
    {0x10E0, 0x0000}, {0x10E2, 0xFFFF}, {0x10E4, 0xFFFF}, {0x10E6, 0xFFFF}, {0x10E8, 0x0202}, {0x10EA, 0x0202},
    {0x10EC, 0x0202}, {0x10EE, 0x0202}, {0x1120, 0x005F}, {0x1122, 0x0005}, {0x1124, 0x0000}, {0x2000, 0x0004},
    {0x2002, 0x2D25}, {0x2004, 0x3D35}, {0x20EE, 0x0001}, {0x20F0, 0x001C}, {0x2880, 0x0005}, {0x288E, 0x01F4},
    {0x3000, 0x0101}, {0x3002, 0x0000}, {0x3004, 0x0000}, {0x3006, 0x0000}, {0x4500, 0x46C9}, {0x4502, 0x0F0F},
    {0x4504, 0x000A}, {0x4506, 0x0320}, {0x45C0, 0x0000}, {0x45C2, 0x0000}, {0x45C4, 0x0000}, {0x45C6, 0x0000},
    {0xFFFF, 0x0001}, 
    #endif
#endif
    

};
const GU16 g_usEcgCascadeSlaveRealRegLen = sizeof(stGh3x2xEcgCascadeSlaveRealReglist) / sizeof(STGh3x2xReg);

#else
const GU16 g_usEcgCascadeSlaveRealRegLen = 1;
const STGh3x2xReg stGh3x2xEcgCascadeSlaveRealReglist[1];
#endif
