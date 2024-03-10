
/*
 * VechleIcons.c
 *
 *  Created on: 25 мая 2023 г.
 *      Author: ASUS
 */

#include "main_px_bat.h"
//**************************************

static const VechleIcon_map IcoNums [15] = {
    {ICO_NUM_10, HDL_ICOINDX_10 }, // System start after starter key
	{ICO_NUM_11, HDL_ICOINDX_11},  // Low Bat
    {ICO_NUM_12, HDL_ICOINDX_12},  // Temperature Inv
	{ICO_NUM_13, HDL_ICOINDX_13},  // Inv Ready- ERRor
    {ICO_NUM_14, HDL_ICOINDX_14},
	{ICO_NUM_15, HDL_ICOINDX_15},
    {ICO_NUM_16, HDL_ICOINDX_16},
	{ICO_NUM_17, HDL_ICOINDX_17},
    {ICO_NUM_18, HDL_ICOINDX_18},
	{ICO_NUM_19, HDL_ICOINDX_19},
    {ICO_NUM_20, HDL_ICOINDX_20},
	{ICO_NUM_21, HDL_ICOINDX_21},
    {ICO_NUM_22, HDL_ICOINDX_22},
	{ICO_NUM_23, HDL_ICOINDX_23},
    {ICO_NUM_24, HDL_ICOINDX_24}}; // Low Level voltage on Inv

static u8 stat[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
void* Thread_VechleIcoPanel(void* arg);
// tGsThread VechleIcons_Thread;
// int CreateVechleIconsThread(void) {
//     int th =
//         gsThreadCreate(&VechleIcons_Thread, NULL, Thread_VechleIcoPanel, NULL);
//     if (th != 0) {
//         db_out_time("Thread Vechle IcoPanel  -  FAIL!");
//         return (-1);
//     }
//     return (0);
// }

// void* Thread_VechleIcoPanel(void* arg) {
//     u8 indx = 0;
//     while (1) {
// 		vechleicons_show();
//         usleep(300000);  ///< поток вызывается 1 /300 ms
//     };
//     return NULL;
// }

void vechleicons_show(void) {
    u8 indx = 0;
    // u8 tSpn_bVal = (u8)GetfValueSpn(70);
    // stat[4] = (tSpn_bVal == 1) ? 3 : 0;

    while (indx < 15) {
        u8* pStat = &stat[indx];
        if ((*pStat) > 3) (*pStat) = 0;
        u16 n = IcoNums[indx].num;
        u16 row = n / 40;
        u16 col = n % 10;
        u16 Val = row * 40 + 10 * (*pStat) + col;

        SetVar(IcoNums[indx].HDL_ico, Val);
        //			if (++indx > 14)
        //				indx = 0;
        indx++;
    }
}

void vechleicons_Set_stat(u8 numIcon, u8 val) {
	stat[numIcon]= val;
}
void vechleicons_Reset() {
    for (size_t i = 0; i < 15; i++) {
       stat[i]= 0; /* code */
    }
}

/////////////////
