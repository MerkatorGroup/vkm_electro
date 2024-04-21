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

/****************************************************************************
 * @brief  description
 *
 * @param  yourParam1
 * @param  yourParam1
 * @return none
 ****************************************************************************/
void showWorkSite(){

}
void showbutt_ext(){
  // u8 v =
  // SendToVisuObj(OBJ_BUT_ACT_1, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
}
void brushAnim(void){
  s32 ind = GetVar(HDL_BRUSHESANIMATION);
  ind++;
  FillOnCircL(ind,1,5);
  SetVar(HDL_BRUSHESANIMATION,ind);
}
void main_brushAnim(void){
  s32 ind = GetVar(HDL_MAIN_BR_ANIM);
  ind++;
  FillOnCircL(ind,1,3);
  SetVar(HDL_MAIN_BR_ANIM,ind);
}

void bunkerAnim(int up_dwn){
  // s32 ind = GetVar(HDL_BUNKERANIMATION);
  // if(up_dwn)ind++;
  // else ind--;
  // FillOnCircL(up_dwn,0,3);
  SetVar(HDL_BUNKERANIMATION,up_dwn);
}

void show_Y11_STAT(void) {
    SendToVisuObj(OBJ_Y11_STAT, GS_TO_VISU_SET_ATTR_TRANSPARENT,
                  ((GetVar(SlvMCMs[0].MCM.MCM250.Var_digOut) & 1) == 0));
}

