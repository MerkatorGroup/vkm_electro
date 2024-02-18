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

tGsThread id_MCMThread;
void *Thread_MCM(void *arg);
int parseMCM(tCanMessage *msg);
void MCM_request_DOut(void);
u8 MutexTX = 0;
s32 FifoRX_MCM;
u32 MCM_DOut = 0;
//**************************************
int Ini_MCM_Thread(void) {
    FifoRX_MCM = CANCreateFiFo(64);
    int rs = CANAddRxFilterFiFo(MCM_CAN_ch, 0x020, 0x7C0, 0, FifoRX_MCM);

    SetVar(HDL_IGNITION_IS_ON, 0);

    int th = gsThreadCreate(&id_MCMThread, NULL, Thread_MCM, NULL);
    if (th != 0) {
        // db_out_time("ini Thread_MCM - FAIL!");
        return (-1);
    }
    return 0;
}

void *Thread_MCM(void *arg) {
    u32 Timer_1 = 5;  // x20ms
    while (1) {
        //~~~~~~~~~~~~~~
        tCanMessage *tMsg = (tCanMessage *)&buff[1];
        u32 n = CANReadFiFo(FifoRX_MCM, tMsg, 1);
        if (n) parseMCM(tMsg);
        //////simulation/////
#ifdef SIMULATION
		extern tCanMessage* sim_msg;
        if (gsThreadMutexTrylock(&mtx) != 0) {
            if ((sim_msg != NULL) && (sim_msg->id == 0x22)) {
                 parseMCM(sim_msg);
                sim_msg = NULL;
            }
            gsThreadMutexUnlock(&mtx);
        }
#endif


        if (Timer_1) Timer_1--;  // 100ms
        else {
            MCM_request_DOut();
            Timer_1 = 5;
        }
        usleep(20000);  /// 20 ms
    }
    return NULL;
}

int parseMCM(tCanMessage *msg) {
    if ((0x03 == msg->data.u8[0])) {
        u16 CurrMCM = msg->id;
        u16 CurrHNDL = msg->data.u8[1] + (u16)(msg->data.u8[2] << 8);
        int32_t Data = msg->data.u32[1];
        if (CurrHNDL == 2000) {
            if (CurrMCM == 0x22) SetVar(HDL_IGNITION_IS_ON, Data & 1);
        }
    }

    return 0;
}

void SetDOut(u8 NumDout, u8 val) {
    while (MutexTX) {
    };
    if (val) MCM_DOut |= (1 << NumDout);
    else MCM_DOut &= ~(1 << NumDout);
}

void MCM_request_DOut(void) {
    MutexTX = 1;
    tCanMessage tx_msg;
    tx_msg.channel = MCM_CAN_ch;
    tx_msg.ext = MCM_ADR_EXT;
    tx_msg.id = TX_to_MCM_CAN_ADR;
    tx_msg.len = 8;
    tx_msg.res = 0;
    tx_msg.data.u8[0] = 2;
    tx_msg.data.u8[1] = 0xD8;
    tx_msg.data.u8[2] = 0x07;
    tx_msg.data.u8[3] = 0;          //
    tx_msg.data.u32[2] = MCM_DOut;  //
    CANSendMsg(&tx_msg);
    MutexTX = 0;
}
