/****************************************************************************
 *
 * Project:   name
 *
 * @file      filename.c
 * @author    author
 * @date      [Creation date in format %02d.%02d.20%02d]
 *
 * @brief     description
 *
 ****************************************************************************/


#include "main_px_bat.h"
#include "mr_MCM.h"
/****************************************************************************
 * @brief
 ****************************************************************************/
 sett_SLAVE_Device SlaveDevs_sett[] =
{
	{.N_MCM = 0,.Cfg.CAN = MCM_CAN_ch, .Cfg.Ext = 0, .Cfg.RX_ID = 0x21, .Cfg.TX_ID[0] = 34, .Cfg.name = "MCM250_0", .type_MCM = MCMTYPE_MCM250}, //0x22 // TX_ID
  {.N_MCM = 1,.Cfg.CAN = MCM_CAN_ch, .Cfg.Ext = 0, .Cfg.RX_ID = 0x23, .Cfg.TX_ID[0] = 36, .Cfg.name = "MCM250_1", .type_MCM = MCMTYPE_MCM250}, //0x18
  {.N_MCM = 2,.Cfg.CAN = MCM_CAN_ch, .Cfg.Ext = 0, .Cfg.RX_ID = 0x25, .Cfg.TX_ID[0] = 38, .Cfg.name = "MCM250_2", .type_MCM = MCMTYPE_MCM250}, //0x1A
  {.N_MCM = 3,.Cfg.CAN = MCM_CAN_ch, .Cfg.Ext = 0, .Cfg.RX_ID = 0x27, .Cfg.TX_ID[0] = 40, .Cfg.name = "MCM250_3", .type_MCM = MCMTYPE_MCM250}, //0x1c

};
// Cfg_strct JoystickCfg = {
//   .TX_ID [0] = 0x0CFDD600,
//   .TX_ID [1] = 0x0CFDD700,
//   .CAN = CAN0,
//   .Ext = BIT29,
//   .name = "GESSMAN"
// };

/****************************************************************************
 * @brief
 ****************************************************************************/
// u32 NumSlave_DEVISES_ON_SYSTEM = GS_ARRAYELEMENTS(SlaveDevs_sett);
/****************************************************************************
 * @brief   конфигурация вводов - выводов
 ****************************************************************************/
void Cfg_MCM_IOs(inpOutp_tdf* targetIOs){
  for(int p = 0; p < 18; p++){
  	targetIOs->NumAnInp = 255;
//    if(targetIOs->typeIo == NOT_USE )continue;
//    else
    if(targetIOs->typeIo == DOut){
        targetIOs->Hndle = DOut;
        switch (targetIOs->PinNum){
        case 3:  targetIOs->cast.DO.shiftBit = 0;targetIOs->NumAnInp = 4;break;
        case 4 :  targetIOs->cast.DO.shiftBit = 2;targetIOs->NumAnInp = 6;break;
        case 5 :  targetIOs->cast.DO.shiftBit = 1;targetIOs->NumAnInp = 5;break;
        case 6 :  targetIOs->cast.DO.shiftBit = 3;targetIOs->NumAnInp = 7;break;
        case 7 :  targetIOs->cast.DO.shiftBit = 5;targetIOs->NumAnInp = 9;break;
        case 8 :  targetIOs->cast.DO.shiftBit = 7;targetIOs->NumAnInp = 11;break;
        case 9 :  targetIOs->cast.DO.shiftBit = 4;targetIOs->NumAnInp = 8;break;
        case 10 : targetIOs->cast.DO.shiftBit = 6;targetIOs->NumAnInp = 10;break;

        case 28 : targetIOs->cast.DO.shiftBit = 8;break;
        case 29 : targetIOs->cast.DO.shiftBit = 9;break;

        default : targetIOs->Hndle = NOT_USE;
      }
    }
    else if(targetIOs->typeIo == PWMout){
      switch (targetIOs->PinNum){
       case 5 : targetIOs->Hndle = PWMout;targetIOs->NumAnInp = 5; break;
       case 6 : targetIOs->Hndle = PWMout+1;targetIOs->NumAnInp = 7; break;
       case 7 : targetIOs->Hndle = PWMout+2;targetIOs->NumAnInp = 9; break;
       case 8 : targetIOs->Hndle = PWMout+3;targetIOs->NumAnInp = 11; break;
        default : targetIOs->Hndle = NOT_USE;
      }
    }
    else if(targetIOs->typeIo == AnInp){
      switch (targetIOs->PinNum){
        case 20 : targetIOs->Hndle = AnInp; targetIOs->NumAnInp = 0;break;
        case 21 : targetIOs->Hndle = AnInp+1; targetIOs->NumAnInp = 1;break;
        case 22 : targetIOs->Hndle = AnInp+2; targetIOs->NumAnInp = 2;break;
        case 23 : targetIOs->Hndle = AnInp+3; targetIOs->NumAnInp = 3;break;
        default : targetIOs->Hndle = NOT_USE;
      }
    }
    else if(targetIOs->typeIo == DInp){
      if(IsInRange(targetIOs->PinNum,24,29)){
        targetIOs->Hndle = DInp;
        targetIOs->cast.DI.shiftBit = (targetIOs->PinNum -24);
        }
      else targetIOs->Hndle = NOT_USE;
    }
    else targetIOs->Hndle = NOT_USE;
    targetIOs++;
  }
}
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
/****************************************************************************
 * @brief
 ****************************************************************************/

inpOutp_tdf MCM_0 [18]={
		{ .PinNum =3,.typeIo = DOut,.Name = "hyd_comon"    },
		{ .PinNum =4,.typeIo = DOut,.Name = "side_brsh_up" },
		{ .PinNum =5,.typeIo = DOut,.Name = "side_brsh_dwn" },
		{ .PinNum =6,.typeIo = PWMout,.Name = "side_brush_spd" },
		{ .PinNum =7,.typeIo = DOut,.Name = "trunk_shaft_dwn"  }, //шахта и фартук
		{ .PinNum =8,.typeIo = DOut,.Name = "trunk_shaft_up"  }, //шахта и фартук 2
		{ .PinNum =9,.typeIo = DOut,.Name = "rght_brsh_unfold" }, //разложить
		{ .PinNum =10,.typeIo = DOut,.Name = "rght_brsh_fold" }, // сложить
		{ .PinNum =20,.typeIo = NOT_USE},
		{ .PinNum =21,.typeIo = NOT_USE},
		{ .PinNum =22,.typeIo = NOT_USE},
		{ .PinNum =23,.typeIo = NOT_USE},
		{ .PinNum =24,.typeIo = DInp,.Name = "key_pos_1"},
		{ .PinNum =25,.typeIo = DInp,.Name = "turtle"},
		{ .PinNum =26,.typeIo = DInp,.Name = "brake_pedal"},//
		{ .PinNum =27,.typeIo = DInp,.Name = "brake_press"},
		{ .PinNum =28,.typeIo = NOT_USE},//
		{ .PinNum =29,.typeIo = DOut,.Name = "key_pos_2"}//
};
inpOutp_tdf MCM_1 [18]={
		{ .PinNum =3,.typeIo = DOut,.Name = "front_brsh_revrs"},//
		{ .PinNum =4,.typeIo =  NOT_USE},
		{ .PinNum =5,.typeIo = NOT_USE},
		{ .PinNum =6,.typeIo =  NOT_USE},
		{ .PinNum =7,.typeIo = PWMout,.Name = "front_brush_spd" }, // ‰
		{ .PinNum =8,.typeIo = NOT_USE},
		{ .PinNum =9,.typeIo = NOT_USE},
		{ .PinNum =10,.typeIo = NOT_USE},
		{ .PinNum =20,.typeIo = NOT_USE},
		{ .PinNum =21,.typeIo = NOT_USE},
		{ .PinNum =22,.typeIo = NOT_USE},
		{ .PinNum =23,.typeIo = NOT_USE},
		{ .PinNum =24,.typeIo = NOT_USE},
		{ .PinNum =25,.typeIo = DInp,.Name = "hyd_oil_lvl"},
		{ .PinNum =26,.typeIo = DInp,.Name = "hyd_oil_term"},
		{ .PinNum =27,.typeIo = DInp,.Name = "ramp_open"},
		{ .PinNum =28,.typeIo = NOT_USE},
		{ .PinNum =29,.typeIo = NOT_USE}
};
inpOutp_tdf MCM_2 [18]={

		{ .PinNum =3,.typeIo = DOut,.Name = "box_cover_up"  }, //
		{ .PinNum =4,.typeIo = DOut,.Name = "box_cover_dwn"  }, //
		{ .PinNum =5,.typeIo = DOut,.Name = "box_dwn"  }, //
		{ .PinNum =6,.typeIo = DOut,.Name = "box_up"  }, //
		{ .PinNum =7,.typeIo = PWMout,.Name = "coolant_pump_spd" }, // ‰
		{ .PinNum =8,.typeIo = NOT_USE},
		{ .PinNum =9,.typeIo = NOT_USE},
		{ .PinNum =10,.typeIo = NOT_USE},
		{ .PinNum =20,.typeIo = NOT_USE},
		{ .PinNum =21,.typeIo = NOT_USE},
		{ .PinNum =22,.typeIo = NOT_USE},
		{ .PinNum =23,.typeIo = NOT_USE},
		{ .PinNum =24,.typeIo = DInp,.Name = "box_pos"}, // Датчик положения бункера
		{ .PinNum =25,.typeIo = DInp,.Name = "coolant_lvl"}, // Датчик уровня ОЖ
		{ .PinNum =26,.typeIo = DInp,.Name = "box_down_ind"}, // Кнопка опускания бункера
		{ .PinNum =27,.typeIo = DInp,.Name = "box_up_ind"}, // Кнопка поднятия бункера
		{ .PinNum =28,.typeIo = NOT_USE},
		{ .PinNum =29,.typeIo = NOT_USE},
};
inpOutp_tdf MCM_3 [18] = {
	{ .PinNum =3,.typeIo = DOut,.Name =  "fr_brsh_up"}, //0
	{ .PinNum =4,.typeIo = DOut,.Name =  "fr_brsh_dwn"},  //2
	{ .PinNum =5,.typeIo = DOut,.Name ="fr_brsh_fold" },//1
	{ .PinNum =6,.typeIo = DOut,.Name = "fr_brsh_tilt_r"},//3
	{ .PinNum =7,.typeIo = DOut,.Name = "lft_brsh_unfold"},//5
	{ .PinNum =8,.typeIo = DOut,.Name = "lft_brsh_fold"},//7
	{ .PinNum =9,.typeIo = DOut,.Name = "fr_brsh_tilt_l" },//4
	{ .PinNum =10,.typeIo = DOut,.Name = "fr_brsh_unfold"},//6
	{ .PinNum =20,.typeIo =  NOT_USE}, //
	{ .PinNum =21,.typeIo = NOT_USE},
	{ .PinNum =22,.typeIo = NOT_USE},
	{ .PinNum =23,.typeIo = NOT_USE},
	{ .PinNum =24,.typeIo =  NOT_USE}, //
	{ .PinNum =25,.typeIo =  NOT_USE}, //
	{ .PinNum =26,.typeIo =  NOT_USE}, //
	{ .PinNum =27,.typeIo = NOT_USE}, //
	{ .PinNum =28,.typeIo = NOT_USE}, //
	{ .PinNum =29,.typeIo = DOut,.Name = "wetting"},
};


