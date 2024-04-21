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

void SetPWM_5PIN(int32_t Direct);
void chekPowSuplayPin(void);
tGsMutex mtx;
// u8 WDT_LEDS[32];
/****************************************************************************
 * UserC initialisation
 ****************************************************************************/
int UserCInit(uint32_t initFlags) {

    vechleicons_show();
    int r = Ini_Thread_MAIN();
#ifdef SIMULATION
    r = Ini_SIMULATION_Thread();
#endif
    MaskOn(MSK_WORKSITE);//MSK_DASHBOARD
    return 10;
}

/****************************************************************************
 * UserC cyclic part
 *
 * @param  evtc Number of events available (Event count)
 * @param  evtv Pointer to event value structures
 *              (see UserC-MCQ_de.chm im Hilfe-Menь)
 ****************************************************************************/
void UserCCycle(uint32_t evtc, tUserCEvt *evtv) {
    for (uint32_t i = 0; i < evtc; i++) {
        if (CEVT_KEY == evtv[i].Type) {
            tCEvtKey *KeyCntnt = &evtv[i].Content.mKey;
            if (KeyCntnt->State == 1) {
                u32 Numkey = KeyCntnt->KeyCode;
                switch (Numkey) {
                    if(IsMaskOn(MSK_MASK1) == 1){
                        case 103:
                        	SetPWM_5PIN(1);
                        	break;
                        case 104:
                        	SetPWM_5PIN(-1);
                        	break;
                    }

                    case 1:
                        // if(GetVar(HDL_IS_OFF)){
                        //  SendRequest_BAT_ON_OFF (1);
                        //  SetVar(HDL_IS_OFF,0);
                        // }
                        MaskOn(MSK_DASHBOARD);
                        MaskOff(MSK_MASK_PARAM);
                        MaskOff(MSK_WORKSITE);
                        break;
                    case 2:
                        // if(GetVar(HDL_IS_OFF)==0){
                        // 	SendRequest_BAT_ON_OFF(0);
                        //  	SetVar(HDL_IS_OFF,1);
                        // }
                        MaskOn(MSK_MASK_PARAM);
                        MaskOff(MSK_DASHBOARD);
                        MaskOff(MSK_WORKSITE);
                        break;
                    case 3:
                        MaskOff(MSK_MASK_PARAM);
                        MaskOff(MSK_DASHBOARD);
                        MaskOn(MSK_WORKSITE);
                        break;
                    case 4:
                        break;

                    case 5:
                      if(IsMaskOn(MSK_WORKSITE) == 1){
                        cover_set_up_dwn();
                      }
                        break;

                    case 7:
                        break;
                    case 9:
                        break;
                }
            }
        }
    }
}

// void chekPowSuplayPin(void) {
    // u8 dout = GetVar(HDL_POWERSUPLAYPIN);
//     if (dout != 9) {
//         SendToVisuObj(29, GS_TO_VISU_SET_ATTR_TRANSPARENT, 0);
//         SendToVisuObj(10, GS_TO_VISU_SET_ATTR_TRANSPARENT, 1);
//         dout = 9;
//     } else {
//         SendToVisuObj(29, GS_TO_VISU_SET_ATTR_TRANSPARENT, 1);
//         SendToVisuObj(10, GS_TO_VISU_SET_ATTR_TRANSPARENT, 0);
//         dout = 6;
//     }
//     SetVar(HDL_POWERSUPLAYPIN, dout);
// }

void SetPWM_5PIN(int32_t Direct) {
    // u16 t_duty = GetVar(HDL_DUTY_5P);
    // if((t_duty > 9)&&(Direct == (-1))){
    //         t_duty -= 10;
    // }else if((t_duty < 991)&&(Direct == 1)){
    //         t_duty += 10;
    // }
    // SetVar(HDL_DUTY_5P,t_duty);
    if (GetVar(HDL_INVERTER_ON_HYD))
    {
        s8 prcnt = Inv_Hydr.TorqPercent;
        prcnt += Direct;
        if ((prcnt > -1) && (prcnt < 101)) {
            Inv_Hydr.TorqPercent = prcnt;
        }
        SetVar(HDL_DUTY_5P,Inv_Hydr.TorqPercent);
    }
}
/* void SendPWM_Duty_5PIN(void){

        u16 t_duty = GetVar(HDL_DUTY_5P);

        tCanMessage tx_msg;
        tx_msg.channel = MCM_CAN_ch;
        tx_msg.ext = 0;
        tx_msg.id = 0x21;
        tx_msg.len = 8;
        tx_msg.res = 0;
        tx_msg.data.u8[0] = 0x15;
        tx_msg.data.u8[1] = 0x21;
        tx_msg.data.u8[2] = 0;
        tx_msg.data.u8[3] = 0;  // 5 pin
        tx_msg.data.u16[2] = ((u16)200) & 0xFFFF; //Hz
        tx_msg.data.u16[3] = t_duty & 0xFFFF;
        CANSendMsg(&tx_msg);
}
 */
/****************************************************************************
 * UserC timer process
 ****************************************************************************/

void UserCTimer(void) {
    /* for (u8 i = 0; i < 7; i++) {
         if (WDT_LEDS[i])
             WDT_LEDS[i]--;
         else
             SetVar(HDL_AVDISPOW_LED_1 + i, 0);
     }*/

    //   if(CAN_Send_Time_Duty)CAN_Send_Time_Duty--;
    //   else{
    //     SendPWM_Duty_5PIN();
    //     CAN_Send_Time_Duty = 10;
    //   }
    show_Y11_STAT();
}
