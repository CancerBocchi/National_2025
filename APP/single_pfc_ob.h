#ifndef __SINGLE_PFC_OB_H__
#define __SINGLE_PFC_OB_H__

#include "single_pfc.h"

extern hrtim_sontim Bridge_Driver1;
extern hrtim_sontim Bridge_Driver2;


extern Pos_PID_t Current_PI; //电流环路
extern Pos_PID_t Volt_PI;    //电压环路
extern SPLL_1PH_SOGI SPLL;   //锁相环
extern DCL_DF22 Bus_Notch;   //总线陷波器
extern DCL_DF22 Current_PR;  //电流PR
extern POWER_MEAS_SINE_ANALYZER measure; //功率测量

extern SinglePFC_Typedef PFC;

void Single_PFC_ob_Init();

#endif