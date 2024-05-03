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
#include <stdbool.h>


#define GET_SOURCE(x) (x & 0xFF)
#define GET_PGN(x) ((x & 0xFFFF00) >> 8)
//u8 CAN_Send_Time_Duty = 10;
static tCanMessage buff[16];
// u8 WDT_LEDS[32];
static tGsThread id_AKBThread;
static s32 FifoRX_j1939_BATT;
static void* Thread_AKB(void *arg);
BatProfile HVBattery ;
// static void SendToAKB(void);
static void ShowPowerDisCharge(void);
static void ShowVoltagLvl(void);
static void ShowFstUpdatCharge(void);
static void ShowCurrent(void);
static void ShowLowCellVoltage ( void);
static void ShowHiCellVoltage(void);
static void ShowDischargeCap (void);
static void SendRequest_BAT_ON_OFF(u8 OnOff, u8 Emergency);
// static void AKB_WDT_Timer(void);
static void ShowBAT_SOC (void );
static void ShowBAT_DIST(void);
// static int parsePGN(tCanMessage*msg);

static void HVBatteryReset(void) {
    // HVBattery.AvlblDischargePower = 0;
    // HVBattery.VoltagLvl = 0;
    // HVBattery.Current = 0;
    // HVBattery.FstUpdatCharge = 0;
    // HVBattery.HiCellVoltage = 0;
    // HVBattery.LowCellVoltage = 0;
    // HVBattery.DischargeCap = 0;
		// HVBattery.SOC = 0;
		// HVBattery.RemainingDistance = 0;
    if (HVBattery.Fifo_Id != 0)
        CANRemoveFiFo(HVBattery.Fifo_Id);
    HVBattery.Fifo_Id = 0;
		HVBattery.IsJ1939 =0;
		HVBattery.IsOnLine =0;
		HVBattery.BatON_OFF_REQ = 0;
}

//**************************************
int Ini_AKB_Thread(void) {
   HVBatteryReset();
	FifoRX_j1939_BATT = CANCreateFiFo(64);
	int rs= CANAddRxFilterFiFo( ShassieCAN_ch,0x0CF0905B ,0x1FFFF0FF , 1, FifoRX_j1939_BATT );
	rs= CANAddRxFilterFiFo( ShassieCAN_ch,0x18FCB75B ,0x1FFFFFFF , 1, FifoRX_j1939_BATT );



	int th = gsThreadCreate(&id_AKBThread, NULL, Thread_AKB, NULL);
	if (th != 0) {
		//db_out_time("ini Thread_AKB  - FAIL!");
		return (-1);
	}
    return 0;
}

static void* Thread_AKB(void* arg) {
		u32 Timer100ms = 10;
		u32 WDT_AKB_Online = 0;
		u8 EnableAKB = 0;
		u32 time_TESTOUT_EN = 0;
    while (1) {
        tCanMessage* tMsg = (tCanMessage*)&buff[0];
        u32 n = CANReadFiFo(FifoRX_j1939_BATT, tMsg, 1);
        if (n) {
            HVBattery.IsJ1939 = 1;
						HVBattery.IsOnLine = 1;
						WDT_AKB_Online = 100;
            // parsePGN(tMsg);
						parsePGN(tMsg);
        }
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
        ShowPowerDisCharge();
        ShowVoltagLvl();
        ShowCurrent();
        ShowFstUpdatCharge();
        ShowHiCellVoltage();
        ShowLowCellVoltage();
        ShowDischargeCap();
				ShowBAT_SOC();
				ShowBAT_DIST();

				if(HVBattery.BatON_OFF_REQ == START){
					EnableAKB = 1;
					HVBattery.BatON_OFF_REQ = ON_START;
				}else if(HVBattery.BatON_OFF_REQ == ON_START ){
					u8 valid_p, valid_n;
					s32 pos_cont = GetValueSpn(8091, &valid_p);
					s32 neg_cont = GetValueSpn(8092, &valid_n);
					if((valid_p == 1)&&(valid_n == 1)){
						if((pos_cont == 1)&& (neg_cont == 1))
						HVBattery.BatON_OFF_REQ =ON_WORK;
						time_TESTOUT_EN = 501;
					}
				}else if(HVBattery.BatON_OFF_REQ == STOP ){
					EnableAKB = 0;
					HVBattery.BatON_OFF_REQ = WAIT_STOP;
				}else if(HVBattery.BatON_OFF_REQ == WAIT_STOP ){
					u8 valid_p, valid_n;
					s32 pos_cont = GetValueSpn(8091, &valid_p);
					s32 neg_cont = GetValueSpn(8092, &valid_n);
					if((valid_p == 1)&&(valid_n == 1)){
						if((pos_cont == 0)&& (neg_cont == 0))
						HVBattery.BatON_OFF_REQ = IDLE;

						MR_SetDO_byName("test_out", 0);
					}
				}

				if (time_TESTOUT_EN) {
						time_TESTOUT_EN--;
						if (time_TESTOUT_EN == 1) {
								time_TESTOUT_EN = 0;
								MR_SetDO_byName("test_out", 1);
						}
				}

        // u32 tWd = 0;
        // for (size_t i = 0; i < 32; i++) tWd = WDT_LEDS[i];
        // AKB_WDT_Timer();
				if(Timer100ms)Timer100ms--;
				else{
					// SendRequest_BAT_ON_OFF(HVBattery.BatON_OFF_REQ,0);
					if(WDT_AKB_Online)WDT_AKB_Online--;
					else HVBattery.IsOnLine =0;
					SetVar(HDL_AKB_AVLBL, HVBattery.IsOnLine );
					SendRequest_BAT_ON_OFF(EnableAKB,0);
					Timer100ms =10;
				}

        usleep(10000);  /// 10 ms
    }
    return NULL;
}

/* ****   SPN 8123 PGN 1B5B 1.1 2 bit
Команда для подключения системы хранения энергии высокого напряжения
к высоковольтной шине. Запрос на аварийное замыкание контактора может использоваться
в ситуациях, когда одна или несколько неисправностей гибридной трансмиссии
препятствуют нормальной работе, но оператор транспортного средства желает
работать из-за предполагаемой аварийной ситуации. Приемники этой команды
всегда должны поддерживать состояние 10b (запрос на аварийное замыкание контактора),
даже если критерии замыкания контактора такие же, как и при ответе на состояние 01b.

00b = Open Contactor; Disconnect HVES from high-voltage bus
01b = Close Contactor; Connect HVES to high-voltage bus
10b = Emergency contactor closure request
11b = Don't care/Take no action
*/

/* -----   SPN 8124 PGN 1B5B 1.3 2 bit
Команда отключения системы хранения энергии высокого напряжения.
Обычное отключение питания может включать в себя действия, занимающие определенный
период времени (тесты, запись данных и т. д.). Однако аварийное отключение питания
должно привести к отключению системы хранения как можно быстрее, например, в случае
автомобильной аварии или проблемы безопасности. Приемники этой команды всегда должны
поддерживать состояние 10b (Выполнение аварийного отключения питания),
даже если фактический процесс отключения питания такой же, как тот, который
используется во время «обычного» отключения питания.

00b = Power-down not requested
01b = Execute normal power-down
10b = Execute emergency power-down
11b = Don't care/Take no action
*/
static void SendRequest_BAT_ON_OFF(u8 OnOff, u8 Emergency) {
    u8 vSend = 0x00;
		if (OnOff) vSend = 1;
		else vSend = 4;
    if (Emergency) vSend <<= 1;
		vSend |= 0xF0;


    tCanMessage tx_msg;
    tx_msg.channel = ShassieCAN_ch;
    tx_msg.ext = 1;
    tx_msg.id = 0x0C1B5B21;
    tx_msg.len = 8;
    tx_msg.res = 0;
	tx_msg.data.u32[0] =0xFFFFFFFF;
	tx_msg.data.u32[1] =0xFFFFFFFF;
		tx_msg.data.u8[0] &= vSend;
    CANSendMsg(&tx_msg);
}
// static int parsePGN(tCanMessage*msg) {

// 	u16 CurrPGN = GET_PGN(msg->id);
// 	u8 CurrSourse = GET_SOURCE(msg->id);
// 	if(CurrSourse == 0x5B){
// 	u16 Val = 0;
// 		switch (CurrPGN){
// 		case 0xF090:

// 			Val = msg->data.u8[0];
// 			Val |= (u16)msg->data.u8[1] << 8;
// 			if(Val> 0xFAFF) {
// 				SetVar(HDL_AVDISPOW_LED_1,2);
// 			}else{
// 				HVBattery.AvlblDischargePower = (float)Val * 0.05f;
// 				SetVar(HDL_AVDISPOW_LED_1,1);
// 			}
// 				WDT_LEDS[0]= WDT_TIME;

// 			Val = msg->data.u8[4];
// 			Val |= (u16)msg->data.u8[5] << 8;
// 			if(Val> 0xFAFF) {
// 				SetVar(HDL_AVDISPOW_LED_2 ,2);
// 			}else{
// 				HVBattery.VoltagLvl = (float)Val * 0.05f;
// 				SetVar(HDL_AVDISPOW_LED_2 ,1);
// 			}
// 				WDT_LEDS[1]= WDT_TIME;

// 			Val = msg->data.u16[3];
// 			// Val |= (u16)msg->data.u8[7] << 8;
// 			if(Val> 0xFAFF) {
// 				SetVar(HDL_AVDISPOW_LED_3,2);
// 			}else{
// 				HVBattery.Current = (float)Val * 0.05f - 1600.0f;
// 				SetVar(HDL_AVDISPOW_LED_3,1);
// 			}
// 				WDT_LEDS[2]= WDT_TIME;
// 			break;
// 		case 0xF091:
// 			Val = msg->data.u8[0];
// 			Val |= (u16)msg->data.u8[1] << 8;
// 			if(Val> 0xFAFF) {
// 				SetVar(HDL_AVDISPOW_LED_4,2);
// 			}else{
// 				HVBattery.FstUpdatCharge = (float)Val /640.0f;
// 				SetVar(HDL_AVDISPOW_LED_4,1);
// 			}
// 				WDT_LEDS[3]= WDT_TIME;

// 			Val = msg->data.u8[2];
// 			Val |= (u16)msg->data.u8[3] << 8;
// 			if(Val> 0xFAFF) {
// 				SetVar(HDL_AVDISPOW_LED_5,2);
// 			}else{
// 				HVBattery.HiCellVoltage = (float)Val * 0.001f;
// 				SetVar(HDL_AVDISPOW_LED_5,1);
// 			}
// 				WDT_LEDS[4]= WDT_TIME;

// 			Val = msg->data.u8[4];
// 			Val |= (u16)msg->data.u8[5] << 8;
// 			if(Val> 0xFAFF) {
// 				SetVar(HDL_AVDISPOW_LED_6,2);
// 			}else{
// 				HVBattery.LowCellVoltage = (float)Val * 0.001f;
// 				SetVar(HDL_AVDISPOW_LED_6,1);
// 			}
// 				WDT_LEDS[5]= WDT_TIME;
// 			break;
// 		case 0xF093:
// 			Val = msg->data.u8[0];
// 			Val |= (u16)msg->data.u8[1] << 8;
// 			if(Val> 0xFAFF) {
// 				SetVar(HDL_AVDISPOW_LED_7,2);
// 			}else{
// 				HVBattery.DischargeCap = (float)Val * 0.01f;
// 				SetVar(HDL_AVDISPOW_LED_7,1);
// 			}
// 				WDT_LEDS[6]= WDT_TIME;
// 			break;
// 		case 0xFCB7:
// 		// Battery Pack Remaining Charge  %
// 			Val = msg->data.u8[0];
// 			Val |= (u16)msg->data.u8[1] << 8;
// 			HVBattery.SOC = (float)Val * 0.0025f;

// 		// Remaining Distance (Расчетный запас хода) km
// 			Val = msg->data.u8[4];
// 			Val |= (u16)msg->data.u8[5] << 8;
// 			HVBattery.RemainingDistance = (float)Val * 0.03125f;

// 			break;
// 		default:
// 			break;
// 		}
// 	}
// 	return 0;
// }

static s32 SetParam_AKB(u32 spn, int hdl_akb, int hdl_led) {
    u8 valid;
    s32 value = GetValueSpn(spn, &valid);
    SetVar(hdl_akb, value);
 		RunSpnTimeOutExpiried(spn, hdl_led);
	// if(hdl_led){
	// 		if (valid != 0)
	// 				SetVar(hdl_led, valid);
	// 		else {
	// 				if (IsSpnTimeOutExpiried(spn))
	// 					SetVar(hdl_led, 0);
	// 		}
	// }
		return value;
}
static float SetFloatParam_AKB(u32 spn, int hdl_akb, int hdl_led) {
    u8 valid;
    float value = GetfValueSpn(spn, &valid);
    SetVarFloat(hdl_akb, value);
 		RunSpnTimeOutExpiried(spn, hdl_led);
		return value;
}
static void ShowPowerDisCharge(void){
	SetParam_AKB(5917,HDL_DISCH_PWR,HDL_AVDISPOW_LED_1);
	// if(WDT_LEDS[0])WDT_LEDS[0]--;
	// else SetVar(HDL_AVDISPOW_LED_1,0);
  // SetVar(HDL_DISCH_PWR, (s32) powDisCh );
}
static void ShowVoltagLvl(void) {
	s32 v = SetParam_AKB(5919,HDL_VOLTAGE_LEVEL ,HDL_AVDISPOW_LED_2);
    // if (WDT_LEDS[1])  WDT_LEDS[1]--;
    // else {
    //     SetVar(HDL_AVDISPOW_LED_2, 0);
    //     Volt = 0;
    // }
    // SetVar(HDL_VOLTAGE_LEVEL, (s32) Volt);
    u8 fLowLevel_v = v < BAT_LOW_VOLTAGE;
    SetVar(HDL_BAT_LOW_VOLTAGE, fLowLevel_v );
    vechleicons_Set_stat(LOW_VOLT_BAT, (fLowLevel_v  * 3 * HVBattery.IsOnLine));
}
static void ShowBAT_SOC(void){
	s32 soc = SetParam_AKB(5464,HDL_SOC_BAT,0);
  // SetVar(HDL_SOC_BAT,(s32)SOC);
    u8 fLowLevel = soc  < BAT_LOW_LEVEL;
    SetVar(HDL_BAT_LOW_LVL_CHRG, fLowLevel);
    vechleicons_Set_stat(LOW_LVL_CRG, (fLowLevel * 3 * HVBattery.IsOnLine));
}
static void ShowCurrent(void){
	SetParam_AKB(5920,HDL_CURRENT,HDL_AVDISPOW_LED_3);
	// if(WDT_LEDS[2])WDT_LEDS[2]--;
	// else SetVar(HDL_AVDISPOW_LED_3,0);
  // SetVar(HDL_CURRENT, (s32) Currnt );
}
static void ShowFstUpdatCharge(void){
	SetParam_AKB( 5921,HDL_FASTUPDCHARG ,HDL_AVDISPOW_LED_4 );
	// if(WDT_LEDS[3])WDT_LEDS[3]--;
	// else SetVar(,0);
  // SetVar(, (s32) FUCharge );
}
static void ShowHiCellVoltage(void ){
	SetFloatParam_AKB( 5922,HDL_HI_CELL_VOLT ,HDL_AVDISPOW_LED_5 );
	// if(WDT_LEDS[4])WDT_LEDS[4]--;
	// else SetVar(HDL_AVDISPOW_LED_5,0);
  // SetVarFloat(HDL_HI_CELL_VOLT, HiCllVolt);
}
static void ShowLowCellVoltage (void){
	SetFloatParam_AKB( 5923,HDL_LOW_CELL_VOLT ,HDL_AVDISPOW_LED_6 );

// 	if(WDT_LEDS[5])WDT_LEDS[5]--;
// 	else SetVar(HDL_AVDISPOW_LED_6,0);
//   SetVarFloat(,   LowCllVolt);
}
static void ShowDischargeCap (void){
	SetParam_AKB(8079,HDL_DISCHARGE_CAP,HDL_AVDISPOW_LED_7);
	// if(WDT_LEDS[6])WDT_LEDS[6]--;
	// else SetVar(,0);
  // SetVar(HDL_DISCHARGE_CAP,(s32) DischCap );
}
static void ShowBAT_DIST(void){
	SetParam_AKB(9709,HDL_DIST_BAT , 0);
  // SetVar(HDL_DIST_BAT,(s32) dist);
}
// static void AKB_WDT_Timer(void){
//     for (u8 i = 0; i < 7; i++) {
//         if (WDT_LEDS[i])
//             WDT_LEDS[i]--;
//         else
//             SetVar(HDL_AVDISPOW_LED_1 + i, 0);
//     }
// }
