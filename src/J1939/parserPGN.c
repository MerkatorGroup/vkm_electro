
/*
 * tmp.c
 *
 *  Created on: 21 апр. 2023 г.
 *      Author: ASUS
 *      Парсинг CAN сообщения J1939
 *
 */

#include "main_px_bat.h"
// mat_dens = (rgnt_ -> MaterialDensity - 1000) / 100;
typedef struct _t_j1939_spn {
	u32 SPN;	//< имя spn
	u32 PGN;	//< Код группы
	u8 bSize;	//< Размер в битах
	float resolution;  //< дискретность
	s32 RangeMinMax[2];
	u8 bByte;	//< N байта 1..8
	u8 bPos;	//< Позиция в битах 1..8 *
	float LastValue;

} t_j1939_spn;

 t_j1939_spn SPNs[2]={
		{190, 61444,16,0.125,{0,8031},4,1,0.0}, 	//< обороты двигателя PGN F004 об/мин
		{168, 65271,16,0.05,{0,3212},5,1,0.0}  //< Потенциал батареи PGN FEF7 вольты

 };
//typedef struct tagCanMessage {
//    uint32_t id;                //!< Id of the message
//    union {
//        uint32_t u32[2];
//        uint8_t  u8[8];
//    }data;
//}tCanMessage;

int parseSPN(const t_j1939_spn* pSpn, u64* p_data, float* CalcValue) {
	u64 data = *p_data;
	u8 byte = (pSpn->bByte) - 1;
	u8 bit = (pSpn->bPos) - 1;
	u8 start = (byte << 3) | bit;
	u32 mask1 = 0;   //(1 << 22) ;
	u32 mask2 = 0; // mask1 << 32;
	int i = start, end = start + (pSpn->bSize);
	for (; i < end; i++) {
		u32 tmp;
		if (i < 32){
			 tmp = (1<< i);
			mask2 |= tmp;
		}
		else{
			tmp = (1<< (i % 32));
			mask1 |= tmp;
		}
	}
	u64 mask = ((u64)mask1 << 32) | (u64)mask2;
	u64 rawB = (data & mask) >> start;
 //printf("mask SPN_%d - 0x%08X%08X ; Shift %d\r\n",pSpn->SPN, mask1,mask2, start  );

 ///////////

	float Val = rawB * pSpn->resolution + pSpn->RangeMinMax[0];
	if ((Val > pSpn->RangeMinMax[1]) || (Val < pSpn->RangeMinMax[0])) {
		*CalcValue = NaN;
		return -1;
	}
	*CalcValue =   Val;
	return 0;
}


#define GET_PGN(x) ((x & 0xFFFF00) >> 8)

int parsePGN(tCanMessage*msg) {

	u16 CurrPGN = GET_PGN(msg->id);
	int i = 0;

	for (i = 0; i < 2; i++) {
		if (SPNs[i].PGN == CurrPGN) {
			t_j1939_spn* tspn = &SPNs[i];
			float Calc;
			int result = parseSPN(tspn, (u64*) msg->data.u8, &Calc);
			if (result == 0) {
				tspn->LastValue = Calc;
			} else
				tspn->LastValue = NaN;
		}
	}
	return -1;
}


//**************************************
