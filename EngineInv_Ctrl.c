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

tGsThread id_EngInvThread;
void *Thread_EngineInv(void *arg);
s32 FifoRX_INVERTOR;
int parseInvert(tCanMessage *msg);
void resetAll(void);

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

void *Thread_EngineInv(void *arg) {
    resetAll();
    while (1) {
        //~~~~~~~~~~~~~~~~~~~~
        tCanMessage *tMsg = (tCanMessage *)&buff[2];
        u32 n = CANReadFiFo(FifoRX_INVERTOR, tMsg, 1);

        // if (GetVar(HDL_IGNITION_IS_ON))
        {
            if (n) parseInvert(tMsg);
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
        }
        // else {
        //     resetAll();
        // }
    }
    return NULL;
}

void resetAll(void) {
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
}

void approx(int HDL, int val) {
    s32 now_val = GetVar(HDL);
    now_val += (val - now_val) * 200 / 1000;
     SetVar(HDL, now_val);
    //SetVar(HDL, val);
}

int parseInvert(tCanMessage *msg) {
    u16 Reg = msg->id;
    if (Reg == 0x181) {
        u8 f_er = msg->data.u8[0] & 1;
        SetVar(HDL_INVERTER_ON, (1 << f_er));
        f_er = (msg->data.u8[0] & 2) >> 1;
        SetVar(HDL_INVERTER_ERROR, (1 << f_er));
        f_er = (msg->data.u8[0] & 4) >> 2;
        SetVar(HDL_INVERTER_WARN, (1 << f_er));
        f_er = (msg->data.u8[0] & 8) >> 3;
        SetVar(HDL_INVERTER_OVER_TEMP, (1 << f_er));

        s32 t = (s8)msg->data.s8[1];
        // t += 127;
        // t = (t * 200) / 255 - 100;
        t = (t * 100) / 127;
        approx(HDL_INVERTER_TORQUE, t);

        t = (s16)(msg->data.u8[2] | (msg->data.u8[3]<<8));
        t += 32767;
        t = (t * 16000) / 65535 - 8000;
        approx(HDL_INVERTER_RPM, t);

        t = msg->data.s8[4];
        // t += 650;
        // t = (t * 1300) / 255 - 650;
        t = (t * 650) / 127;
        approx(HDL_INVERTER_CURRENT, t);

        t = msg->data.s8[5];
        // t += 150;
        // t = (t * 300) / 255 - 150;
        t = (t * 150) / 127;
        approx(HDL_INVERTER_POWER, t);

        u32 v = msg->data.u8[6];
        v = (v * 1620) / 255;
        approx(HDL_INVERTER_VOLTAGE, v);
    }

    return 0;
}
