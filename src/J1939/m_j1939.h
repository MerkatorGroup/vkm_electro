
/*
 * j1939.h
 *
 *  Created on: 31 марта 2023 г.
 *      Author: ASUS
 */

#ifndef J1939_J1939_H_
#define J1939_J1939_H_

#include "main_px_bat.h"


#define GET_SOURCE(x) (x & 0xFF)
#define GET_PGN(x) ((x & 0xFFFF00) >> 8)


typedef struct _t_j1939_spn {
	u32 SPN;	//< имя spn
	u32 PGN;	//< Код группы
	u8 bSize;	//< Размер в битах
	float resolution;
	s32 RangeMinMax[2];
	u8 bByte;	//< N байта 1..8
	u8 bPos;	//< Позиция в битах 1..8 *

	// ////  ******runtime***
	// u32 varName;	//< Адрес переменной Spn_name TextResource
	// u32 varObjId;	//< Id объекта VISU
	// u32 varValue;	//< Адрес переменной spnVal
	// ////  ******
	float floatValue;
	u32 TimeOutValid;
	u8 IsValid;
	// u8 ValidSource;
} t_j1939_spn;

// typedef struct _dumpSpn{
// 	u32 Pgn;
// 	const t_j1939_spn* pspn;
// 	u32 n;
// 	u32 TimeOutValid;
// //	u16 LastSource;
// }dumpSpn;



float GetfValueSpn(u32 spn, u8 *valid);
s32 GetValueSpn(u32 spn, u8 *valid);
// int32_t Rec_J1939_CB(tCanMessage * msg);
u8 RunSpnTimeOutExpiried(u32 spn, int hdl_led) ;
// void ini_J1939_Hndl(void);
// u8 J1939FillKey(u8 NumKeyDwn);
// u8 GetSpnValid(u32 spn);
int parsePGN(tCanMessage* msg);
//const
extern t_j1939_spn SPNs[];
//const
extern u32 SzSPNArr;
// extern dumpSpn*  dmp;


#endif /* J1939_J1939_H_ */
