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
static tGsThread id_EngInvThread;
static void *Thread_EngineInv(void *arg);
static s32 FifoRX_INVERTOR;
static int parseInvert(tCanMessage *msg);

static void sendCmd_EngInv(u8 En);
t_Inv_Profile Inv_Engine;
static u8 resetErr = 1;

//**************************************
int Ini_EngInv_Thread(void) {
    FifoRX_INVERTOR = CANCreateFiFo(64);
    int rs =
        CANAddRxFilterFiFo(ShassieCAN_ch, 0x081, 0x0FF, 0, FifoRX_INVERTOR);

    int th = gsThreadCreate(&id_EngInvThread, NULL, Thread_EngineInv, NULL);
    if (th != 0) {
        // db_out_time("ini Thread_EngineInv  - FAIL!");
        return (-1);
    }
    return 0;
}

static void *Thread_EngineInv(void *arg) {

    u32 timer_100ms = 2;
    u32 timer_500ms = 10;
    u8 EnInv =1;
    while (1) {
        //~~~~~~~~~~~~~~~~~~~~
        tCanMessage *tMsg = (tCanMessage *)&buff[2];
        u32 n = CANReadFiFo(FifoRX_INVERTOR, tMsg, 1);

        // if (GetVar(HDL_IGNITION_IS_ON))
        // {
        if (n) {
            parseInvert(tMsg);
            Inv_Engine.Inv_OnLine = 1;
        }
//////simulation/////
#ifdef SIMULATION
        extern tCanMessage *sim_msg;
        if (gsThreadMutexTrylock(&mtx) != 0) {
            if ((sim_msg != NULL) && (sim_msg->id == 0x181)) {
                parseInvert(sim_msg);
                sim_msg = NULL;
            }
            gsThreadMutexUnlock(&mtx);
        }
#endif
        ///////////
        u32 nowInv_OverTerm = ((GetVar(HDL_INVERTER_OVER_TEMP) & 2) == 2);
        vechleicons_Set_stat(TERM_INV, (nowInv_OverTerm * 3));
        // }
        // else {
        //     resetAll();
        // }
        if(Inv_Engine.OnOff == START){
            EnInv = 1;
            Inv_Engine.OnOff = ON_START;
        }else if(Inv_Engine.OnOff == ON_START){
            if(GetVar(HDL_INVERTER_ON)){
                Inv_Engine.OnOff = ON_WORK;
            }
        }else if(Inv_Engine.OnOff == STOP){
                EnInv =0;
                Inv_Engine.OnOff = WAIT_STOP;
        }else if(Inv_Engine.OnOff == WAIT_STOP){
            if(GetVar(HDL_INVERTER_ON)==0)
                Inv_Engine.OnOff = IDLE;
        }

        if (timer_100ms)
            timer_100ms--;
        else {
          sendCmd_EngInv(EnInv);
            timer_100ms = 10;
        }
        if (timer_500ms)
            timer_500ms--;
        else {

            if(resetErr) EnInv = 0;
            resetErr =0;
            timer_500ms = 50;
        }


        usleep(10000);  /// 10 ms
    }
    return NULL;
}

static void sendCmd_EngInv(u8 En) {
    tCanMessage tx_msg;
    tx_msg.channel = ShassieCAN_ch;
    tx_msg.ext = 0;
    tx_msg.id = 0x194;
    tx_msg.len = 8;
    tx_msg.res = 0;
    tx_msg.data.u32[0] = En & 0x1;
    tx_msg.data.u16[2] = ((Inv_Engine.EmergensyStop & 0x1) | ((Inv_Engine.Retarder << 2) & 0x4)) * En;
    s16 powerAKB = GetVar(HDL_VOLTAGE_LEVEL);
    tx_msg.data.u16[3] = mabs(powerAKB);
    CANSendMsg(&tx_msg);
}

 void Inv_Eng_resetAll(void) {
    SetVar(HDL_INVERTER_ON, 0);
    SetVar(HDL_INVERTER_ERROR, 0);
    SetVar(HDL_INVERTER_WARN, 0);
    SetVar(HDL_INVERTER_OVER_TEMP, 0);

    approx(HDL_INVERTER_TORQUE, 0);
    approx(HDL_INVERTER_RPM, 0);
    approx(HDL_INVERTER_CURRENT, 0);
    approx(HDL_INVERTER_POWER, 0);
    approx(HDL_INVERTER_VOLTAGE, 0);

    vechleicons_Set_stat(TERM_INV, 0);
    Inv_Engine.OnOff = IDLE;
    Inv_Engine.Retarder = 0;
    Inv_Engine.EmergensyStop = 0;
    resetErr =1;
    Inv_Engine.Inv_OnLine = 0;
}

static int parseInvert(tCanMessage *msg) {
    u16 Reg = msg->id;
    if (Reg == 0x181) {
        u8 f_er = msg->data.u8[0] & 1;
        SetVar(HDL_INVERTER_ON, f_er);
        f_er = (msg->data.u8[0] & 2) >> 1;
        SetVar(HDL_INVERTER_ERROR,  f_er);
        SetVar(HDL_INV_LED_E_ENG,  f_er+1);

        f_er = (msg->data.u8[0] & 4) >> 2;
        SetVar(HDL_INVERTER_WARN, f_er);
        SetVar(HDL_INV_LED_W_ENG,  f_er+1);

        f_er = (msg->data.u8[0] & 8) >> 3;
        SetVar(HDL_INVERTER_OVER_TEMP, f_er);
        SetVar(HDL_INV_LED_T_ENG,  f_er+1);

        s32 t = (s8)msg->data.s8[1];
        // t += 127;
        // t = (t * 200) / 255 - 100;
        t = (t * 100) / 127;
        // approx(HDL_INVERTER_TORQUE, t);
        SetVar(HDL_INVERTER_TORQUE, t);
        t = (s16)(msg->data.u8[2] | (msg->data.u8[3] << 8));
        // t += 32767;
        // t = (t * 16000) / 65535 - 8000;
        t = t * 8000/32767;
        // approx(HDL_INVERTER_RPM, t);
        SetVar(HDL_INVERTER_RPM, mabs(t));

        t = msg->data.s8[4];
        // t += 650;
        // t = (t * 1300) / 255 - 650;
        t = (t * 650) / 127;
        // approx(HDL_INVERTER_CURRENT, t);
        SetVar(HDL_INVERTER_CURRENT, t);

        t = msg->data.s8[5];
        // t += 150;
        // t = (t * 300) / 255 - 150;
        t = (t * 150) / 127;
        // approx(HDL_INVERTER_POWER, t);
        SetVar(HDL_INVERTER_POWER, t);

        u32 v = msg->data.u8[6];
        v = (v * 1620) / 255;
        // approx(HDL_INVERTER_VOLTAGE, v);
        SetVar(HDL_INVERTER_VOLTAGE, v);
    }

    return 0;
}
