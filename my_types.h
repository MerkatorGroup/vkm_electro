/************************************************************************
*
* File:         my_types.h
* Project:
* Author(s):
* Date:
*
* Description:
*
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef MY_TYPES_H
#define MY_TYPES_H

#include	<stdint.h>
/*************************************************************************/

/* macro definitions *****************************************************/
#ifndef u64t
typedef   uint64_t u64;
#endif

#ifndef u32
typedef   uint32_t u32;
#endif

#ifndef u16
typedef   uint16_t u16;
#endif

#ifndef u8
typedef   uint8_t u8;
#endif

#ifndef s64
typedef   int64_t s64;
#endif
#ifndef s32
typedef   int32_t s32;
#endif

#ifndef s16
typedef   int16_t s16;
#endif
#ifndef s8
typedef   int8_t s8;
#endif

#define NOT_VALID ((float)((u32) 0))
/*************************************************************************/

#endif  // #ifndef MY_TYPES_H
