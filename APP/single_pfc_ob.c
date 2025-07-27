#include "single_pfc_ob.h"

hrtim_sontim Bridge_Driver1 = {

  .htimx = &(HRTIM1->sTimerxRegs[2]),
  .tim_channel[0] = HRTIM_OUTPUT_TC1,
  .tim_channel[1] = HRTIM_OUTPUT_TC2,
  .tim_id = HRTIM_TIMERID_TIMER_C,
  .tim_cmp = HRTIM_COMPAREUNIT_1
  
};

hrtim_sontim Bridge_Driver2 = {
  
  .htimx = &(HRTIM1->sTimerxRegs[1]),
  .tim_channel[0] = HRTIM_OUTPUT_TB1,
  .tim_channel[1] = HRTIM_OUTPUT_TB2,
  .tim_id = HRTIM_TIMERID_TIMER_B,
  .tim_cmp = HRTIM_COMPAREUNIT_1
  
};


Pos_PID_t           Current_PI; //������·
Pos_PID_t           Volt_PI;    //��ѹ��·
SPLL_1PH_SOGI       SPLL;   //���໷
POWER_MEAS_SINE_ANALYZER measure; //���ʲ���

DCL_DF22            Bus_Notch;   //�����ݲ���
DCL_DF22            Current_PR;  //����PR


SinglePFC_Typedef   PFC;

void Single_PFC_ob_Init(){

    Pos_PID_Init(&Current_PI,1,0.01,0);
    Current_PI.Output_Max = 0.5;
    Current_PI.Output_Min = -0.5;
    Current_PI.Value_I_Max = 10000;

    Pos_PID_Init(&Volt_PI,0.5f,0.1f,0);
    Volt_PI.Output_Max = 1;
    Volt_PI.Output_Min = 0;
    Volt_PI.Value_I_Max = 5000;

    

    //��ʼ��Ӳ��������㷨
    SinglePFC_Init(&PFC,&hadc1,
                  &Bridge_Driver1,
                  &Bridge_Driver2,
                  &SPLL,
                  50,SINGLE_PFC_PUCOE,SINGLE_PFC_PUCOE*2,SINGLE_PFC_PUCOE*2,
                  &Current_PI,&Volt_PI,&Bus_Notch,&Current_PR,&measure);
    

    //
    PFC.adc_rawoffset_ac  = 0.4908f * 4096;
    PFC.adc_rawoffset_bus = 0;
    PFC.adc_rawoffset_L   = 0.488f * 4096;

    Single_PFC_SetBusVolt(PFC,36.0f);

}