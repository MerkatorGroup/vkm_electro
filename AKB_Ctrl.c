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


#define GET_SOURCE(x) (x & 0xFF)
#define GET_PGN(x) ((x & 0xFFFF00) >> 8)
//u8 CAN_Send_Time_Duty = 10;

u8 WDT_LEDS[32];
tGsThread id_AKBThread;
s32 FifoRX_j1939;
void* Thread_AKB(void *arg);

	float  AvlblDischargePower =0;
	float VoltagLvl = 0;
	float Current = 0;
	float FstUpdatCharge  = 0;
	float HiCellVoltage  = 0;
	float LowCellVoltage  = 0;
	float DischargeCap  = 0;


void ShowPowerDisCharge(float pow);
void ShowVoltagLvl(float Volt);
void ShowFstUpdatCharge(float FUCharge);
void ShowCurrent(float Currnt);
void ShowLowCellVoltage (float LowCllVolt );
void ShowHiCellVoltage(float HiCllVolt );
void ShowDischargeCap (float DischCap);
void SendRequest_BAT_ON_OFF(u8 OnOff);
void AKB_WDT_Timer(void);



int parsePGN(tCanMessage*msg);
//**************************************
int Ini_AKB_Thread(void) {

	FifoRX_j1939 = CANCreateFiFo(64);
	int rs= CANAddRxFilterFiFo( ShassieCAN_ch,0x0CF0905B ,0x1FFFFCFF , 1, FifoRX_j1939 );



	int th = gsThreadCreate(&id_AKBThread, NULL, Thread_AKB, NULL);
	if (th != 0) {
		//db_out_time("ini Thread_AKB  - FAIL!");
		return (-1);
	}
    return 0;
}


void* Thread_AKB(void* arg) {
    while (1) {
        tCanMessage* tMsg = (tCanMessage*)&buff[0];
        u32 n = CANReadFiFo(FifoRX_j1939, tMsg, 1);
        if (n) parsePGN(tMsg);
        //////simulation/////
#ifdef SIMULATION
		extern tCanMessage* sim_msg;
                if (gsThreadMutexTrylock(&mtx) != 0) {
                    if ((sim_msg != NULL) &&
                        ((sim_msg->id & 0x0CF0905B) == 0x0CF0905B)) {
                        parsePGN(sim_msg);
                        sim_msg = NULL;
                    }
                    gsThreadMutexUnlock(&mtx);
                }
#endif
        ///////////

        ShowPowerDisCharge(AvlblDischargePower);
        ShowVoltagLvl(VoltagLvl);
        ShowCurrent(Current);
        ShowFstUpdatCharge(FstUpdatCharge);
        ShowHiCellVoltage(HiCellVoltage);
        ShowLowCellVoltage(LowCellVoltage);
        ShowDischargeCap(DischargeCap);
		u32 tWd =0;
      for (size_t i = 0; i < 32; i++)  tWd =WDT_LEDS[i];
        SetVar(HDL_AKB_AVLBL,  tWd != 0 );
		// AKB_WDT_Timer();
        usleep(10000);  /// 5 ms
    }
    return NULL;
}

int parsePGN(tCanMessage*msg) {

	u16 CurrPGN = GET_PGN(msg->id);
	u8 CurrSourse = GET_SOURCE(msg->id);

	if(CurrSourse == 0x5B){
	u16 Val = 0;
		switch (CurrPGN){
		case 0xF090:

			Val = msg->data.u8[0];
			Val |= (u16)msg->data.u8[1] << 8;
			if(Val> 0xFAFF) {
				SetVar(HDL_AVDISPOW_LED_1,2);
			}else{
				AvlblDischargePower = (float)Val * 0.05f;
				SetVar(HDL_AVDISPOW_LED_1,1);
			}
				WDT_LEDS[0]= WDT_TIME;

			Val = msg->data.u8[4];
			Val |= (u16)msg->data.u8[5] << 8;
			if(Val> 0xFAFF) {
				SetVar(HDL_AVDISPOW_LED_2 ,2);
			}else{
				VoltagLvl = (float)Val * 0.05f;
				SetVar(HDL_AVDISPOW_LED_2 ,1);
			}
				WDT_LEDS[1]= WDT_TIME;

			Val = msg->data.u16[3];
			// Val |= (u16)msg->data.u8[7] << 8;
			if(Val> 0xFAFF) {
				SetVar(HDL_AVDISPOW_LED_3,2);
			}else{
				Current = (float)Val * 0.05f - 1600.0f;
				SetVar(HDL_AVDISPOW_LED_3,1);
			}
				WDT_LEDS[2]= WDT_TIME;
			break;
		case 0xF091:
			Val = msg->data.u8[0];
			Val |= (u16)msg->data.u8[1] << 8;
			if(Val> 0xFAFF) {
				SetVar(HDL_AVDISPOW_LED_4,2);
			}else{
				FstUpdatCharge = (float)Val /640.0f;
				SetVar(HDL_AVDISPOW_LED_4,1);
			}
				WDT_LEDS[3]= WDT_TIME;

			Val = msg->data.u8[2];
			Val |= (u16)msg->data.u8[3] << 8;
			if(Val> 0xFAFF) {
				SetVar(HDL_AVDISPOW_LED_5,2);
			}else{
				HiCellVoltage = (float)Val * 0.001f;
				SetVar(HDL_AVDISPOW_LED_5,1);
			}
				WDT_LEDS[4]= WDT_TIME;

			Val = msg->data.u8[4];
			Val |= (u16)msg->data.u8[5] << 8;
			if(Val> 0xFAFF) {
				SetVar(HDL_AVDISPOW_LED_6,2);
			}else{
				LowCellVoltage = (float)Val * 0.001f;
				SetVar(HDL_AVDISPOW_LED_6,1);
			}
				WDT_LEDS[5]= WDT_TIME;
			break;
		case 0xF093:
			Val = msg->data.u8[0];
			Val |= (u16)msg->data.u8[1] << 8;
			if(Val> 0xFAFF) {
				SetVar(HDL_AVDISPOW_LED_7,2);
			}else{
				DischargeCap = (float)Val * 0.01f;
				SetVar(HDL_AVDISPOW_LED_7,1);
			}
				WDT_LEDS[6]= WDT_TIME;
			break;

		default:
			break;
		}
	}
	return 0;
}

void ShowPowerDisCharge(float powDisCh){
	if(WDT_LEDS[0])WDT_LEDS[0]--;
	else SetVar(HDL_AVDISPOW_LED_1,0);
  SetVar(HDL_DISCH_PWR, (s32) powDisCh );
}
void ShowVoltagLvl(float Volt) {
    if (WDT_LEDS[1])  WDT_LEDS[1]--;
    else {
        SetVar(HDL_AVDISPOW_LED_2, 0);
        Volt = 0;
    }
    SetVar(HDL_VOLTAGE_LEVEL, (s32) Volt);

    u8 fLowLevel = Volt < BAT_LOW_LEVEL;
    SetVar(HDL_BAT_LOW_LEVEL, fLowLevel);
    vechleicons_Set_stat(LOW_BAT, (fLowLevel * 3));
}
void ShowCurrent(float Currnt){
	if(WDT_LEDS[2])WDT_LEDS[2]--;
	else SetVar(HDL_AVDISPOW_LED_3,0);
  SetVar(HDL_CURRENT, (s32) Currnt );
}

void ShowFstUpdatCharge(float FUCharge){
	if(WDT_LEDS[3])WDT_LEDS[3]--;
	else SetVar(HDL_AVDISPOW_LED_4,0);
  SetVar(HDL_FASTUPDCHARG, (s32) FUCharge );
}
void ShowHiCellVoltage(float HiCllVolt ){
	if(WDT_LEDS[4])WDT_LEDS[4]--;
	else SetVar(HDL_AVDISPOW_LED_5,0);
  SetVar(HDL_HI_CELL_VOLT, (s32) HiCllVolt );
}
void ShowLowCellVoltage (float LowCllVolt ){
	if(WDT_LEDS[5])WDT_LEDS[5]--;
	else SetVar(HDL_AVDISPOW_LED_6,0);
  SetVar(HDL_LOW_CELL_VOLT,  (s32) LowCllVolt);
}

void ShowDischargeCap (float DischCap){
	if(WDT_LEDS[6])WDT_LEDS[6]--;
	else SetVar(HDL_AVDISPOW_LED_7,0);
  SetVar(HDL_DISCHARGE_CAP,(s32) DischCap );
}

void AKB_WDT_Timer(void){
    for (u8 i = 0; i < 7; i++) {
        if (WDT_LEDS[i])
            WDT_LEDS[i]--;
        else
            SetVar(HDL_AVDISPOW_LED_1 + i, 0);
    }
}
