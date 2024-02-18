


/*
 * simul.c

 */

#include "main_px_bat.h"
//**************************************
tCanMessage* sim_msg = NULL;
tGsThread id_SIMULATIONThread;
extern s32 FifoRX_j1939;
void* Thread_SIMULATION(void* arg);
//**************************************
u8 ign = 0, invErr = 0, invWarn = 0, invTerm =0;
typedef struct dynamicInc_Dec {
    float step;
    s32 min, max;
    float now_Val;
    float res;
    float offset;
} dInc_Dec;
dInc_Dec dInc[30] = {
    {20, 50, 600, 132, 0.05f, 0},
    {5, 290, 420, 300, 0.05f, 0},
    {20, -560, 600, 30, 0.05f, -1600.0f},
    {2, 5, 99, 25, 0.0015625f, 0},
    {1, 30, 82, 36, 0.001f, 0},
    {1, 4, 42, 8, 0.001f, 0},
    {12, 120, 460, 160, 0.01f, 0},

    {1, 0, 1, 0, 1, 0},
    {1, 0, 1, 1, 1, 0},
    {1, 0, 1, 0, 1, 0},
    {1, 0, 1, 1, 1, 0},

    {1, -50, 85, -8, 1.27, 0},
    {10, 550, 1150, 880, 4.096, 0},
    {5, 50, 385, 80, 0.1953846, 0},
    {1, -50, 85, -8, 1.17647, 0},
    {7, 200, 350, 220, 0.1574, 0},

};
int div = 10, divII = 6;
float inc_dec(dInc_Dec* n) {
    if(div)div--;
    else {
        n->now_Val += n->step;
        if ((n->now_Val > n->max) || (n->now_Val < n->min)) n->step *= (-1.0f);
        // div = 3;
    }
    float fRet = (n->now_Val-n->offset)/n->res ;
    return fRet ;
}
s32 inc_dec_II(dInc_Dec* n) {
    if(divII)divII--;
    else {
        n->now_Val += n->step;
        if ((n->now_Val > n->max) || (n->now_Val < n->min)) n->step *= (-1.0f);
        // divII = 5;
    }

    return  (s32)(n->now_Val * n->res);
}
int Ini_SIMULATION_Thread(void) {
    int th =
        gsThreadCreate(&id_SIMULATIONThread, NULL, Thread_SIMULATION, NULL);
    if (th != 0) {
        // db_out_time("ini Thread_AKB  - FAIL!");
        return (-1);
    }
    if (gsThreadMutexInit(&mtx, NULL) != 0) return (-1);
    return 0;
}

tCanMessage buffSim[10];
void* Thread_SIMULATION(void* arg) {
    usleep(1000000);  /// 5 ms
    u8 indxSim = 0;
    while (1) {
        tCanMessage* tMsg = &buffSim[indxSim];
        tMsg->channel = ShassieCAN_ch;
        tMsg->ext = 1;
        tMsg->len = 8;
        tMsg->res = 0;
        switch (indxSim) {
            case 0:
                tMsg->id = 0x0CF0905B;
                tMsg->data.u16[0] = (u16)(inc_dec(&dInc[0]));
                tMsg->data.u16[2] = (u16)(inc_dec(&dInc[1]));
                tMsg->data.u16[3] = (u16)(inc_dec(&dInc[2]));
                break;
            case 1:
                tMsg->id = 0x0CF0915B;
                tMsg->data.u16[0] = (u16)(inc_dec(&dInc[3]));
                tMsg->data.u16[1] = (u16)(inc_dec(&dInc[4]));
                tMsg->data.u16[2] = (u16)(inc_dec(&dInc[5]));
                break;
            case 2:
                tMsg->id = 0x0CF0935B;
                tMsg->data.u16[0] = (u16)(inc_dec(&dInc[6]));
                break;
            case 3:
                tMsg->id = 0x181;
                tMsg->ext = 0;
                tMsg->data.u8[0] = 1;//(u8)(inc_dec(&dInc[7]));
                tMsg->data.u8[0] |= invErr<<1; //  ; (u8)(inc_dec(&dInc[8]))<<1;
                tMsg->data.u8[0] |= invWarn<<2; //(u8)(inc_dec(&dInc[9]))<<2;
                tMsg->data.u8[0] |= invTerm<<3; //(u8)(inc_dec(&dInc[10]))<<3;


// s32 t = -36;
                //tMsg->data.s8[1] =(s8)(t*127/100);
                 tMsg->data.s8[1] = (s8)(inc_dec_II(&dInc[11])); //

// t = -634;      t = t * 0x7FFF / 8000;
s16 t_16 =(s16)(inc_dec_II(&dInc[12]));
                tMsg->data.u8[2] = t_16  & 0xFF;
                tMsg->data.u8[3] =( t_16 >> 8)& 0xFF ;
// t = -434;
                tMsg->data.s8[4] =(s8)(inc_dec_II(&dInc[13])); //(s8)(t*127/650);
// t = -34;
                tMsg->data.s8[5] =(s8)(inc_dec_II(&dInc[14])); //(s8)(t*127/ 150);
// t = 530;
                tMsg->data.u8[6] =(u8)(inc_dec_II(&dInc[15])); //(u8)(t*255/1620);

                break;
            case 4:
                tMsg->id = 0x22;
                tMsg->ext = 0;
                tMsg->channel = MCM_CAN_ch;
                tMsg->data.u8[0] = 3;
                tMsg->data.u8[1] = ((u16)2000) & 0xFF;
                tMsg->data.u8[2] = (((u16)2000)>>8) & 0xFF ;
                tMsg->data.u8[3] = 0;
                tMsg->data.u32[1] = 0;
                tMsg->data.u32[1] |= ign;
                break;
            default:
                break;
        }


        while (gsThreadMutexTrylock(& mtx) != 0) {  };
        while (sim_msg != NULL) {  };
        sim_msg = tMsg;
        gsThreadMutexUnlock(& mtx);
        static int delay = 100 ;
        if(delay ){
            if(delay == 60) invErr ^= 1;
            if(delay == 40)  invWarn^= 1;
            if(delay == 30)    invTerm^= 1;
            delay--;
        }
        else {
            ign ^= 1;
            invErr = 0; invWarn = 0; invTerm =0;
            delay = 100;
        }

        if (++indxSim >  4) indxSim = 0;

        usleep(100000);  /// 5 ms
    }
    return NULL;
}
