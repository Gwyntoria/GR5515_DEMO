#ifndef __GOODIX_NADT_H
#define __GOODIX_NADT_H

#include <stdint.h>
#ifndef _ARM_MATH_H
#include "goodix_type.h"
#endif
#include "goodix_algo.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void NADTProcInit(void);
extern void NADTControl (GS32 lOptTye, GS32 lConfigValue[]);
extern GU8 NADTProc(GU32 lPacketInfoArr[], GS32 lResult[]);
extern void NADTProcDeinit(void);
extern GU8 NADTWearDetectByCap(GS32 lPacketInfoArr[]);
extern GU8* GetNadtVersion(void);

#ifdef __cplusplus
}
#endif

#endif
