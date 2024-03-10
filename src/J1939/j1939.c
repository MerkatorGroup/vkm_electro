
/*
 * j1939.c
 *
 *  Created on: 31 марта 2023 г.
 *      Author: ASUS
 */

// #include "UserCAPI.h"
// #include "my_types.h"
#include "m_j1939.h"
// #include "gsMsgFiFo.h"
//
// #include "gsThread.h"
// #include "gsToVisu.h"
// #include "gsedebug.h"
#define SZ_SPN_ARR(x) (SzSPNArr)
u8 Ignition_J1939 = 0;
u32 TimerIgnitionON = 0;
static int parseSPN(t_j1939_spn* pSpn, u64* p_data);  //, float* CalcValue);
// s32 FifoRX_j1939;
// static tGsThread _Thread_J1939 = NULL;
// static int J1939_Ini_Thread(void);
// static void iniLinkToVISU(void);
// static int parsePGN(tCanMessage* msg);
//static void* J1939_Thread(void* arg);
//static u32 GetObj_J1939_val_ID(u32 num);
//void make_Dump(void);
extern u32 numThreads;
tGsMsgFiFo CAN_Simfifo;
u32 indxDump = 0;

///// fill dump ///
// void make_Dump(void) {
//     if (SzSPNArr == 0) return;
//     ///// fill dump ///
//     dmp = calloc(SzSPNArr, sizeof(dumpSpn));
//     if (dmp == NULL) return;
//     FifoRX_j1939 = CANCreateFiFo(312);
//     //	dmp[0].pspn = &SPNs[0];
//     dmp[0].Pgn = 0xFF000000;
//     //	dmp[0].LastSource = 0xFFFF;
//     indxDump = 0;
//     u32 nsp = 0;  //,Prevnsp = 0;
//     dumpSpn* CurrSpn = dmp;
//     while (nsp < SzSPNArr) {
//         //		if (CurrSpn->Pgn  != SPNs[nsp].PGN)
//         {
//             dmp[indxDump].pspn = &SPNs[nsp];
//             dmp[indxDump].Pgn = dmp[indxDump].pspn->PGN;
//             //			dmp[indxDump].LastSource = 0xFFFF;
//             //			dmp[indxDump - 1].n = nsp - Prevnsp;
//             // printf("PGN_%d - %04X   \r\n",dmp[indxSPNs].pspn->PGN
//             // ,dmp[indxSPNs].pspn->PGN  );
//             u32 tId = /*0x1C000000 | */ ((u32)SPNs[nsp].PGN << 8) |
//                       SPNs[nsp].ValidSource;
//             //			u32 tMsk = tId & 0x00FFFF00;
//             int rs = CANAddRxFilterFiFo(J1939_CAN_IF, tId, 0x00FFFFFF, 1,
//                                         FifoRX_j1939);
//             //			int rs= CANAddRxFilterFiFo( J1939_CAN_IF,tId
//             //,tMsk, 1, FifoRX_j1939 ); 			int rs= CANAddRxFilterCallBack (
//             //J1939_CAN_IF,tId ,tMsk, 1,Rec_J1939_CB);
// #if (IsSET(DBUG_SIMULATION, SIMUL_INTERNAL_J1939))
//             AddPGN_FILTR_Simulation(tId);
// #endif
//             CurrSpn = &dmp[indxDump];
//             indxDump++;
//             // Prevnsp = nsp;
//         }
//         CurrSpn->n++;
//         nsp++;
//     }
//     // dmp[indxDump - 1].n = nsp - Prevnsp;
// }
// void ini_J1939_Hndl(void) {
//     make_Dump();
//     for (int sp = 0; sp <= 35; sp++)
//         SendToVisuObj(GetObj_J1939_val_ID(sp), GS_TO_VISU_SET_FG_COLOR,
//                       0x00404040);
//     iniLinkToVISU();
//     ///// end fill dump ///
//     CAN_Simfifo = gsMsgFiFoCreate(512 * sizeof(tCanMessage));
//     // int r =
//     J1939_Ini_Thread();
// }
// /// ///////////////////////////////
// static void iniLinkToVISU(void) {
//     u32 N_Row = 0;
//     for (u32 i = 0; i < indxDump; i++) {
//         t_j1939_spn* pSpn = dmp[i].pspn;
//         for (int sp = 0; sp < dmp[i].n; sp++) {
//             SetVar(400 + N_Row, pSpn->varName);  // вставим название SPN
//             // SetVar(HDL_SPNNAME5,17); // вставим название SPN
//             pSpn->varValue = 300 + N_Row;  //
//             u32 idObj = GetObj_J1939_val_ID(N_Row);
//             pSpn->varObjId = idObj;
//             pSpn++;
//             N_Row++;
//         }
//     }
// }
// u8 J1939FillKey(u8 NumKeyDwn) {
//     u8 ret = NumKeyDwn;
//     switch (NumKeyDwn) {
//         case 7:
//             break;
//         case 8:
//             break;
//         case 9:
//             break;
//         case 10:
//             break;
//         case 11:
//             break;
//         case 12:
//             break;
//         case 105:
//             break;
//         case 104:
//             break;
//     }
//     return ret;
// }
// /// /////////////////////////////////////
// static int J1939_Ini_Thread(void) {
//     if (_Thread_J1939 == NULL) {
//         int th = gsThreadCreate(&_Thread_J1939, NULL, J1939_Thread, NULL);
//         if (th == 0) {
//             numThreads++;
//         } else {
//             db_out_time("Thread_J1939 - FAIL!");
//             return (-1);
//         }
//     }
//     return 0;
// }
// static void J1939_TimeOutValidSpn(void) {
//     for (u32 i = 0; i < indxDump; i++) {
//         t_j1939_spn* tspn = dmp[i].pspn;
//         if (dmp[i].TimeOutValid)
//             dmp[i].TimeOutValid--;
//         else {
//             tspn->IsValid = 0;
//             tspn->floatValue = 0;
//         }
//         if (tspn->IsValid == 2)
//             SendToVisuObj(tspn->varObjId, GS_TO_VISU_SET_FG_COLOR, 0x00FFFF40);
//         else if (tspn->IsValid == 1)
//             SendToVisuObj(tspn->varObjId, GS_TO_VISU_SET_FG_COLOR, 0x00A04040);
//         else
//             SendToVisuObj(tspn->varObjId, GS_TO_VISU_SET_FG_COLOR, 0x00004040);
//         SetVarFloat(tspn->varValue, tspn->floatValue);
//     }
// }
// static void* J1939_Thread(void* arg) {
//     tCanMessage* tMsg;
//     // u8 SzCanMsg = sizeof(tCanMessage);
//     u8 buff[128];
//     tMsg = (tCanMessage*)buff;
//     u32 TimerFillValid = 1000;
//     while (1) {
//         //    	if(CANReadFiFo(FifoRX_j1939,&tMsg,1) != 0){
//         u32 n = 0;
//         do {
// #if (IsSET(DBUG_SIMULATION, SIMUL_INTERNAL_J1939))
//             n = gsMsgFiFoRead(CAN_Simfifo, buff, SzCanMsg, 0);
//             // if(filtrFifo((tCanMessage *)buff)) continue;
// // #elif(IsSET(DBUG_SIMULATION,  SIMUL_ExTERNAL_J1939  ))
// //			n = CANReadFiFo(FifoRX_j1939,buff,1);
// #else
//             n = CANReadFiFo(FifoRX_j1939, tMsg, 1);
// #endif
//             if (n) parsePGN(tMsg);
//         } while (n > 0);
//         if (TimerFillValid)
//             TimerFillValid--;
//         else {
//             J1939_TimeOutValidSpn();
//             TimerFillValid = 100;
//         }
//         if (TimerIgnitionON) {
//             TimerIgnitionON--;
//             if (TimerIgnitionON < 3) {
//                 Ignition_J1939 = 0;
//                 TimerIgnitionON = 0;
//             }
//         }
//         usleep(1000);  // поток вызывается 1 / 10ms - 0.1kHz
//     }
//     return NULL;
// }

////////////////////////////////////////

 int parsePGN(tCanMessage* msg) {
    u16 CurrPGN = GET_PGN(msg->id);
    u8 CurrSourse = GET_SOURCE(msg->id);
    // if (CurrSourse == 0x00) {
    //     Ignition_J1939 = 1;
    //     TimerIgnitionON = 200;
    // }
    u32 i = 0;
    int result = 0;
    for (i = 0; i < SzSPNArr; i++) {
        if (SPNs[i].PGN == CurrPGN) {
            t_j1939_spn* tspn = &SPNs[i];
            // if (tspn->ValidSource == CurrSourse) {
                // db_out_time("PGN_%d - %04X  Cnt %d \r\n", SPNs[i].PGN,
                            // SPNs[i].Pgn, SPNs[i].n);
                // for (int s = 0; s < SzSPNArr; s++, tspn++) {
                    //				float Calc;
                    result = parseSPN(tspn, (u64*)msg->data.u8);  //, &Calc);
                    if (result != 0) {
                        SPNs[i].TimeOutValid = 30;
                        // db_out_time("  -value SPN_%d - %s\r\n", tspn->SPN,
                        //             "n/a");
                    }
                    //  else if (result == 2) {
                    //     // if(CurrSourse < SPNs[i].LastSource)
                    //     // {
                    //         // tempF[s] = Calc;SPNs[i].TimeOutValid = 10;
                    //         //							s32
                    //         //t = (s32) tspn->floatValue; 							SetVar(tspn->varValue,
                    //         //t); 							SetVarFloat(tspn->varValue, tspn->floatValue);
                    //         SPNs[i].TimeOutValid = 10;  // 1 sec
                    //     // }
                    //     // db_out_time("  -value SPN_%d - %.3f\r\n", tspn->SPN,
                    //     //             tspn->floatValue);
                    // }
                    tspn->IsValid =  result;
                // }
            // }
        }
    }
    //	for (int i = 0; i < GS_ARRAYELEMENTS(SPNs); i++) {
    //		u16 tPgn = (u16)((msg->id) >> 8);
    //		if (SPNs[i].PGN == tPgn) {
    //			t_j1939_spn* pSpn = &SPNs[i];
    //			parseSPN(pSpn,(u64*)msg->data.u8 );
    //
    //		}
    //	}
    return result ;
}

int parseSPN(t_j1939_spn* pSpn, u64* p_data) {  //, float* CalcValue) {
    u64 data = *p_data;
    u8 byte = (pSpn->bByte) - 1;
    u8 bit = (pSpn->bPos) - 1;
    u8 start = (byte << 3) | bit;
    u32 mask1 = 0;  //(1 << 22) ;
    u32 mask2 = 0;  // mask1 << 32;
    int i = start, end = start + (pSpn->bSize);
    for (; i < end; i++) {
        u32 tmp;
        if (i < 32) {
            tmp = (1 << i);
            mask2 |= tmp;
        } else {
            tmp = (1 << (i % 32));
            mask1 |= tmp;
        }
    }
    u64 mask = ((u64)mask1 << 32) | (u64)mask2;
    u64 rawB = (data & mask) >> start;
    float Val = rawB * pSpn->resolution + pSpn->RangeMinMax[0];
    //	if ((Val > pSpn->RangeMinMax[1]) || (Val < pSpn->RangeMinMax[0]))
    if (IsInRange(Val, pSpn->RangeMinMax[0], pSpn->RangeMinMax[1])) {
        pSpn->floatValue = Val;
        return 1;
    }
    pSpn->floatValue = 0;
    return 2;
}
//**************************************

int32_t Rec_J1939_CB(tCanMessage* msg) { return parsePGN(msg); }

s32 GetValueSpn(u32 spn, u8 *valid) {
    u32 nsp = 0;
    t_j1939_spn* pSpn;
    while (nsp < SzSPNArr) {
        pSpn = &SPNs[nsp];
        if (pSpn->SPN == spn) {
            s32 t = (s32)pSpn->floatValue ;  //
            *valid = pSpn ->IsValid;
            return t;
        }
        nsp++;
    }
    return -1;
}
float GetfValueSpn(u32 spn, u8 *valid) {
    u32 nsp = 0;
    t_j1939_spn* pSpn;
    while (nsp < SzSPNArr) {
        pSpn = &SPNs[nsp];
        if (pSpn->SPN == spn) {
            *valid = pSpn ->IsValid;
            return pSpn->floatValue;
        }
        nsp++;
    }
    return NOT_VALID;
}
u8 RunSpnTimeOutExpiried(u32 spn, int hdl_led) {
    u32 nsp = 0;
    t_j1939_spn* pSpn;
    while (nsp < SzSPNArr) {
        pSpn = &SPNs[nsp];
        if (pSpn->SPN == spn) {
            if(pSpn->TimeOutValid){
                if(hdl_led)
                    SetVar(hdl_led, pSpn->IsValid);
                pSpn->TimeOutValid--;
            }
            else{
                if(hdl_led)SetVar(hdl_led, 0);
            }
            return 1;
        }
        nsp++;
    }
    return 0;
}

// u8 GetSpnValid(u32 spn) {
//     u32 nsp = 0;
//     t_j1939_spn* pSpn;
//     while (nsp < SzSPNArr) {
//         pSpn = &SPNs[nsp];
//         if (pSpn->SPN == spn) {
//             return (pSpn->IsValid == 2);
//         }
//         nsp++;
//     }
//     return 0;
// }

// // ***************************************
// static u32 GetObj_J1939_val_ID(u32 num) {
//     switch (num) {
//         case 0:
//             return OBJ_VALUE_0;
//         case 1:
//             return OBJ_VALUE_1;
//         case 2:
//             return OBJ_VALUE_2;
//         case 3:
//             return OBJ_VALUE_3;
//         case 4:
//             return OBJ_VALUE_4;
//         case 5:
//             return OBJ_VALUE_5;
//         case 6:
//             return OBJ_VALUE_6;
//         case 7:
//             return OBJ_VALUE_7;
//         case 8:
//             return OBJ_VALUE_8;
//         case 9:
//             return OBJ_VALUE_9;
//         case 10:
//             return OBJ_VALUE_10;
//         case 11:
//             return OBJ_VALUE_11;
//         case 12:
//             return OBJ_VALUE_12;
//         case 13:
//             return OBJ_VALUE_13;
//         case 14:
//             return OBJ_VALUE_14;
//         case 15:
//             return OBJ_VALUE_15;
//         case 16:
//             return OBJ_VALUE_16;
//         case 17:
//             return OBJ_VALUE_17;
//         case 18:
//             return OBJ_VALUE_18;
//         case 19:
//             return OBJ_VALUE_19;
//         case 20:
//             return OBJ_VALUE_20;
//         case 21:
//             return OBJ_VALUE_21;
//         case 22:
//             return OBJ_VALUE_22;
//         case 23:
//             return OBJ_VALUE_23;
//         case 24:
//             return OBJ_VALUE_24;
//         case 25:
//             return OBJ_VALUE_25;
//         case 26:
//             return OBJ_VALUE_26;
//         case 27:
//             return OBJ_VALUE_27;
//         case 28:
//             return OBJ_VALUE_28;
//         case 29:
//             return OBJ_VALUE_29;
//         case 30:
//             return OBJ_VALUE_30;
//         case 31:
//             return OBJ_VALUE_31;
//         case 32:
//             return OBJ_VALUE_32;
//         case 33:
//             return OBJ_VALUE_33;
//         case 34:
//             return OBJ_VALUE_34;
//         case 35:
//             return OBJ_VALUE_35;
//     }
//     return 0;
// }
