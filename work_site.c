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

#define GET_PGN(x) ((x & 0xFFFF00) >> 8)
#define GET_SRCADR(x) (x & 0xFF)
#define TIME_HYD_WORK 3000000
#define COMON_DELAY 50000

static void SendRequest_Turbo(u8 OnOff, u16 RPM);
static void trunk_shaft_cycl(void);
static int parsePGN_WorkCtrl(tCanMessage* msg);
static void Turb_cycl(void);
static void bunker_cycl(void);
static void cover_cycl(void);
static void wetting_cycl(void);
static void fr_br_up_dwn_cycl(void) ;
static void fr_br_tilt_cycl(void) ;
static void fr_br_fold_cycl(void);

static void side_brush_fold_cycl(void);
// static void sideBrush_cycl(void) ;
// static void main_Brush_cycl(void) ;
static void* Thread_EqWork(void* arg);
static void* Thread_Turbo_Send(void* arg);
static void Brush_Adjst_cycl(encoder_butt * enc_but);


butt TurbSpeedEnc = {.stat.raw = 0, .val = 0};
butt FrntBrshSpeedEnc = {.stat.raw = 0, .val = 0};
butt SideBrshSpeedEnc = {.stat.raw = 0, .val = 0};
butt trunk_shaft_butt = {.stat.raw = 0, .val = 0};  // шахты,  боковых щеток
butt bunker_up_dwn_butt = {.stat.raw = 0, .val = 0};
butt side_brush_fold_butt = {.stat.raw = 0, .val = 0};//  боковых щеток
butt wetting_butt = {.stat.raw = 0, .val = 0};//  увлажнение
butt Frnt_br_butt_tilt = {.stat.raw = 0, .val = 0};//  пер щетка наклон
butt Frnt_br_butt_up_dwn = {.stat.raw = 0, .val = 0};//  пер щетка опускание
butt Frnt_br_butt_fold = {.stat.raw = 0, .val = 0};//  пер щетка раскл


encoder_butt FrntBrshEncBut = {
  .namePWM_IO = "front_brush_spd",
  .pEnc = &FrntBrshSpeedEnc,
  .anim = main_brushAnim,
  .hdl_VISU_BR_ACTIVE = HDL_MAIN_BR_ACTIVE,
  .hdl_visu_BR_ANIM = HDL_MAIN_BR_ANIM,
  .hdl_VISU_BRSH_SPEED = HDL_MAIN_BRSH_SPEED,
  .min = 370, .max = 800,
  .Onoff =0, .OnEditRPM =0, .curValEnc =0,.currPromile =0,
};
encoder_butt sideBrshEncBut = {
  .namePWM_IO = "side_brush_spd",
  .pEnc = &SideBrshSpeedEnc,
  .anim = brushAnim,
  .hdl_VISU_BR_ACTIVE = HDL_BRUSH_ACTIVE_VAR,
  .hdl_visu_BR_ANIM = HDL_BRUSHESANIMATION,
  .hdl_VISU_BRSH_SPEED = HDL_BRUSH_SPEED_VAR,
  .min = 270, .max = 800,

  .Onoff =0, .OnEditRPM =0, .curValEnc =0,.currPromile =0,
};

u32 FiFo_WorkEq;
tGsThread id_EqWorkThread;
tGsThread id_TurboSendThr;
static s8 coverReq = stat_idle;
static u32 CommonOnProc = 0;



void ini_work_eq(void) {
    FiFo_WorkEq = CANCreateFiFo(64);
    int rs =
        CANAddRxFilterFiFo(MCM_CAN_ch, 0x0CFDD803, 0x1FFFFFFF, 1, FiFo_WorkEq);
    rs = CANAddRxFilterFiFo(MCM_CAN_ch, 0x18FFA0E0, 0x1FFFFFFC, 1, FiFo_WorkEq);
    rs = CANAddRxFilterFiFo(MCM_CAN_ch, 0x18FF90E0, 0x1FFFFFFC, 1, FiFo_WorkEq);

    int th = gsThreadCreate(&id_EqWorkThread, NULL, Thread_EqWork, NULL);
    th = gsThreadCreate(&id_TurboSendThr, NULL, Thread_Turbo_Send, NULL);

}

static void* Thread_EqWork(void* arg) {
    while (1) {
        tCanMessage tMsg;
        u32 n = CANReadFiFo(FiFo_WorkEq, &tMsg, 1);
        if (n) {
            parsePGN_WorkCtrl(&tMsg);
        }
        usleep(10000);
    }
    return 0;
}

/****************************************************************************
 * @brief  description
 * @param  mode_main
 * @return none
 ****************************************************************************/
void work_cycle(u8 mode_main) {
    // static u8 onoff_y11 =0;
    mode_main = ON_WORK;
  if (mode_main == ON_WORK)
    {
        trunk_shaft_cycl();
       Brush_Adjst_cycl(&sideBrshEncBut); // sideBrush_cycl();
        Brush_Adjst_cycl(&FrntBrshEncBut); // main_Brush_cycl();
        Turb_cycl();
        bunker_cycl();
        cover_cycl();
        side_brush_fold_cycl();
        wetting_cycl();
        fr_br_up_dwn_cycl();
        fr_br_tilt_cycl();
        fr_br_fold_cycl();
        // usleep(10000);  /// 50 ms
    }
    if (CommonOnProc == 0){
          MR_SetDO_byName("hyd_comon", 0);
    }
    if((sideBrshEncBut.Onoff == 0) && (FrntBrshEncBut.Onoff == 0))
      Inv_Hydr.TorqPercent = 25;
}
static void side_brush_fold_cycl(void) {
    static s8 isOnFold = 0;
    if (side_brush_fold_butt.stat.bits.isNew) {
        if ((side_brush_fold_butt.val & 0x40)) {  // складываем
            if (isOnFold == 0) {
                MR_SetDO_byName("lft_brsh_fold", 1);
                MR_SetDO_byName("rght_brsh_fold", 1);
                usleep(COMON_DELAY);  /// 50 ms
                MR_SetDO_byName("hyd_comon", 1);

                CommonOnProc |= 2;
            }
            isOnFold = 1;
        } else if ((side_brush_fold_butt.val & 0x10)) {
            if (isOnFold == 0) {
                MR_SetDO_byName("lft_brsh_unfold", 1);
                MR_SetDO_byName("rght_brsh_unfold", 1);
                // usleep(COMON_DELAY);  /// 50 ms
                // MR_SetDO_byName("hyd_comon", 1);
                // CommonOnProc |= 2;
            }
            isOnFold = -1;
        } else if (((side_brush_fold_butt.val & 0x50) == 0) &&(isOnFold )){
            // MR_SetDO_byName("hyd_comon", 0);
            CommonOnProc &= ~((u32)2);
            MR_SetDO_byName("lft_brsh_fold", 0);
            MR_SetDO_byName("lft_brsh_unfold", 0);
            MR_SetDO_byName("rght_brsh_fold", 0);
            MR_SetDO_byName("rght_brsh_unfold", 0);
            isOnFold = 0;
        }
    }
    side_brush_fold_butt.stat.bits.isNew = 0;
    side_brush_fold_butt.stat.bits.onBusy = 0;

    SendToVisuObj(OBJ_BRUSH_FOLD_ACT, GS_TO_VISU_SET_ATTR_VISIBLE,  (isOnFold == 1));
    SendToVisuObj(OBJ_BRUSH_UNFOLD_ACT, GS_TO_VISU_SET_ATTR_VISIBLE, (isOnFold == -1));
}

void extBut_BOXUP_CB(u8 val) {
    if (bunker_up_dwn_butt.stat.bits.isNew == 0) {

        bunker_up_dwn_butt.val &= ~((s32)0x0C);
        bunker_up_dwn_butt.val |= (val & 0x0C);
        bunker_up_dwn_butt.stat.bits.isNew = 1;
    }
}

static void bunker_cycl(void) {
    static s8 isOnUP = 0;
    static s32 timeAnim = 0;
    if (bunker_up_dwn_butt.stat.bits.isNew) {
        // bunker_up_dwn_butt.stat.bits.onBusy = 1;
        if ((bunker_up_dwn_butt.val & 0x48)) {  // поднимаем
            if (isOnUP == 0) {
                MR_SetDO_byName("box_up", 1);
                usleep(COMON_DELAY);  /// 50 ms
                MR_SetDO_byName("hyd_comon", 1);
                CommonOnProc |= 4;
                timeAnim = 40;
            }
            timeAnim++;
            // timeAnim =constrain(timeAnim,0,200);
            // bunkerAnim(timeAnim / 50);
            isOnUP = 1;

        } else if ((bunker_up_dwn_butt.val & 0x14)) {
            if (isOnUP == 0) {
                MR_SetDO_byName("box_dwn", 1);
                usleep(COMON_DELAY);  /// 50 ms
                MR_SetDO_byName("hyd_comon", 1);
                CommonOnProc |= 4;
                timeAnim = 160;
            }
            timeAnim--;
            isOnUP = -1;
        } else if (((bunker_up_dwn_butt.val & 0x5C) == 0) && (isOnUP != 0)) {
            // MR_SetDO_byName("hyd_comon", 0);
            CommonOnProc &= ~((u32)4);
            MR_SetDO_byName("box_up", 0);
            MR_SetDO_byName("box_dwn", 0);
            isOnUP = 0;
        }
        timeAnim = constrain(timeAnim, 0, 200);
        bunkerAnim(timeAnim / 50);

        SendToVisuObj(OBJ_BOX_DWN_ACT, GS_TO_VISU_SET_ATTR_VISIBLE,
                      (isOnUP == -1));
        SendToVisuObj(OBJ_BOX_UP_ACT, GS_TO_VISU_SET_ATTR_VISIBLE,
                      (isOnUP == 1));

        bunker_up_dwn_butt.stat.bits.isNew = 0;
        bunker_up_dwn_butt.stat.bits.onBusy = 0;
    }
}

// static void bunker_cycl(void) {
//     static u8 isUP =0;
//     if (bunker_up_dwn_butt.stat.bits.isNew) {
//         bunker_up_dwn_butt.stat.bits.onBusy = 1;
//         if ((bunker_up_dwn_butt.val & 0x40)&&(isUP ==0)) {  // поднимаем
//             MR_SetDO_byName("box_up", 1);
//             usleep(COMON_DELAY);  /// 50 ms
//             MR_SetDO_byName("hyd_comon", 1);
//             // MR_SetDO_byName("box_up_dwn_II", 1);
//             for (u8 i = 0; i < 4; i++) {
//                  bunkerAnim(i);
//                  usleep(1250000);  /// 5s
//             }
//             isUP =1;
//         } else if ((bunker_up_dwn_butt.val & 0x10)&&(isUP ==1))  {
//             MR_SetDO_byName("box_dwn", 1);
//             usleep(COMON_DELAY);  /// 50 ms
//             MR_SetDO_byName("hyd_comon", 1);
//             // MR_SetDO_byName("box_up_dwn_I", 1);
//             for (int i = 3; i >= 0; i--) {
//                  bunkerAnim(i);
//                  usleep(1250000);  /// 5s
//             }
//             isUP =0;
//         }
//
//         MR_SetDO_byName("hyd_comon", 0);
//         MR_SetDO_byName("box_up", 0);
//         MR_SetDO_byName("box_dwn", 0);
//         bunker_up_dwn_butt.stat.bits.isNew = 0;
//         bunker_up_dwn_butt.stat.bits.onBusy = 0;
//     }
// }

static void fr_br_fold_cycl(void) {
    if (Frnt_br_butt_fold.stat.bits.isNew) {
        if (Frnt_br_butt_fold.val & 0x10) {  // раскл
            MR_SetDO_byName("lft_brsh_unfold", 1);
            usleep(COMON_DELAY);  /// 50 ms
            MR_SetDO_byName("hyd_comon", 1);
            CommonOnProc |= 16;
        } else if (Frnt_br_butt_fold.val & 0x40) {  // склад
            MR_SetDO_byName("fr_brsh_fold", 1);
            usleep(COMON_DELAY);  /// 50 ms
            MR_SetDO_byName("hyd_comon", 1);
            CommonOnProc |= 16;
        } else {
            MR_SetDO_byName("fr_brsh_fold", 0);
            MR_SetDO_byName("lft_brsh_unfold", 0);
            CommonOnProc &= ~((u32)16);
        }
        Frnt_br_butt_fold.val = 0;
        Frnt_br_butt_fold.stat.bits.isNew = 0;
    }
}


static void fr_br_tilt_cycl(void) {
    if (Frnt_br_butt_tilt.stat.bits.isNew) {
        if (Frnt_br_butt_tilt.val & 0x10) {  // вправо
            MR_SetDO_byName("fr_brsh_tilt_r", 1);
            usleep(COMON_DELAY);  /// 50 ms
            MR_SetDO_byName("hyd_comon", 1);
            CommonOnProc |= 32;
        } else if (Frnt_br_butt_tilt.val & 0x04) {  // влево
            MR_SetDO_byName("fr_brsh_tilt_l", 1);
            usleep(COMON_DELAY);  /// 50 ms
            MR_SetDO_byName("hyd_comon", 1);
            CommonOnProc |= 32;
        } else {
            MR_SetDO_byName("fr_brsh_tilt_r", 0);
            MR_SetDO_byName("fr_brsh_tilt_l", 0);
            CommonOnProc &= ~((u32)32);
        }
        Frnt_br_butt_tilt.val = 0;
        Frnt_br_butt_tilt.stat.bits.isNew = 0;
    }
}


static void fr_br_up_dwn_cycl(void) {
    if (Frnt_br_butt_up_dwn.stat.bits.isNew) {
        if ((Frnt_br_butt_up_dwn.val & 0x14) == 0x04)  {  // опускаем
            MR_SetDO_byName("fr_brsh_dwn", 1);
            CommonOnProc &= ~((u32)8);

        } else if ((Frnt_br_butt_up_dwn.val & 0x14) == 0x10) {  // поднимаем
            MR_SetDO_byName("fr_brsh_up", 1);
            usleep(COMON_DELAY);  /// 50 ms
            MR_SetDO_byName("hyd_comon", 1);
            CommonOnProc |= 8;
        } else {
            MR_SetDO_byName("fr_brsh_up", 0);
            MR_SetDO_byName("fr_brsh_dwn", 0);
            CommonOnProc &= ~((u32)8);
        }
        Frnt_br_butt_up_dwn.val = 0;
        Frnt_br_butt_up_dwn.stat.bits.isNew = 0;
    }
}

static void trunk_shaft_cycl(void) {

    if (trunk_shaft_butt.stat.bits.isNew) {
        trunk_shaft_butt.stat.bits.onBusy = 1;
        if (trunk_shaft_butt.val & 0x10) {  // раскладываем, опускаем
             SendToVisuObj(OBJ_BUT_ACT_1, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
            MR_SetDO_byName("hyd_comon", 0);
            MR_SetDO_byName("lft_brsh_unfold", 1);
            usleep(TIME_HYD_WORK);  /// 3s
            MR_SetDO_byName("lft_brsh_unfold", 0);

            MR_SetDO_byName("rght_brsh_unfold", 1);
            usleep(TIME_HYD_WORK);  /// 3s
            MR_SetDO_byName("rght_brsh_unfold", 0);

            MR_SetDO_byName("trunk_shaft_dwn", 1);
            usleep(TIME_HYD_WORK);  /// 3s
            MR_SetDO_byName("trunk_shaft_dwn", 0);

            MR_SetDO_byName("side_brsh_dwn", 1);
            usleep(TIME_HYD_WORK);  /// 3s
            MR_SetDO_byName("side_brsh_dwn", 0);

            // usleep(TIME_HYD_WORK);  /// 3s
            // MR_SetDO_byName("hyd_comon", 0);
            // CommonOnProc |= 1;
            SetVar(HDL_BRUSHESANIMATION,1);

        } else if (trunk_shaft_butt.val & 0x40) {  // поднимаем, складываем
        SendToVisuObj(OBJ_BUT_ACT_2, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
            MR_SetDO_byName("side_brsh_up", 1);
            MR_SetDO_byName("trunk_shaft_up", 1);
            usleep(COMON_DELAY);  /// 50 ms
            MR_SetDO_byName("hyd_comon", 1);
            SetVar(HDL_BRUSHESANIMATION,0);

            usleep(TIME_HYD_WORK);  /// 3s
            MR_SetDO_byName("side_brsh_up", 0);
            MR_SetDO_byName("trunk_shaft_up", 0);


            // usleep(TIME_HYD_WORK);  /// 3s

            MR_SetDO_byName("lft_brsh_fold", 1);
            usleep(TIME_HYD_WORK);  /// 3s
            MR_SetDO_byName("lft_brsh_fold", 0);
            MR_SetDO_byName("rght_brsh_fold", 1);
            usleep(TIME_HYD_WORK);  /// 3s
            MR_SetDO_byName("rght_brsh_fold", 0);

            MR_SetDO_byName("hyd_comon", 0);
            // CommonOnProc |= 1;
        }
        else if ((trunk_shaft_butt.val & 0x50) == 0) {
            // MR_SetDO_byName("hyd_comon", 0);
            CommonOnProc &= ~((u32)1);
            MR_SetDO_byName("side_brsh_dwn", 0);
            MR_SetDO_byName("side_brsh_up", 0);
            MR_SetDO_byName("trunk_shaft_up", 0);
            MR_SetDO_byName("trunk_shaft_dwn", 0);
            MR_SetDO_byName("rght_brsh_fold", 0);
            MR_SetDO_byName("rght_brsh_unfold", 0);
            MR_SetDO_byName("lft_brsh_unfold", 0);
            MR_SetDO_byName("lft_brsh_fold", 0);
        }
        trunk_shaft_butt.val =0;
        trunk_shaft_butt.stat.bits.isNew = 0;
        trunk_shaft_butt.stat.bits.onBusy = 0;
        SendToVisuObj(OBJ_BUT_ACT_1, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
        SendToVisuObj(OBJ_BUT_ACT_2, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    }
}

void cover_set_up_dwn(void) {
    u8 v = GetVar(HDL_CAP_DOWN_VAR);
    if (coverReq == stat_idle) {
        if (v)  coverReq = stat_on_close | stat_busy;
        else coverReq = stat_on_open | stat_busy;
    }
}

static void cover_cycl(void) {
    if (coverReq & stat_busy) {
        if (coverReq & stat_on_open) {  // поднимаем
            MR_SetDO_byName("box_cover_up", 1);
            usleep(COMON_DELAY);  /// 50 ms
            MR_SetDO_byName("hyd_comon", 1);
            usleep(TIME_HYD_WORK);  /// 5s
            MR_SetDO_byName("hyd_comon", 0);

            SetVar(HDL_CAP_DOWN_VAR,1);

        } else if (coverReq & stat_on_close) {
            MR_SetDO_byName("box_cover_dwn", 1);
            // usleep(COMON_DELAY);  /// 50 ms
            // MR_SetDO_byName("hyd_comon", 1);
            usleep(5000000);  /// 5s
            SetVar(HDL_CAP_DOWN_VAR,0);
        }

            MR_SetDO_byName("box_cover_up", 0);
            MR_SetDO_byName("box_cover_dwn", 0);
         coverReq = stat_idle;
    }
}

static void wetting_cycl(void) {
    static u8 OnWetting = 0;
    if (wetting_butt.stat.bits.isPress) {
            OnWetting ^= 1;
            wetting_butt.stat.bits.isPress = 0;
    }
    SetVar(HDL_ON_WETTING, OnWetting);
    SendToVisuObj(OBJ_WETT_ICO, GS_TO_VISU_SET_ATTR_BLINK,OnWetting == 1);
    MR_SetDO_byName("wetting", OnWetting);
}



static void Brush_Adjst_cycl(encoder_butt * enc_but) {
    if (enc_but->pEnc->stat.bits.isPress) {
        if (enc_but->Onoff) {
            enc_but->OnEditRPM = 0;
            enc_but->Onoff = 0;
            enc_but->currPromile = 0;
        } else {
            enc_but->OnEditRPM ^= 1;
        }
        enc_but->pEnc->stat.bits.isPress = 0;
    }

    if (enc_but->OnEditRPM) {
        if (enc_but->pEnc->stat.bits.isNew) {
            s16 inc_promile = 0;
            if (enc_but->curValEnc > enc_but->pEnc->val)
                inc_promile = -5;
            else if (enc_but->curValEnc < enc_but->pEnc->val)
                inc_promile = 5;
            if(inc_promile){
                enc_but->currPromile = constrain(enc_but->currPromile,enc_but->min , enc_but->max);
                enc_but->currPromile += inc_promile;
                enc_but->currPromile = constrain(enc_but->currPromile,enc_but->min , enc_but->max);

                float diap = enc_but->max - enc_but->min;
                s8 add =   (s8)((float)(enc_but->currPromile - enc_but->min) *  15.0f/diap); // 0.20588235f;(s8)((float)(40 - 25))/((float)(450-280))
                Inv_Hydr.TorqPercent = 25 + add;

            }
            enc_but->Onoff = enc_but->currPromile > enc_but->min ? 1 : 0;

            enc_but->curValEnc = enc_but->pEnc->val;
            enc_but->pEnc->stat.bits.isNew = 0;
        }
    }
    MR_MCM_SetPWMOut_by_Name(
                        enc_but->namePWM_IO,
                        400 * enc_but->Onoff,
                        enc_but->currPromile * enc_but->Onoff);

    SetVar(enc_but->hdl_VISU_BRSH_SPEED,enc_but->currPromile*enc_but->Onoff);
    SetVar(enc_but->hdl_VISU_BR_ACTIVE, enc_but->OnEditRPM);


    static int an_div = 20;
    if (dec_elapced(an_div)){
        if((enc_but->Onoff != 0) && (enc_but->anim != 0))
         enc_but->anim();
        else SetVar(enc_but->hdl_visu_BR_ANIM,0);
        an_div = 20;
    }
}


// static void main_Brush_cycl(void) {
//     static s32 curValEnc = 0;
//     static u8 OnEditMainBrRPM = 0;
//     static s16 currPromile = 0;
//     static u8 Onoff = 0;
//     if (MainBrshSpeedEnc.stat.bits.isPress) {
//         if (Onoff) {
//             OnEditMainBrRPM = 0;
//             Onoff = 0;
//             currPromile = 0;
//         } else {
//             OnEditMainBrRPM ^= 1;
//         }
//         MainBrshSpeedEnc.stat.bits.isPress = 0;
//     }
//     if (OnEditMainBrRPM) {
//         if (MainBrshSpeedEnc.stat.bits.isNew) {
//             s16 inc_promile = 0;
//             if (curValEnc > MainBrshSpeedEnc.val)
//                 inc_promile = -5;
//             else if (curValEnc < MainBrshSpeedEnc.val)
//                 inc_promile = 5;
//             if(inc_promile){
//                 currPromile += inc_promile + 270;
//                 currPromile = constrain(currPromile, 270, 450);
//             }
//             Onoff = currPromile > 270 ? 1 : 0;
// //  s8 add =   (currPromile -280) * 0.20588235f; // (s8)((float)(60 - 25))/((float)(450-280))
// //  Inv_Hydr.TorqPercent = 25 + add;
//             curValEnc = MainBrshSpeedEnc.val;
//             MainBrshSpeedEnc.stat.bits.isNew = 0;
//         }
//     }
//     MR_MCM_SetPWMOut_by_Name("front_brush_spd", 400 * Onoff, currPromile * Onoff);
//     SetVar(HDL_MAIN_BRSH_SPEED,currPromile*Onoff);
//     SetVar(HDL_MAIN_BR_ACTIVE,OnEditMainBrRPM);
//
//     static int an_div = 20;
//     if (dec_elapced(an_div)){
//         if(currPromile > 0)   main_brushAnim();
//         else SetVar(HDL_MAIN_BR_ANIM,0);
//         an_div = 20;
//     }
// }

// static void sideBrush_cycl(void) {
//     static s32 curValEnc = 0;
//     static u8 OnEditSideBrRPM = 0;
//     static s16 currPromile = 0;
//
//
//     if (SideBrshSpeedEnc.stat.bits.isPress) {
//             OnEditSideBrRPM ^= 1;
//             SideBrshSpeedEnc.stat.bits.isPress = 0;
//     }
//
//     if (OnEditSideBrRPM) {
//
//         if (SideBrshSpeedEnc.stat.bits.isNew) {
//             s16 inc_promile = 0;
//             if (curValEnc > SideBrshSpeedEnc.val)
//                 inc_promile = -5;
//             else if (curValEnc < SideBrshSpeedEnc.val)
//                 inc_promile = 5;
//             currPromile += inc_promile;
//             currPromile = constrain(currPromile, 280, 800);
//             u8 Onoff = currPromile > 279 ? 1 : 0;
//             //currPromile *= Onoff;
//             MR_MCM_SetPWMOut_by_Name("side_brush_spd", 400, currPromile*Onoff);
//
//     //  s8 add =   (currPromile -280) * 0.20588235f; // (s8)((float)(60 - 25))/((float)(450-280))
//     //  Inv_Hydr.TorqPercent = 25 + add;
//
//             SetVar(HDL_BRUSH_SPEED_VAR,currPromile*Onoff);
//             curValEnc = SideBrshSpeedEnc.val;
//             SideBrshSpeedEnc.stat.bits.isNew = 0;
//         }
//     }
//     SetVar(HDL_BRUSH_ACTIVE_VAR,OnEditSideBrRPM);
//     static int an_div = 20;
//     if (dec_elapced(an_div)){
//         if(currPromile > 0)  brushAnim();
//         else SetVar(HDL_BRUSHESANIMATION,0);
//         an_div = 20;
//     }
// }

static void Turb_cycl(void) {
    static s32 curValEnc = 0;
    static u8 OnEditTurbRPM = 0;
    static s16 currRPM = 0;
    if (TurbSpeedEnc.stat.bits.isPress) {
            OnEditTurbRPM ^= 1;
            TurbSpeedEnc.stat.bits.isPress = 0;
    }

    if (OnEditTurbRPM) {
        if (TurbSpeedEnc.stat.bits.isNew) {
            s16 inc_rpm = 0;
            if (curValEnc > TurbSpeedEnc.val)
                inc_rpm = -100;
            else if (curValEnc < TurbSpeedEnc.val)
                inc_rpm = 100;
            currRPM += inc_rpm;
            currRPM = constrain(currRPM, 0, 3000);
            // u8 OnOff = 0;
            // if (currRPM > 100) OnOff = 1;
            // SendRequest_Turbo(OnOff, currRPM);
            SetVar(HDL_VACUUM_SPEED_VAR,currRPM);
            curValEnc = TurbSpeedEnc.val;
            TurbSpeedEnc.stat.bits.isNew = 0;
        }
    }
    SetVar(HDL_VACUUM_ACTIVE_VAR,OnEditTurbRPM);
}

static void SendRequest_Turbo(u8 OnOff, u16 RPM) {
    u16 tRpm = (RPM * OnOff + 15000) ;
    tCanMessage tx_msg;
    tx_msg.channel = ShassieCAN_ch;
    tx_msg.ext = 1;
    tx_msg.id = 0x0C6C00C2;
    tx_msg.len = 8;
    tx_msg.res = 0;
    tx_msg.data.u8[0] = 0x62;
    tx_msg.data.u8[1] = 3 + OnOff;
    tx_msg.data.u8[3] = (u8)((tRpm >> 8) & 0xFF);
    tx_msg.data.u8[2] = (u8)(tRpm & 0xFF);
    tx_msg.data.u32[1] = 0;
    CANSendMsg(&tx_msg);
}
static void* Thread_Turbo_Send(void* arg){
        SetVar(HDL_VACUUM_SPEED_VAR,0);
        usleep(2000000);
        while (1){
            u32 tRpm =GetVar(HDL_VACUUM_SPEED_VAR);
            SendRequest_Turbo((tRpm > 0),tRpm );
          usleep(20000);
        }
    return 0;
}


static int parsePGN_WorkCtrl(tCanMessage* msg) {
    u16 CurrPGN = GET_PGN(msg->id);
    u8 Sender = GET_SRCADR(msg->id);

    // u8 i = 0;
    switch (CurrPGN) {
        case 0xFDD8:  // джойстик
            if (trunk_shaft_butt.stat.bits.onBusy == 0) {
                trunk_shaft_butt.stat.bits.isPress = 1;
                trunk_shaft_butt.val = (msg->data.u8[6] & 0x50);
                trunk_shaft_butt.stat.bits.isNew = 1;
            }
            if (Frnt_br_butt_up_dwn.stat.bits.isNew == 0) {
                Frnt_br_butt_up_dwn.stat.bits.isNew = 1;
                Frnt_br_butt_up_dwn.val = (msg->data.u8[0] & 0x14);
            }
            if (Frnt_br_butt_tilt.stat.bits.isNew == 0) {
                Frnt_br_butt_tilt.stat.bits.isNew = 1;
                Frnt_br_butt_tilt.val = (msg->data.u8[1] & 0x14);
            }
            if (Frnt_br_butt_fold.stat.bits.isNew == 0) {
                Frnt_br_butt_fold.stat.bits.isNew = 1;
                Frnt_br_butt_fold.val = (msg->data.u8[5] & 0x50);
            }

            break;
        case 0xFF90:               // панель
            if (Sender == 0xE1) {  // центральная
                if (msg->data.u8[0] & 0x04) {
                        if(TurbSpeedEnc.stat.bits.prevPress == 0){
                            TurbSpeedEnc.stat.bits.prevPress =
                            TurbSpeedEnc.stat.bits.isPress = 1;
                        }
                    // }
                } else TurbSpeedEnc.stat.bits.prevPress =0;

            } else if (Sender == 0xE2) {  // боковая
                if (msg->data.u8[0] & 0x01){
                        if(SideBrshSpeedEnc.stat.bits.prevPress == 0){
                            SideBrshSpeedEnc.stat.bits.prevPress =
                            SideBrshSpeedEnc.stat.bits.isPress = 1;
                        }
                    // }
                } else SideBrshSpeedEnc.stat.bits.prevPress =0;

                if (msg->data.u8[0] & 0x04){
                        if(FrntBrshSpeedEnc.stat.bits.prevPress == 0){
                            FrntBrshSpeedEnc.stat.bits.prevPress =
                            FrntBrshSpeedEnc.stat.bits.isPress = 1;
                        }
                    // }
                } else FrntBrshSpeedEnc.stat.bits.prevPress =0;

                if (bunker_up_dwn_butt.stat.bits.isNew == 0) {
                    u8 t =  bunker_up_dwn_butt.val &= ~(0x50) ;
                    t |= msg->data.u8[0] & 0x50;
                    bunker_up_dwn_butt.val = t;
                    bunker_up_dwn_butt.stat.bits.isNew = 1;
                }
                side_brush_fold_butt.val = msg->data.u8[1] & 0x50;
                side_brush_fold_butt.stat.bits.isNew = 1;


                if (msg->data.u8[1] & 0x04) {
                    if (wetting_butt.stat.bits.prevPress == 0) {
                        wetting_butt.stat.bits.prevPress =
                            wetting_butt.stat.bits.isPress = 1;
                    }
                    // }
                }else wetting_butt.stat.bits.prevPress =0;

                SendToVisuObj(OBJ_WETTING_ACT, GS_TO_VISU_SET_ATTR_VISIBLE,
                        wetting_butt.stat.bits.isPress);
            }
            break;
        case 0xFFA0:               // энкодеры панелей
            if (Sender == 0xE2) {  // боковая
                SideBrshSpeedEnc.val = (msg->data.s32[0]);
                SideBrshSpeedEnc.stat.bits.isNew = 1;

                FrntBrshSpeedEnc.val = (msg->data.s32[1]);
                FrntBrshSpeedEnc.stat.bits.isNew = 1;

            } else if (Sender == 0xE1) {  // центральная
                TurbSpeedEnc.val = (msg->data.s32[1]);
                TurbSpeedEnc.stat.bits.isNew = 1;
            }
            break;

        default:
            break;
    }
    return 1;
}
