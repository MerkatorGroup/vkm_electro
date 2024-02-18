/*
 * VechleIcons.h
 *
 *  Created on: 25 мая 2023 г.
 *      Author: ASUS
 */

#ifndef WGHTSPREAD_SOURCE_VISU_VECHLEICONS_H_
#define WGHTSPREAD_SOURCE_VISU_VECHLEICONS_H_

#include "../main_px_bat.h"
void vechleicons_show(void);
int CreateVechleIconsThread(void);
/////////////////
#define ICO_NUM_10 369  // key ignition
#define ICO_NUM_11 41   // Low Bat
#define ICO_NUM_12 45   // Temperature Inv
#define ICO_NUM_13 47   // Inv Ready- ERRor
#define ICO_NUM_14 6
#define ICO_NUM_15 0
#define ICO_NUM_16 1
#define ICO_NUM_17 2
#define ICO_NUM_18 80
#define ICO_NUM_19 240
#define ICO_NUM_20 366
#define ICO_NUM_21 246
#define ICO_NUM_22 209
#define ICO_NUM_23 246
#define ICO_NUM_24 249  // Low Level voltage on Inv

typedef enum {
    KEY_IGN = 0,
    LOW_BAT,
    TERM_INV,
    READY_INV,
    LOW_LVL_INV = 14
} eIco;

typedef struct _VechleIcon_map {
    int num;      // num on gray spline index bmap
    int HDL_ico;  // hdl icon on panel
} VechleIcon_map;
extern void vechleicons_show(void);
extern void vechleicons_Set_stat(u8 numIcon, u8 val);
#endif /* WGHTSPREAD_SOURCE_VISU_VECHLEICONS_H_ */
