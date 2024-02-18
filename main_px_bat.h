
#ifndef USERCAPI_PROTOTYPES
#define USERCAPI_PROTOTYPES
#endif
#ifndef SIMULATION
// #define SIMULATION
#endif


#include <stdint.h>
#include <UserCEvents.h> /* Events send to the Cycle function              */
#include <UserCAPI.h>	 /* API-Function declarations                      */

#include "vartab.h" /* Variable Table definitions:                    */

#include "objtab.h" /* Object ID definitions:  */

#include "my_types.h"
#include "gsThread.h"
#include "gsMsgFiFo.h"
#include "src\VechleIcons.h"
#include "src\VertBar.h"
#include "simul.h"



#define ShassieCAN_ch 1
#define MCM_CAN_ch 0
#define TX_to_MCM_CAN_ADR 0x21
#define MCM_ADR_EXT 0

#define BAT_LOW_LEVEL 300
#define INV_LOW_LEVEL 300
#define INV_LOW_LEVELWARN (INV_LOW_LEVEL*8/10)
#define WDT_TIME 50

extern int Ini_AKB_Thread(void) ;
extern int Ini_EngInv_Thread(void);
extern int Ini_MCM_Thread(void);
extern int Ini_Thread_MAIN(void);
extern 	tCanMessage buff[16];
extern 	void approx(int HDL, int val);
extern 	void SetDOut(u8 NumDout, u8 val);
extern tGsMutex mtx;


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
