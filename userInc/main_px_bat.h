
#ifndef MAIN_PX_H
#define MAIN_PX_H


#ifndef SIMULATION
//#define SIMULATION
#endif


#include <stdint.h>
#include <UserCEvents.h> /* Events send to the Cycle function              */
#include <UserCAPI.h>	 /* API-Function declarations                      */

#include "vartab.h" /* Variable Table definitions:                    */

#include "objtab.h" /* Object ID definitions:  */
#include "gsToVisu.h"

#include "my_types.h"
#include "gsThread.h"
#include "gsMsgFiFo.h"
#include "VechleIcons.h"
#include "VertBar.h"
#include "simul.h"
#include "m_j1939.h"
#include "mr_MCM.h"
#include "work_site.h"



#define IsOnBand(x,val,ofset) x< (val- ofset) ? 0 : x > (val + ofset)  ? 0: 1
#define IsOutBand(x,val,ofset) x< (val- ofset) ? (-1) : x > (val + ofset)  ? 1: 0
#define IsInRange(x,min,max) x< min ? 0 : x > max ? 0: 1
#define concat(a,b,c) a##b##c
#define IsSET(x,bitMask) (((x) & (bitMask)) != 0)
#define FillOnCircL(x,min,max)  x < min ?  (x = max)  :  x > max ? (x =min)  : (x = x)
#define FillOnBand(x,min,max)  x < min ?  (x = min )  :  x > max ? (x = max)  : (x = x)
#define constrain(x,min,max)  x < min ?  min   :  x > max ? max   : (x )
#define dec_elapced(x) x > 0 ? x--, 0 : 1


#define ENABLE 1
#define DISABLE 0
#define RelayPowerEn(x) SetDOut(0,x)

#define ShassieCAN_ch 1
#define MCM_CAN_ch 0
#define TX_to_MCM_CAN_ADR 0x21
#define MCM_ADR_EXT 0

#define BAT_LOW_VOLTAGE 300
#define BAT_LOW_LEVEL 20

#define INV_LOW_LEVEL 280
#define INV_LOW_LEVELWARN (INV_LOW_LEVEL*105/100)
#define WDT_TIME 50

#define mabs(x) x<0?x*=(-1):x

typedef enum _e_mode_op{
	IDLE = 0,
	START,
	ON_START,
	WAIT,
	ON_WORK,
	STOP,
	ON_STOP,
	WAIT_STOP
}e_mode_op;


extern u8 mode_operation;


extern void Inv_Eng_resetAll(void);
extern int Ini_EngInv_Thread(void);

extern int Ini_AKB_Thread(void) ;
extern int Ini_HydrInv_Thread(void);
extern void make_hyd_torq_vs_rpm(u32 reqRpm);

extern void Inv_Hydr_resetAll(void);
extern int Ini_MCM_Thread(void);
extern int Ini_Thread_MAIN(void);
//extern 	tCanMessage buff[16];
extern 	void approx(int HDL, int val);
extern 	void SetDOut(u8 NumDout, u8 val);
extern 	void approx(int HDL, int val);
extern tGsMutex mtx;
////////////  BATERY  //////////
typedef struct _t_BatProfile{
	s32 Fifo_Id;
  // float  AvlblDischargePower;
	// float VoltagLvl;
	// float Current;
	// float FstUpdatCharge ;
	// float HiCellVoltage ;
	// float LowCellVoltage ;
	// float DischargeCap;
	// float SOC; //Remaining Charge (SOC for Cabin Display) spn5464
	// float RemainingDistance;
  u8 IsJ1939;
	u8 IsOnLine;
	e_mode_op BatON_OFF_REQ;

}BatProfile;
extern BatProfile HVBattery;
extern int Ini_AKB_Thread(void) ;
// void HVBatteryReset(void) ;
// typedef struct _t_j1939_spn {
// 	char* SPN;	//< имя spn
// 	u32 PGN;	//< Код группы
// 	u8 bSize;	//< Размер в битах
// 	float resolution;
// 	s32 RangeMinMax[2];
// 	u8 bByte;	//< N байта 1..8
// 	u8 bPos;	//< Позиция в битах 1..8 *
// 	////  ******runtime***
// 	u32 varName;	//< Адрес переменной Spn_name TextResource
// 	u32 varObjId;	//< Id объекта VISU
// 	u32 varValue;	//< Адрес переменной spnVal
// 	////  ******

// 	float floatValue;
// 	u32 timeout;
// 	u8 IsValid;
// 	u8 ValidSource;
// } t_j1939_spn;

// t_j1939_spn SpnArr[] ={
// 	{"PowerDisCharge",0xF091,16,0.05,
// }

//////////  INV ///////////
typedef struct _t_Inv_Profile{
e_mode_op OnOff;
u8 Retarder;
u8 EmergensyStop;
s8 TorqPercent;
u8 Inv_OnLine;
}t_Inv_Profile;
extern t_Inv_Profile Inv_Engine;
extern  t_Inv_Profile Inv_Hydr;
extern  tMr_SLV_DEV SlvMCMs[4];
#endif