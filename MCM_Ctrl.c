/****************************************************************************
 *
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
static tCanMessage buff[16];
tGsThread id_MCMThread;
void *Thread_MCM(void *arg);
int parseMCM(tCanMessage *msg);
void MCM_request_DOut(void);
static s32 InitSlave_Device( sett_SLAVE_Device * currSlaveDev) ;
extern void Cfg_MCM_IOs(inpOutp_tdf* targetIOs);
void setCollantPump_50(void);

u8 MutexTX = 0;
s32 FifoRX_MCM;
u32 MCM_DOut = 0;
tMr_SLV_DEV SlvMCMs[4];
tMr_SLV_DEV * _pSlvMCMs = &SlvMCMs;
extern  inpOutp_tdf MCM_0 [];
extern  inpOutp_tdf MCM_1 [];
extern  inpOutp_tdf MCM_2 [];
extern  inpOutp_tdf MCM_3 [];
extern   sett_SLAVE_Device SlaveDevs_sett[];


#define PWM_INTERVAL 100
// static u32 MaskPWM = 0;

//**************************************
int Ini_MCM_Thread(void) {
    FifoRX_MCM = CANCreateFiFo(64);
    int rs = CANAddRxFilterFiFo(MCM_CAN_ch, 0x020, 0x7F1, 0, FifoRX_MCM);

    SetVar(HDL_IGNITION_IS_ON, 0);


	for( u32 i = 0; i < 4; i++ ) {
  sett_SLAVE_Device * currMCMsett = &SlaveDevs_sett[i];
  if (InitSlave_Device(currMCMsett)== (-1))
      return 1;
	}


    int th = gsThreadCreate(&id_MCMThread, NULL, Thread_MCM, NULL);
    if (th != 0) {
        // db_out_time("ini Thread_MCM - FAIL!");
        return (-1);
    }
    return 0;
}

void *Thread_MCM(void *arg) {
    u32 Timer_ReqDout = 5;  // x20ms
    u32 Timer_ReqPWMout = 6;  // x20ms
    while (1) {
        //~~~~~~~~~~~~~~
        tCanMessage *tMsg = (tCanMessage *)&buff[1];
        u32 n = CANReadFiFo(FifoRX_MCM, tMsg, 1);
        if (n) parseMCM(tMsg);
            //////simulation/////
#ifdef SIMULATION
        extern tCanMessage *sim_msg;
        if (gsThreadMutexTrylock(&mtx) != 0) {
            if ((sim_msg != NULL) && (sim_msg->id == 0x22)) {
                parseMCM(sim_msg);
                sim_msg = NULL;
            }
            gsThreadMutexUnlock(&mtx);
        }
#endif

        if (Timer_ReqDout) Timer_ReqDout--;  // 100ms
        else {
            // MCM_request_DOut();
            static u32 imcm = 0;
            tMr_SLV_DEV *tMCM = &_pSlvMCMs[imcm];
            imcm++;
            FillOnCircL(imcm, 0, 3);
            MR_MCM_SendDout(tMCM);
            Timer_ReqDout = 2;
        }

        if (Timer_ReqPWMout)
            Timer_ReqPWMout--;  // 100ms
        else {
            // setCollantPump_50();
            static u32 pwmMcm = 0;
            tMr_SLV_DEV *tMCM = &_pSlvMCMs[pwmMcm];
            MR_RequestPwmOut(tMCM);
            pwmMcm++;
            FillOnCircL(pwmMcm, 0, 3);
            Timer_ReqPWMout = 3;
        }
        usleep(5000);  /// 5 ms
    }
    return NULL;
}

int parseMCM(tCanMessage *msg) {
    if ((0x03 == msg->data.u8[0])) {
        u16 CurrMCM = msg->id;
        u16 CurrHNDL = msg->data.u8[1] + (u16)(msg->data.u8[2] << 8);
        int32_t Data = msg->data.u32[1];
        if (CurrHNDL == 2000) {
            if (CurrMCM == 0x22) {// MCM2
                // u8 ti = ((Data & 0x10) == 0x10);
                // SetVar(HDL_KEY_POSITION, ti);
                u8 ti = Data & 0x11;
                SetVar(HDL_KEY_POSITION , ti==0x10?1:ti==0x11?2:0);

                ti = Data & 0x02;
                SetVar(HDL_TURTLE , ti != 0);

                ti = Data & 0x08;
                SetVar(HDL_BRAKE_PRESSURE , ti==0x08?0:1);
                ti = Data & 0x04;
                SetVar(HDL_BRAKE_PEDAL , ti==0x04?1:0);
            }

            if (CurrMCM == 0x24) { // MCM1
                u8 ti = Data & 0x02;
                SetVar(HDL_LOW_LEV_HYD , ti == 0);

                ti = Data & 0x04;
                SetVar(HDL_OVER_TERM_HYD , ti == 0);

                ti = Data & 0x08;
                SetVar(HDL_RAMP_OPEN , ti == 0);
            }

            if (CurrMCM == 0x26) { // MCM2
                u8 ti = Data & 0x01;
                SetVar(HDL_BOX_IS_UP , ti == 1);
                // SendToVisuObj(OBJ_BOXUP , GS_TO_VISU_SET_ATTR_BLINK, ti);
                ti = Data & 0x02;
                SetVar(HDL_COLANT_LOW, ti != 0);

                ti = Data & 0x0C;
                extBut_BOXUP_CB(ti);


            }
        }
    }
    return 0;
}


/////////////// DOut ////////////////
/**
@brief установка значения
      по имени выхода
*/
 int MR_SetDO_byName(char* nameIO_do , u8 Val ){
//Add All Modules jf Slaves devices
	for( u32 imcm = 0; imcm < 4; imcm++ ) {
    tMr_SLV_DEV* thisMCM  = &_pSlvMCMs[imcm];
    inpOutp_tdf* curIo= thisMCM->MCM.MCM250.pIOs;
    for(int i = 0; i<18; i++){

      if((curIo->typeIo == DOut)&&((IsInRange(curIo->PinNum ,3,10))||
                (IsInRange( curIo->PinNum ,28,29)))){

        if(!(strcmp(nameIO_do ,curIo ->Name))){
          u32 tOut = //thisMCM->MCM.MCM250.digOut;
                  GetVar(thisMCM->MCM.MCM250.Var_digOut);
//          u32 t = SetVar((curIo-> cast.DO.GlVar),Val );
          curIo-> cast.DO.DO_Value = Val;
          if(Val)tOut |= ((u32)(1 << (curIo-> cast.DO.shiftBit)));
          else tOut &= ~((u32)(1 << (curIo-> cast.DO.shiftBit)));
          thisMCM ->MCM.MCM250.digOut = tOut;
          SetVar(thisMCM->MCM.MCM250.Var_digOut, tOut);
          return 0;
        }
      }
    curIo++;
    }
    //}
 }
  return -1;
}
/**
@brief установка значения в переменную digOut
       соответствующего MCM
*/
 int MR_SetDO(tMr_SLV_DEV * MCM, u8 nPin, u8 Val ){
  inpOutp_tdf* curIo = MCM->MCM.MCM250.pIOs;
  for(int i = 0  ; i < 18; i++){
    if((curIo->typeIo == DOut)&&(curIo->PinNum == nPin)){
      Val &= 1;
      u32 tOut = //MCM->MCM.MCM250.digOut;
                  GetVar(MCM->MCM.MCM250.Var_digOut);
//      u32 t = SetVar((curIo-> cast.DO.GlVar),Val );
      curIo-> cast.DO.DO_Value = Val;
      if(Val)tOut |= ((u32)(1 << (curIo-> cast.DO.shiftBit)));
      else tOut &= ~((u32)(1 << (curIo-> cast.DO.shiftBit)));
      MCM->MCM.MCM250.digOut = tOut;
      SetVar(MCM->MCM.MCM250.Var_digOut, tOut);
		return 0;
    }
    curIo++;
  }
  return -1;
}
/**
* @brief  отправка значения digOut
       соответствующего MCM  в регистр 2008
*/
int MR_MCM_SendDout(tMr_SLV_DEV *MCM) {
    //   MCM->MCM.MCM250.DO_cntMs_ival = MCM->MCM.MCM250.DO_Interval;
    u32 DO = //MCM->MCM.MCM250.digOut;
            GetVar(MCM->MCM.MCM250.Var_digOut);
    DO &= ~(MCM->MCM.MCM250.MaskPWM);

    tCanMessage tx_msg;
    tx_msg.channel = MCM->CAN;
    tx_msg.ext = MCM->Ext;
    tx_msg.id = MCM->RX_Id;
    tx_msg.len = 8;
    tx_msg.res = 0;
    tx_msg.data.u8[0] = 0x02;
    tx_msg.data.u8[1] = 0xD8;
    tx_msg.data.u8[2] = 0x07;
    tx_msg.data.u8[3] = 0;
    tx_msg.data.u32[1] = DO;
    CANSendMsg(&tx_msg);

    return 0;
}

// void SetDOut(u8 NumDout, u8 val) {
//     while (MutexTX) {
//     };
//     if (val)
//         MCM_DOut |= (1 << NumDout);
//     else
//         MCM_DOut &= ~(1 << NumDout);
// }

// void MCM_request_DOut(void) {
//     MutexTX = 1;
//     tCanMessage tx_msg;
//     tx_msg.channel = MCM_CAN_ch;
//     tx_msg.ext = MCM_ADR_EXT;
//     tx_msg.id = TX_to_MCM_CAN_ADR;
//     tx_msg.len = 8;
//     tx_msg.res = 0;
//     tx_msg.data.u8[0] = 2;
//     tx_msg.data.u8[1] = 0xD8;
//     tx_msg.data.u8[2] = 0x07;
//     tx_msg.data.u8[3] = 0;          //
//     tx_msg.data.u32[1] = MCM_DOut;  //
//     CANSendMsg(&tx_msg);
//     MutexTX = 0;
// }

static u32 _numMCM = 0;

static s32 InitSlave_Device( sett_SLAVE_Device * currSlaveDev) {
s32 idx = _numMCM;
	tMr_SLV_DEV* thisMCM  = &_pSlvMCMs[idx];
	thisMCM->RX_Id = currSlaveDev->Cfg.RX_ID;//  GSeMCM_RX;
	thisMCM->TX_Id = currSlaveDev->Cfg.TX_ID[0]; //  GSeMCM_TX;
	thisMCM->Ext = currSlaveDev->Cfg.Ext;
	thisMCM->CAN = currSlaveDev->Cfg.CAN;
	//thisMCM->Last_Msg_ms = GetMSTick() + 10000;
	thisMCM->Type = currSlaveDev->type_MCM;
  thisMCM->DeviceName = currSlaveDev->Cfg.name;

  thisMCM->MCM.MCM250.DO_Interval = 200;
  /// рассинхронизация отправляемых сообщений DOUT
  static u32 cntMs_ival_tmp = 5;
  thisMCM->MCM.MCM250.DO_cntMs_ival=cntMs_ival_tmp;
  cntMs_ival_tmp += 5;

		thisMCM->MCM.MCM250.digOut  = 0;
		thisMCM->polling_ms = 2000;
		thisMCM->IDnumMCM =  currSlaveDev->N_MCM;
                switch (currSlaveDev->N_MCM) {
                    case 0:
                        thisMCM->MCM.MCM250.pIOs = MCM_0;
                        thisMCM->MCM.MCM250.Var_digOut = HDL_DIG_OUT_0;
                        break;
                    case 1:
                        thisMCM->MCM.MCM250.pIOs = MCM_1;
                        thisMCM->MCM.MCM250.Var_digOut = HDL_DIG_OUT_1;
                        break;
                    case 2:
                        thisMCM->MCM.MCM250.pIOs = MCM_2;
                        thisMCM->MCM.MCM250.Var_digOut = HDL_DIG_OUT_2;
                        break;
                    case 3:
                        thisMCM->MCM.MCM250.pIOs = MCM_3;
                        thisMCM->MCM.MCM250.Var_digOut = HDL_DIG_OUT_3;
                        break;
                }
     SetVar(thisMCM->MCM.MCM250.Var_digOut, 0);
     Cfg_MCM_IOs(thisMCM ->MCM.MCM250.pIOs);
     makePwmArray(thisMCM );

	// CANAddRxFilterCallBack( thisMCM->CAN, thisMCM->TX_Id, 0x7FF, thisMCM->Ext, MCM250_Callback );

  _numMCM++;					//Index
	return idx;
}

////////// PWM_Out /////////////////////
/**
 @brief  заполнение списка ШИМ выходов
          если номер выхода - PWM Out - заполняется, иначе - 0
*/
void makePwmArray(tMr_SLV_DEV * MCM ){
//  memset(&MCM->MCM.MCM250.pPWM_io, 0,sizeof(MCM.MCM250.MCM.pPWM_io));
  for(int i = 0; i < 4; i++)MCM->MCM.MCM250.pPWM_io[i] = 0;

  inpOutp_tdf* curIo = MCM->MCM.MCM250.pIOs;
  for(int i = 0; i < 18; i++){

    if(curIo->typeIo == PWMout){
     // curIo->cast.Pwm.PWMout_Interval = GetMSTick()+PWM_INTERVAL;
      if(curIo->PinNum == 5){ MCM->MCM.MCM250.pPWM_io[0] = curIo; MCM->MCM.MCM250.MaskPWM = 2;}
      else if(curIo->PinNum == 6){ MCM->MCM.MCM250.pPWM_io[1] = curIo; MCM->MCM.MCM250.MaskPWM |= 8;}
      else if(curIo->PinNum == 7){ MCM->MCM.MCM250.pPWM_io[2] = curIo;MCM->MCM.MCM250.MaskPWM |= 32;}
      else if(curIo->PinNum == 8){ MCM->MCM.MCM250.pPWM_io[3] = curIo; MCM->MCM.MCM250.MaskPWM |= 128;};
    }
    curIo++;
  }
}
/**
@brief установка значения частоты и скважности
      по имени ШИМ выхода
*/
int MR_MCM_SetPWMOut_by_Name(char* nameIOpwm, u16 Frq, u16 Duty) {
//Add All Modules jf Slaves devices
	for (u32 imcm = 0; imcm < 4; imcm++) {
		tMr_SLV_DEV* thisMCM = &_pSlvMCMs[imcm];
//    if(!(strcmp(nameMCM,thisMCM->DeviceName))
//    {
		for (int i = 0; i < 4; i++) {
			inpOutp_tdf* pIO_pwm = thisMCM->MCM.MCM250.pPWM_io[i];
			if (pIO_pwm != NULL) {
				if (!(strcmp(nameIOpwm, pIO_pwm->Name))) {
					pIO_pwm->cast.Pwm.FreqValue = Frq;
					pIO_pwm->cast.Pwm.DutyValue = Duty;
					return 0; // MR_MCM_SetPWMOut(thisMCM  ,i, Frq, Duty ) ;
				}
			}
		}
		//}
	}
	return -1;
}


/**
 @brief  устанавливает в глобальные переменные значения Freq и Duty
 @param  MCM - MCM250
 @param  num - номер PWM выхода
 @return
*/
int MR_MCM_SetPWMOut( tMr_SLV_DEV* MCM, u8 num, u16 Frq, u16 Duty ) {
   inpOutp_tdf* pIO_pwm = MCM->MCM.MCM250.pPWM_io[num];
   if(pIO_pwm != NULL){
//      uint16_t PWM_NUM = pIO_pwm->Hndle - PWMout ;
//      if(IsInRange(PWM_NUM,0,3)){
    	  pIO_pwm->cast.Pwm.FreqValue= Frq;
    	  pIO_pwm->cast.Pwm.DutyValue= Duty;
        return 0;
//      }
    }
return -1;
}


void MR_RequestPwmOut(tMr_SLV_DEV *MCM) {
//  static u8 currNum =0;

	for (int currNum = 0; currNum < 4; ++currNum) {
		inpOutp_tdf *pCurrIo = MCM->MCM.MCM250.pPWM_io[currNum];

		if (pCurrIo != 0) {
			t_IO_PWM *currPWMout = &pCurrIo->cast.Pwm;
			s16 PWM_NUM = pCurrIo->Hndle - PWMout;
			if (IsInRange(PWM_NUM, 0, 3)) {
				u16 duty = currPWMout->DutyValue;
				u16 Frq = currPWMout->FreqValue;

				tCanMessage tx_msg;
				tx_msg.channel = MCM->CAN;
				tx_msg.ext = MCM->Ext;
				tx_msg.id = MCM->RX_Id;
				tx_msg.len = 8;
				tx_msg.res = 0;
				tx_msg.data.u8[0] = 0x15;
				tx_msg.data.u8[1] = 0x21;
				tx_msg.data.u8[2] = 0;
				tx_msg.data.u8[3] = 0xFF & (PWM_NUM);
				tx_msg.data.u16[2] = Frq & 0xFFFF;
				tx_msg.data.u16[3] = duty & 0xFFFF;
				CANSendMsg(&tx_msg);
			}
			usleep(20000); //
		}
	}
}

// void setCollantPump_50(void) {
//     inpOutp_tdf *pCurrIo = &MCM_2[4];  //          MCM.MCM250.pPWM_io[currNum];
//     u16 duty = 500;
//     u16 Frq = 400;

//     tCanMessage tx_msg;
//     tx_msg.channel = MCM_CAN_ch;
//     tx_msg.ext = 0;
//     tx_msg.id = 0x25;
//     tx_msg.len = 8;
//     tx_msg.res = 0;
//     tx_msg.data.u8[0] = 0x15;
//     tx_msg.data.u8[1] = 0x21;
//     tx_msg.data.u8[2] = 0;
//     tx_msg.data.u8[3] = 0xFF & (2);
//     tx_msg.data.u16[2] = Frq & 0xFFFF;
//     tx_msg.data.u16[3] = duty & 0xFFFF;
//     CANSendMsg(&tx_msg);
// }


 void parseDIN(inpOutp_tdf* currPin, u32 data){
    u8 shft = currPin->cast.DI.shiftBit;
    currPin->cast.DI.DI_Value =  (data>>shft) & 1;
}


