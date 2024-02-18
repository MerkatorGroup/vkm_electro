/****************************************************************************
 * Project:   name
 * @file      project.c
 * @author    author
 * @date      [Creation date in format %02d.%02d.20%02d]
 * @brief     description
 * @version   0.1
 * @copyright copyright (c) [Actual Year] by your company
 *
 ****************************************************************************/
#include "main_px_bat.h"

tGsThread id_MAIN_Thread;

void SwitchIgnition(u8 En);
void *Thread_MAIN(void *arg);
//**************************************
int Ini_Thread_MAIN(void) {
    int th = gsThreadCreate(&id_MAIN_Thread, NULL, Thread_MAIN, NULL);
    if (th != 0) {
        // db_out_time("ini Thread_AKB  - FAIL!");
        return (-1);
    }
    return 0;
}

static u8 bat_JnOff = 0;
void *Thread_MAIN(void *arg) {
    bat_JnOff = 0;
    SwitchIgnition(0);

    SetDOut(0, 0);

    while (1) {
            u8 nowInv_ERR = (GetVar(HDL_INVERTER_ERROR)  & 2) !=0;
            u8 nowInv_Warn =( GetVar(HDL_INVERTER_WARN)  & 2) !=0 ;
            u8 fBatLowL = GetVar(HDL_BAT_LOW_LEVEL);

            u8 fInvReady = 0;
            u8 trdyErr = 3;
            if ((nowInv_ERR || nowInv_Warn || fBatLowL) == 0) {
                fInvReady = 1;trdyErr = 1;
                vechleicons_Set_stat(READY_INV, 1);
            } else if(nowInv_Warn)trdyErr = 2;
                vechleicons_Set_stat(READY_INV, trdyErr);

        if (GetVar(HDL_IGNITION_IS_ON)) {
            vechleicons_Set_stat(KEY_IGN, 1);
            SetVar(HDL_INV_READY, fInvReady);
            if (fInvReady) SwitchIgnition(1);
        }

        if (bat_JnOff != 0) {
            u8 f_LowLvlInv = 0;
            if(GetVar(HDL_INVERTER_VOLTAGE) < INV_LOW_LEVEL)
                f_LowLvlInv |= 2;
            if(GetVar(HDL_INVERTER_VOLTAGE) < INV_LOW_LEVELWARN)
                f_LowLvlInv |= 1;
            SetVar(HDL_INV_LOW_LEVEL, f_LowLvlInv != 0);
            vechleicons_Set_stat(LOW_LVL_INV, f_LowLvlInv );
            if (GetVar(HDL_IGNITION_IS_ON) == 0) {
                vechleicons_Set_stat(KEY_IGN, 0);
                SwitchIgnition(0);
            }
        }
        VertBars_Run();
        vechleicons_show();
        usleep(10000);  /// 10 ms
    }
    return NULL;
}

void SendRequest_BAT_ON_OFF(u8 OnOff) {
    tCanMessage tx_msg;
    tx_msg.channel = ShassieCAN_ch;
    tx_msg.ext = 1;
    tx_msg.id = 0x0CFF3521;
    tx_msg.len = 8;
    tx_msg.res = 0;
    memset(&tx_msg.data, 0xFF, 8);
    if (OnOff == 0)
        tx_msg.data.u8[1] &= ((u8)(0xCF));
    else
        tx_msg.data.u8[1] &= ((u8)(0xDF));
    CANSendMsg(&tx_msg);
}

void RelayIgnitionOn(u8 OnOff) { SetDOut(0, OnOff); }

void SwitchIgnition(u8 En) {
    if ((En) && (bat_JnOff == 0)) {
        SendRequest_BAT_ON_OFF(1);
        bat_JnOff = 1;
    } else if ((!En) && (bat_JnOff != 0)) {
        SendRequest_BAT_ON_OFF(0);
        bat_JnOff = 0;
    }
}
