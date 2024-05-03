/****************************************************************************
 *
 * Project:   name
 *
 * @file      filename.h
 * @author
 * @date
 * @brief     description
 *
 ****************************************************************************/

/* Protection against multiple includes.                                    */
#ifndef MR_MCM_H
#define MR_MCM_H

#include "main_px_bat.h"
// #include "mr_incl.h"

//#define IsOnBand(x,val,ofset) x< (val- ofset) ? 0 : x > (val + ofset)  ? 0: 1

//#define IsInRange(x,min,max) x < min ? 0 : x > max ? 0: 1

 //#define abs(x) x<0?x *= -1:x


/**
* @brief
*/


typedef enum eagGSeMCM_TYPE {
	MCMTYPE_MCM200,
	MCMTYPE_MCM210,
	MCMTYPE_MCM250,
	MCMTYPE_BT20,
	MCMTYPE_GSK_K2_4x3,
	MCMTYPE_K1,
	MCMTYPE_K2_PLOUGH,
	MCMTYPE_JOYST
}eGSeMCM_TYPE;


typedef struct mr_MCM_FrCnt {
	uint32_t Freq;
	uint32_t Count;
	uint32_t time;
//	u32 curCntGlVar; /// глобальная переменная счета на входе
//	u32 curFrInGlVar; /// глобальная переменная частоты на входе
}mrMCM_FrCnt;

typedef struct _Cfg_strct{
	int32_t TX_ID[4];
	int32_t RX_ID;
	int32_t CAN;
	int32_t Ext;
	const char* name ;
}Cfg_strct;
typedef struct tIo_sett_Slv{
	uint8_t N_MCM;
	Cfg_strct Cfg;
//	uint8_t CAN;
//	uint8_t Ext;
//	uint32_t TX_ID;
//	uint32_t RX_ID;
//	const char* name;
	eGSeMCM_TYPE type_MCM;
} sett_SLAVE_Device;


typedef struct _t_IO_PWM {
	 u16 FreqValue;
	 u16 DutyValue;
	u8 shiftBit;  ///номер PWMOut, смещение в 32 битном значении регистра 2500
	u32 curentAin;/// значение тока mA
	u32 PWMout_Interval; //< интервал между сообщениями  PWMouts
	u32 PWMout_cntMs_ival;//< счетчик мс интервала между сообщениями
}t_IO_PWM;

typedef struct _tIO_DO {
	u32 val_FeedBACK;
	u8 shiftBit;  /// смещение в 32 битном значении hndl 2008
	u32 curentAin; /// значение тока mA
	u32 DO_Value;
}tIO_DO;

typedef struct tagIO_Ai {

  u16  Hndle;
  /// Время между обновлениями
	u32 EstimateTime;
	u32 AI_Value;
//	u32 GlAiVar; /// глобальная переменная напряжения на входе

} tIO_AI;

typedef struct tagIO_Di {
	u8 shiftBit; /// смещение в 32 битном значении hndl 2000
  u8 DI_Value;
//	u32 GlVar;   /// глобальная переменная дискрета на входе
} tIO_DI;

typedef enum _typeIO{
NOT_USE =0,
DInp = 0x7D0,  //2000
DOut = 0x7D8,  //2008
DOutFB = 0x7DC,  //2012
Frq_Cnt_In =  0xBBC, // 3004
PWMout = 0x9C4, //2500
AnInp = 0x7E0  // 2016
}eTypeIo;


/**
* @brief
*/
typedef struct mTagIO_tdf {
	u8 PinNum;
	eTypeIo typeIo;
	char Name[50];
	u16 Hndle;
	u32 EstimateTime; //< Время между обновлениями
	union {
		t_IO_PWM Pwm;
		tIO_DI DI;
		tIO_DO DO;
		mrMCM_FrCnt FrCnt;
		tIO_AI An_in;
	} cast;
	u8 NumAnInp; // номер аналогового входа

}inpOutp_tdf;

/* @brief|    bit8    |   bit7-6   |   bit5-4    | bit3-0
         |Proportional| num ports  |num ctrl pins|id types
*/
// typedef enum	_valv_typ {
// 	Unknown_Valv = 0,
// 	Valve_PVEO_1 = 0x51,
// 	Valve_PVEO_2 = 0xA2,
// 	Valve_PVEH_16 = 0x192,
// 	Valve_PVEH_32 = 0x193,
// 	Valve_RG = 0xA4,
// 	Valve_OMFB = 0x165
// }valv_typ;


// typedef struct strEquipCTRL {
// inpOutp_tdf * EqCtrl_a;
// inpOutp_tdf * EqCtrl_b;
// valv_typ valve_type;
// uint16_t flow;
// } EquipCTRL;

// typedef struct _Actions_Arr {
// 	EquipCTRL EquiP[12];
// }Actions_Arr;

typedef enum _eAtchClassType {
	eUnknow =0,
	ePLOUGH = 0xA0,
	eBRUSH,
	eSAND,
	eWASHER,
	eTANK,   // поливо мойка
	eReagentDistrib, // уст распр ж реагентов
	eExtHandle // экзотическое оборудование
}  eAtchClassType;


// typedef struct _ClassAttach {
// 		eAtchClassType type;
// 		Actions_Arr*Actions;
// }ClassAttach;

// typedef struct _tAttachModul {
// 	char	name[64];	///< Textlistentry to define the Name of the module
// 	uint32_t containerNum;		///< this Container will be shown in the main view, if the Module is active
//  ClassAttach Class;
// } tAttachModul;

typedef struct _tagMR_MCM250 {
	u32  digOut;	//<	отправляемое значение DOUTs  в регистр 2008
	int Var_digOut; //< перменная глобальная
	u32 DO_Interval; //< интервал между сообщениями  DOUTs
	u32 DO_cntMs_ival;//< счетчик мс интервала между сообщениями  DOUTs
	inpOutp_tdf* pIOs;
	inpOutp_tdf* pPWM_io[4]; //< список ШИМ выходов, заполняется в makePwmArray
u32 MaskPWM ;
} tagMR_MCM250;

typedef struct tMr_SLV_DEV {
	uint32_t   polling_ms;
	uint32_t   polling_t_old;
	uint32_t   Ext;
	uint32_t   CAN;
	uint32_t   RX_Id;
	uint32_t   TX_Id;
	uint32_t   FiFo;
	int32_t   Temp;
	uint32_t   Volt;
	uint32_t   Last_Msg_ms;
	uint32_t   state;
	const char*       DeviceName;
	uint16_t   	array_Msg[10];
	uint8_t 		QueneMsg  ;
	uint8_t 		cntMsg   ;
	uint8_t 		IDnumMCM ;
	u32 				timeReqDout;

	eGSeMCM_TYPE Type;
	union {
		tagMR_MCM250     MCM250;
/*		tGS_BT20       BT20;
		tGS_GSK_K2_4x3 K2_4x3;
		tGS_K1         K1;*/
	}MCM;
}tMr_SLV_DEV;




typedef struct _HydroEng{
 u32  Capacity_ml; // ml/rotate
 float KPD;
 float GearRatio ;
 float k_vs_ratio ;
}HydroEng;



typedef enum _InerpretPINS {
pin3 = 0, pin4, pin5, pin6, pin7, pin8, pin9, pin10,
pin20, pin21, pin22, pin23, pin24, pin25, pin26, pin27, pin28, pin29
}InerpretPINS;

typedef enum _InerpretDO_num {
DO_p3 =1,				// DO_0
DO_p4 = 4,				// DO_2
DO_p5 = 2,				// DO_1
DO_p6 = 8,				// DO_3
DO_p7 = 32,				// DO_5
DO_p8 = 128,			// DO_7
DO_p9 = 16,				// DO_4
DO_p10 = 64,				// DO_6
DO_p28 = 256,				// DO_8
DO_p29 = 512,				// DO_9
}InerpretDO_num;


/**
* @brief
*
* @return
*/
u32 CanMsgManager(void );
/**
* @brief
*
* @return
*/
int32_t  MCM250_Callback( tCanMessage* msg );


 int  MR_SetDO(tMr_SLV_DEV * MCM, u8 nPin, u8 Val );
 int MR_SetDO_byName(char* nameIO_do , u8 Val );
 int MR_MCM_SendDout(tMr_SLV_DEV * MCM );
int MR_MCM_RequestStatDout(tMr_SLV_DEV * MCM );

 tMr_SLV_DEV * GetSlaveDevByMCMNum(u8 Num);
/**
 @brief  устанавливает в глобальные переменные значения Freq и Duty
 @param  MCM - MCM250
 @param  num - номер PWM выхода
 @return
*/
int MR_MCM_SetPWMOut( tMr_SLV_DEV* MCM, u8 num, u16 Frq, u16 Duty );
/**
 @brief  устанавливает в глобальные переменные значения Freq и Duty
 ////////@param  nameMCM - имя MCM250
 @param  nameIO - имя PWM выхода
 @return
*/
int MR_MCM_SetPWMOut_by_Name(char* nameIO, u16 Frq, u16 Duty );
/**
* @brief
* @return
*/
void MR_RequestPwmOut(tMr_SLV_DEV * MCM);
/**
* @brief
* @return
*/
void makePwmArray(tMr_SLV_DEV * MCM );


void parseDout(inpOutp_tdf* currPin, u32 data);
/****************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !     */
/****************************************************************************/
#endif  // #ifndef MR_MCM_H
