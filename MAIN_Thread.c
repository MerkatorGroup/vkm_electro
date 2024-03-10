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
u8 check_Coolant_Level(void);
void Start_Stop_routine(void);
void checkInvHydErr(void);
void checkInvENG_Err(void);
void checkInvEngVoltage(void);
u8 fInvReady = 0;
e_mode_op mode_operation;
eStat fErrInvHyd;
eStat fErrInvEng;
u8 startQue = IDLE;
//**************************************
void MainReset(void) {
    Inv_Eng_resetAll();
    Inv_Hydr_resetAll();
    SetVar(HDL_BAT_LOW_VOLTAGE, 0);
    SetVar(HDL_INVERTER_VOLTAGE, 0);
    SetVar(HDL_INV_READY, 0);
    SetVar(HDL_IGNITION_IS_ON, 0);
    SetVar(HDL_DISCHARGE_CAP, 0);
    SetVar(HDL_BAT_LOW_LVL_CHRG, 0);
    SetVar(HDL_DIST_BAT, 0);
    SetVar(HDL_COOLANT_LOW_LVL, 0);
    vechleicons_Reset();
    mode_operation = IDLE;
}
int Ini_Thread_MAIN(void) {
    int th = gsThreadCreate(&id_MAIN_Thread, NULL, Thread_MAIN, NULL);
    if (th != 0) {
        // db_out_time("ini Thread_AKB  - FAIL!");
        return (-1);
    }
    return 0;
}

void *Thread_MAIN(void *arg) {
    MainReset();
    int r = Ini_MCM_Thread();
    r = Ini_EngInv_Thread();
    r = Ini_HydrInv_Thread();
    r = Ini_AKB_Thread();
    RelayPowerEn(ENABLE);

    while (1) {
        u8 fBatLowL = GetVar(HDL_BAT_LOW_LVL_CHRG);
        u8 fBatLowV = GetVar(HDL_BAT_LOW_VOLTAGE);
        u8 fLowCollant = check_Coolant_Level();

        fInvReady = 0;
        checkInvHydErr();
        checkInvENG_Err();
        if (((fErrInvEng != STAT_OK) || (fErrInvHyd != STAT_OK) ||
            fBatLowL || fBatLowV || fLowCollant) == 0) {
            fInvReady = 1;
            SetVar(HDL_INV_READY, fInvReady);
        }

        Start_Stop_routine();
        DashBoardCycle();
        checkInvEngVoltage();
        vechleicons_Set_stat(KEY_IGN, mode_operation == ON_WORK);
        vechleicons_show();

        usleep(10000);  /// 10 ms
    }
    return NULL;
}
void checkInvEngVoltage(void) {
    eStat fInvLowLvl = NOTAVLBL;
    if (mode_operation == ON_WORK) {
        u32 tllv = GetVar(HDL_INVERTER_VOLTAGE);
        if (tllv < INV_LOW_LEVEL)
            fInvLowLvl = FAIL;
        else if (tllv < INV_LOW_LEVELWARN)
            fInvLowLvl = WARN;
        else
            fInvLowLvl = STAT_OK;
    }
    SetVar(HDL_INV_LOW_LEVEL, fInvLowLvl);
}

void checkInvHydErr(void) {
    u8 nowInv_ERR_HYD = GetVar(HDL_INVERTER_ERROR_HYD);
    u8 nowInv_Warn_HYD = GetVar(HDL_INVERTER_WARN_HYD);
    u8 nowInv_Term_HYD = GetVar(HDL_INVERTER_OVER_TEMP_HYD);

    fErrInvHyd = NOTAVLBL;
    if (Inv_Hydr.Inv_OnLine) {
        if (nowInv_ERR_HYD)
            fErrInvHyd = FAIL;
        else if ((nowInv_Warn_HYD)||(nowInv_Term_HYD))
            fErrInvHyd = WARN;
        else
            fErrInvHyd = STAT_OK;
    }

    vechleicons_Set_stat(READY_HYD_INV, fErrInvHyd);
}
void checkInvENG_Err(void) {
    u8 nowInv_ERR = GetVar(HDL_INVERTER_ERROR);
    u8 nowInv_Warn = GetVar(HDL_INVERTER_WARN);
    u8 nowInv_Term = GetVar(HDL_INVERTER_OVER_TEMP);

    fErrInvEng = NOTAVLBL;
    if (Inv_Engine.Inv_OnLine) {
        if (nowInv_ERR)
            fErrInvEng = FAIL;
        else if ((nowInv_Warn)||(nowInv_Term))
            fErrInvEng = WARN;
        else
            fErrInvEng = STAT_OK;
    }

    vechleicons_Set_stat(READY_INV, fErrInvEng);
}
// void RelayIgnitionOn(u8 OnOff) { SetDOut(0, OnOff); }

// void SwitchIgnition(u8 En) {
//     SetDOut(0, En);
//     // if ((En) && (bat_OnOff == 0)) {
//     //     bat_OnOff = 1;
//     // } else if ((!En) && (bat_OnOff != 0)) {
//     //     bat_OnOff = 0;
//     // }
// }

void approx(int HDL, int val) {
    s32 now_val = GetVar(HDL);
    now_val += (val - now_val) * 200 / 1000;
    SetVar(HDL, now_val);
    // SetVar(HDL, val);
}

u8 check_Coolant_Level(void) {
    SetVar(HDL_COOLANT_LOW_LVL, 0);
    return GetVar(HDL_COOLANT_LOW_LVL);
}

void Start_Stop_routine(void) {
    startQue = IDLE;
    u8 key_pos = GetVar(HDL_KEY_POSITION);
    switch (mode_operation) {
        case IDLE:
            if (key_pos == 2) {
                if (fInvReady) {
                    HVBattery.BatON_OFF_REQ = START;
                    mode_operation = ON_START;
                }
                startQue = 1;
            }
            break;
        case ON_START:
            if (HVBattery.BatON_OFF_REQ == ON_WORK) {
                Inv_Hydr.OnOff = START;
                Inv_Engine.OnOff = START;
                mode_operation = WAIT;
            }
            startQue = 2;
            break;
        case WAIT:
            if ((HVBattery.BatON_OFF_REQ == ON_WORK) &&
                (Inv_Hydr.OnOff == ON_WORK) && (Inv_Engine.OnOff == ON_WORK)) {
                mode_operation = ON_WORK;
                SetDOut(6, 1);
            }
            startQue = 3;
            break;
        case ON_WORK:
            if (key_pos == 0) {
                Inv_Hydr.OnOff = STOP;
                Inv_Engine.OnOff = STOP;
                mode_operation = ON_STOP;
                startQue = 3;
            }
            break;
        case ON_STOP:
            if ((Inv_Hydr.OnOff == IDLE) && (Inv_Engine.OnOff == IDLE)) {
                HVBattery.BatON_OFF_REQ = STOP;
                mode_operation = WAIT_STOP;
            }
            startQue = 2;
            break;
        case WAIT_STOP:
            if (HVBattery.BatON_OFF_REQ == IDLE) {
                SetDOut(6, 0);
                mode_operation = IDLE;
            }
            startQue = 1;
            break;
        default:
            break;
    }
    SetVar(HDL_QUESTART, startQue);
}
