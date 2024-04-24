/*
 *  work_site.h
 */

#ifndef WRK_SITE_H_
#define WRK_SITE_H_

#include "main_px_bat.h"


typedef enum {
    stat_idle = 0,
    stat_up_down = 1,
    stat_fold = 2,
    stat_tilt = 4,
    stat_revers = 8,
    stat_on_open = 16,
    stat_on_close = 32,
    stat_busy = 128
} state;

typedef struct {
    char* ctrlPinName_0;
    char* ctrlPinName_1;
    // void (*doIt)(void* eq)
} action;

typedef struct {
    state stat;
    action fold;
    action up_down;
    action run;
} equip;

typedef union _st_butt {
    struct {
        u8 isNew : 1;
        u8 isPress : 1;
        u8 prevPress : 1;
        u8 onBusy : 1;
    } bits;
    u8 raw;
} st_butt;

typedef struct _butt {
    st_butt stat;
    s32 val;
} butt;

typedef struct _encoder_butt{
     s32 curValEnc ;
     u8 OnEditRPM;
     s16 currPromile;
     u8 Onoff;
    butt* pEnc;
    void(*anim)(void);
    char* namePWM_IO;
    s16 min;
    s16 max;
    int hdl_VISU_BRSH_SPEED;
    int hdl_VISU_BR_ACTIVE;
    int hdl_visu_BR_ANIM;
    u8 QuePress;
    s16 HydTorq_add ;
}encoder_butt;



void work_cycle (u8 mode_main);

void ini_work_eq(void);

void brushAnim(void);
void main_brushAnim(void);
void bunkerAnim(int up_dwn);
void cover_set_up_dwn(void);
void show_Y11_STAT(void);
void extBut_BOXUP_CB(u8 val);
#endif  // WRK_SITE_H_