/*
 *  VertBar.h
 *  Created on: 25 РјР°СЏ 2023 Рі.
 *      Author: ASUS
 */

#ifndef VERTBAR_H_
#define VERTBAR_H_

typedef struct _VertBar{
int HDL_RefValue;
int Min;
int Max;
int HDL_Bar_indx_val;
}VertBar;
extern void VertBars_Run(void);
#endif // VERTBAR_H_
