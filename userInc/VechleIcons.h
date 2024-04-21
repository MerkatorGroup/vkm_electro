/*
 * VechleIcons.h
 *
 *  Created on: 25 мая 2023 г.
 *      Author: ASUS
 */

#ifndef WGHTSPREAD_SOURCE_VISU_VECHLEICONS_H_
#define WGHTSPREAD_SOURCE_VISU_VECHLEICONS_H_

#include "main_px_bat.h"
void vechleicons_show(void);
int CreateVechleIconsThread(void);
/////////////////
#define ICO_NUM_10 369  // key ignition
#define ICO_NUM_11 41   // Low Bat
#define ICO_NUM_12 45   // Temperature Inv
#define ICO_NUM_13 47   // Inv Ready- ERRor
#define ICO_NUM_14 249  // Inv HYD Ready- ERRor
#define ICO_NUM_15 0
#define ICO_NUM_16 1
#define ICO_NUM_17 2
#define ICO_NUM_18 9
#define ICO_NUM_19 44
#define ICO_NUM_20 366
#define ICO_NUM_21 325
#define ICO_NUM_22 209
#define ICO_NUM_23 82
#define ICO_NUM_24 6

typedef enum {
    KEY_IGN = 0,
    LOW_VOLT_BAT,
    TERM_INV,
    READY_INV,
    READY_HYD_INV,
    BOX_IS_UP=8,
    BRAKE_PRESSURE = 9,
    OIL_TERM = 10,
    BRAKE_PEDAL = 11,
    LOW_LVL_CRG = 13
} eIco;

typedef enum {
NOTAVLBL = 0,
STAT_OK,
WARN,
FAIL
}eStat;

typedef struct _VechleIcon_map {
    int num;      // num on gray spline index bmap
    int HDL_ico;  // hdl icon on panel
} VechleIcon_map;
extern void vechleicons_show(void);
extern void vechleicons_Set_stat(u8 numIcon, u8 val);
void vechleicons_Reset(void) ;
#endif /* WGHTSPREAD_SOURCE_VISU_VECHLEICONS_H_ */
