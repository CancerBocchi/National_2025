#ifndef __SINGLE_PFC_OB_H__
#define __SINGLE_PFC_OB_H__

#include "single_pfc.h"

extern hrtim_sontim Bridge_Driver1;
extern hrtim_sontim Bridge_Driver2;


extern Pos_PID_t Current_PI; //������·
extern Pos_PID_t Volt_PI;    //��ѹ��·
extern SPLL_1PH_SOGI SPLL;   //���໷
extern DCL_DF22 Bus_Notch;   //�����ݲ���
extern DCL_DF22 Current_PR;  //����PR
extern POWER_MEAS_SINE_ANALYZER measure; //���ʲ���

extern SinglePFC_Typedef PFC;

void Single_PFC_ob_Init();

#endif