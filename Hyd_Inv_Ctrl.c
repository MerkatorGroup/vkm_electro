/****************************************************************************
 *
 * Project:   name
 *
 * @file      project.c
 * @author    author
 * @date      [Creation date in format %02d.%02d.20%02d]
 *
 * @brief     description
 *
 * @version   0.1
 *
 * @copyright copyright (c) [Actual Year] by your company
 *
 ****************************************************************************/
#include "main_px_bat.h"

static tCanMessage buff[16];
static tGsThread id_HydrInvThread;
static void *Thread_HydrInv(void *arg);
static s32 FifoRX_INVERTOR_HYDR;
static int parseInvert(tCanMessage *msg);
// static void resetAll(void);
static void sendCmd_HydrInv(u8 En);
t_Inv_Profile Inv_Hydr;
static void sendCmd_Hydr_Torque(u16 Promile);

//**************************************
int Ini_HydrInv_Thread(void) {
    FifoRX_INVERTOR_HYDR = CANCreateFiFo(64);
    int rs = CANAddRxFilterFiFo(ShassieCAN_ch, 0x082, 0x0FF, 0,
                                FifoRX_INVERTOR_HYDR);
    int th = gsThreadCreate(&id_HydrInvThread, NULL, Thread_HydrInv, NULL);
    if (th != 0) {
        // db_out_time("ini Thread_EngineInv  - FAIL!");
        return (-1);
    }
    return 0;
}

static u8 resetErr = 1;
static void *Thread_HydrInv(void *arg) {
    // resetAll();
    u32 timer_100ms = 2;
    u32 timer_500ms = 10;
    u8 EnInv = 1;

    u32 tmpUSleep = 10000;
    while (1) {
        //~~~~~~~~~~~~~~~~~~~~
        tCanMessage *tMsg = (tCanMessage *)&buff[2];
        u32 n = CANReadFiFo(FifoRX_INVERTOR_HYDR, tMsg, 1);

        if (n){
            parseInvert(tMsg);
            Inv_Hydr.Inv_OnLine =1;
        }
static u8 t = 100;
        u32 nowInv_OverTerm = ((GetVar(HDL_INVERTER_OVER_TEMP_HYD) & 2) == 2);
        vechleicons_Set_stat(TERM_INV, (nowInv_OverTerm * 3));
        if (Inv_Hydr.OnOff == START) {
            EnInv = 1;
            Inv_Hydr.OnOff = ON_START;
            t =100;
        } else if (Inv_Hydr.OnOff == ON_START) {
            if (GetVar(HDL_INVERTER_ON_HYD)) {
                Inv_Hydr.TorqPercent = 250;
                if(t)t --;
                else{
                    Inv_Hydr.OnOff = ON_WORK;
                }
                // if (GetVar(HDL_INVERTER_TORQUE_HYD) > 24) {

                // }
            }
        } else if (Inv_Hydr.OnOff == STOP) {
                Inv_Hydr.TorqPercent =0;
            if (GetVar(HDL_INVERTER_TORQUE_HYD) <= 0) {
                Inv_Hydr.OnOff = WAIT_STOP;
                Inv_Hydr.TorqPercent = 0;
                EnInv = 0;
            }
        } else if (Inv_Hydr.OnOff == WAIT_STOP) {
            if (GetVar(HDL_INVERTER_ON_HYD) == 0) Inv_Hydr.OnOff = IDLE;
            // tmpUSleep = 10000;
        }
        static u8 queSndMsg = 0;
        if (timer_100ms)
            timer_100ms--;
        else {
            if (queSndMsg == 0)
                sendCmd_HydrInv(EnInv);
            else if (queSndMsg == 1)
                sendCmd_Hydr_Torque(Inv_Hydr.TorqPercent);
            if (++queSndMsg > 1) queSndMsg = 0;
            timer_100ms = 5;
        }

        if (timer_500ms)
            timer_500ms--;
        else {
            if (resetErr) EnInv = 0;
            resetErr = 0;
            timer_500ms = 50;
        }

        usleep(10000);  /// 10 mstmpUSleep
    }
    return NULL;
}

static void sendCmd_HydrInv(u8 En) {
    tCanMessage tx_msg;
    tx_msg.channel = ShassieCAN_ch;
    tx_msg.ext = 0;
    tx_msg.id = 0x195;
    tx_msg.len = 8;
    tx_msg.res = 0;
    tx_msg.data.u32[0] = En & 0x1;
    tx_msg.data.u16[2] =
        ((Inv_Hydr.EmergensyStop & 0x1) | ((Inv_Hydr.Retarder << 2) & 0x4));
    s16 powerAKB = GetVar(HDL_VOLTAGE_LEVEL);
    tx_msg.data.u16[3] = mabs(powerAKB);
    CANSendMsg(&tx_msg);
}
static void sendCmd_Hydr_Torque(u16 Promile) {
    static u16 prevPromile =0;
    s8 step =0;
    if(Promile > prevPromile)step = 1;
    else if(Promile < prevPromile)step = -1;
    prevPromile += step;
    prevPromile = constrain(prevPromile,0,1000);
    tCanMessage tx_msg;
    tx_msg.channel = ShassieCAN_ch;
    tx_msg.ext = 0;
    tx_msg.id = 0x295;
    tx_msg.len = 8;
    tx_msg.res = 0;
    tx_msg.data.u16[0] = 0;
    tx_msg.data.u16[1] = (u16)(32.767f *  prevPromile);  // 0x7332;
    tx_msg.data.u32[1] = 0;
    CANSendMsg(&tx_msg);
}


void make_hyd_torq_vs_rpm(u32 reqRpm){
    u32 now_rpm = GetVar(HDL_INVERTER_RPM_HYD);
    s32 step = 0;
    if(reqRpm > now_rpm)step = -1;
    else if(reqRpm < now_rpm)step = 1;
    Inv_Hydr.TorqPercent += step;
}


void Inv_Hydr_resetAll(void) {
    SetVar(HDL_INVERTER_ON_HYD, 0);
    SetVar(HDL_INVERTER_ERROR_HYD, 0);
    SetVar(HDL_INVERTER_WARN_HYD, 0);
    SetVar(HDL_INVERTER_OVER_TEMP_HYD, 0);

    approx(HDL_INVERTER_TORQUE_HYD, 0);
    approx(HDL_INVERTER_RPM_HYD, 0);
    approx(HDL_INVERTER_CURRENT_HYD, 0);
    approx(HDL_INVERTER_POWER_HYD, 0);
    approx(HDL_INVERTER_VOLTAGE_HYD, 0);

    vechleicons_Set_stat(TERM_INV, 0);
    Inv_Hydr.OnOff = IDLE;
    Inv_Hydr.Retarder = 0;
    Inv_Hydr.EmergensyStop = 0;
    Inv_Hydr.TorqPercent = 0;
    resetErr = 1;
    Inv_Hydr.Inv_OnLine =0;
}

static int parseInvert(tCanMessage *msg) {
    u16 Reg = msg->id;
    if (Reg == 0x182) {
        u8 f_er = msg->data.u8[0] & 1;
        SetVar(HDL_INVERTER_ON_HYD, f_er);
        f_er = (msg->data.u8[0] & 2) >> 1;
        SetVar(HDL_INVERTER_ERROR_HYD, f_er);
        SetVar(HDL_INV_LED_E_HYD,  f_er+1);

        f_er = (msg->data.u8[0] & 4) >> 2;
        SetVar(HDL_INVERTER_WARN_HYD, f_er);
        SetVar(HDL_INV_LED_W_HYD,  f_er+1);
        f_er = (msg->data.u8[0] & 8) >> 3;
        SetVar(HDL_INVERTER_OVER_TEMP_HYD, f_er);
        SetVar(HDL_INV_LED_T_HYD,  f_er+1);

        s32 t = (s8)msg->data.s8[1];
        t = (t * 100) / 127;
        // approx(HDL_INVERTER_TORQUE_HYD, t);
        SetVar(HDL_INVERTER_TORQUE_HYD, t);

        t = (s16)(msg->data.u8[2] | (msg->data.u8[3] << 8));
        t = t * 8000 / 32767;
        // approx(HDL_INVERTER_RPM_HYD, t);
        SetVar(HDL_INVERTER_RPM_HYD, mabs(t));
        t = msg->data.s8[4];
        t = (t * 650) / 127;
        // approx(HDL_INVERTER_CURRENT_HYD, t);
        SetVar(HDL_INVERTER_CURRENT_HYD, t);
        t = msg->data.s8[5];
        // t += 150;
        t = (t * 150) / 127;
        // approx(HDL_INVERTER_POWER_HYD, t);
        SetVar(HDL_INVERTER_POWER_HYD, t);
        u32 v = msg->data.u8[6];
        v = (v * 1620) / 255;
        // approx(HDL_INVERTER_VOLTAGE_HYD, v);
        SetVar(HDL_INVERTER_VOLTAGE_HYD, v);
    }

    return 0;
}
