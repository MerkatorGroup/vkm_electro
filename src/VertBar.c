
/*
 * VechleIcons.c
 *
 *  Created on: 25 РјР°СЏ 2023 Рі.
 *      Author: ASUS
 */

#include "main_px_bat.h"

//**************************************
#define NUM_BARS 4

DashObj vBAr[NUM_BARS] = {
    {HDL_INVERTER_CURRENT, 0, 100, HDL_INV_CURRENT_BAR},
    {HDL_CURRENT, 0, 100, HDL_BAT_CURRENT_BAR},

    {HDL_INVERTER_CURRENT_HYD, 0, 100,HDL_INV_CURRENT_BAR_HYD },
    {HDL_DISCH_PWR, 0, 800, HDL_BAT_DISCHARG_BAR},
};

// const VertBar INV_Curr;
// const VertBar Bat_Curr;
// const VertBar INV_Volt;
// const VertBar Bat_Volt;
// void* res = malloc(sizeof(VertBar));

static void VertBars_Run(void) {
    for (u8 i = 0; i < NUM_BARS; i++) {
        DashObj* curr = &vBAr[i];
        int v = GetVar(curr->HDL_RefValue);
        v = mabs(v);
        u8 k = (curr->Max - curr->Min) / 11;
        int indx = (int)(v / k);
        SetVar(curr->HDL_Bar_indx_val, indx);
    }
}

DashObj vArowObj[] = {
{HDL_INVERTER_RPM, 400, 2400, HDL_RPM_ARROW},
};
void DashBoardCycle(void) {
    VertBars_Run( );





}
