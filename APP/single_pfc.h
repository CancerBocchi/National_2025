#ifndef __SINGLE_PFC_H__
#define __SINGLE_PFC_H__

#include "hrtim.h"
#include "adc.h"

#include "spll_1ph_sogi.h"
#include "PID.h"
#include "DCL.h"
#include "DCLF32.h"
#include "CalCoeff.h"
#include "power_meas_sine_analyzer.h"

#include "stdbool.h"
#include "SEGGER_RTT.h"
#include "arm_math.h"

//�߷ֱ��ʶ�ʱ��ʱ��Ƶ��
#define CLOCK_FREQ                      (170000000.0f*16.0f)
//���໷����
#define SINGLE_PFC_PLL_B0               (166.8806032f)
#define SINGLE_PFC_PLL_B1               (-166.3598518f)

//�������ֵ ���ʵ�λ
#define SINGLE_PFC_BUS_MAX              (48.0669f)
#define SINGLE_PFC_AC_MAX               (39.5f)
#define SINGLE_PFC_CURRENT_MAX          (15.3099f)
#define SINGLE_PFC_DC_CURR_MAX          (0.0f)

//������ֵ
#define SINGLE_PFC_OBUSV_TH             (40.0f/SINGLE_PFC_BUS_MAX)
#define SINGLE_PFC_OBUSI_TH             (5.0f/SINGLE_PFC_CURRENT_MAX)
#define SINGLE_PFC_OACV_TH              (40.0f/SINGLE_PFC_AC_MAX)
#define SINGLE_PFC_LACV_TH              (10.0f/SINGLE_PFC_AC_MAX)    
#define SINGLE_PFC_OACI_TH              (5.0f/SINGLE_PFC_CURRENT_MAX)

#define SINGLE_PFC_AC_PTICK             (0.2f)
#define SINGLE_PFC_DC_PTICK             (0.2f)

// #define SINGLE_PFC_OBUSI_TH             ()

//PFֵ
#define SINGLE_PFC_PF_MIN               (0.80f)
//��������ѹ��Ƶ�ʱ�ֽ
#define SINGLE_PFC_CV_FRE_RATE          (100)//��ѹ��������Ƶ�ʱ�ֵ
//��ѹ�����𶯲���
#define SINGLE_PFC_SOFT_STARTUP_VAL     (15.0f)//�����ٶ�
#define SINGLE_PFC_SOFT_START_V         (30.0f)//���𶯵ĳ�ʼֵ
//��һ������
#define SINGLE_PFC_PUCOE                (0.000244140625f)           //1/4096

#define my_abs(vara)                    (((vara) < 0) ? (-(vara)) : (vara))

 
#define Single_PFC_SetBusVolt(pfc,bus_volt)     (pfc.Volt_Ref = bus_volt/SINGLE_PFC_BUS_MAX) 
#define Single_PFC_SetLCurrent(pfc,L_Current)   (pfc.Current_Ref = L_Current/SINGLE_PFC_CURRENT_MAX)

#define Single_PFC_GetRealPF(pfc)               (PFC.measure->powerFactor)
#define Single_PFC_GetBusVolt(pfc)              (pfc.bus_volt_filter*SINGLE_PFC_BUS_MAX)
#define Single_PFC_GetAcVolt(pfc)               (pfc.measure->vRms*SINGLE_PFC_AC_MAX)
#define Single_PFC_GetLCurrent(pfc)             (pfc.measure->iRms*SINGLE_PFC_CURRENT_MAX)

#define Single_PFC_GetBusVRef(pfc)              (pfc.Volt_Ref*SINGLE_PFC_BUS_MAX)
#define Single_PFC_GetPfRef(pfc)                (cosf(pfc.angle_offset))
#define Single_PFC_GetLCurrentRef(pfc)          (pfc.Current_Ref*SINGLE_PFC_CURRENT_MAX)

#define GATE_OPEN   true
#define GATE_CLOSE  false


typedef struct{
    HRTIM_Timerx_TypeDef*   htimx;              //HRTIM�Ӷ�ʱ��
    uint32_t                tim_channel[2];     //��ʱ��ͨ��
    uint32_t                tim_id;             //��ʱ����ʶ
    uint32_t                tim_cmp;            //��ʱ���ȽϼĴ���
}hrtim_sontim;

typedef enum{
    SINGLE_PFC_OUTPUT,    //���
    SINGLE_PFC_PROTECTION,   //����
    SINGLE_PFC_CZ,        //����������
    SINGLE_PFC_PLL,       //�ȴ�����
    SINGLE_PFC_IDLE

}Single_PFC_State;

typedef struct{

    Single_PFC_State            state;              //����ϵͳ״̬
    
    //�������
    ADC_HandleTypeDef*          hadc;               //ADC����
    hrtim_sontim*               tim[2];             //HRTIM�Ӷ�ʱ��
    SPLL_1PH_SOGI*              PLL;                //���໷  

    int32_t                     raw_adc[3];         //ADC ԭʼ����
    int32_t                     adc_rawoffset_ac;   //ADC acƫ����
    int32_t                     adc_rawoffset_bus;  //ADC busƫ����
    int32_t                     adc_rawoffset_L;    //ADC ����ƫ����

    float                       angle_offset;      //��λƫ����
    float                       angle_off;          //ʵ�ʴ���           

    float                       loop_fre;          //��·��������
    float                       ac_fre;            //����Ƶ��   

    //����
    float                       bus_volt;           //ĸ�ߵ�ѹ
    float                       ac_volt;            //���뽻����ѹ
    float                       L_current;          //��е���

    float                       bus_volt_filter;    
    float                       ac_volt_filter;            

    //�˷�ϵ��
    float                       bus_volt_coeff;     //�����ѹϵ��
    float                       L_current_coeff;    //��е���ϵ��
    float                       ac_volt_coeff;      //���뽻����ѹϵ��

    //������ѹ������ֵ
    float                       L_current_Protect;  //��е�������
    float                       bus_volt_Protect;   //�����ѹ����

    //��������
    float                       Current_Ref;        //�����������
    float                       Volt_Ref;           //�����ѹ����

    //�����㷨
    Pos_PID_t*                  LCurrent_pid;       //��е���������
    Pos_PID_t*                  BusVol_pid;         //�����ѹ 

    DCL_DF22*                   L_Current_PR;    //��е����ݲ���
    DCL_DF22*                   Bus_Volt_Notch;     //ĸ�ߵ�ѹ�ݲ���

    POWER_MEAS_SINE_ANALYZER*   measure;

    float                       duty_out;
    float                       feed_forward;
    
    uint16_t OACI_tick;
    uint16_t OACV_tick;
    uint16_t LACV_tick;

    uint16_t OBUSV_tick;
    uint16_t OBUSI_tick;

    enum{
        SINGLE_PFC_OBUSVP = 1,
        SINGLE_PFC_OBUSIP = 2,
        SINGLE_PFC_OACVP = 4,
        SINGLE_PFC_LACVP = 8,
        SINGLE_PFC_OACIP = 16
    }protection_state;

    uint8_t                       protection_flag;
    
}SinglePFC_Typedef;



void SinglePFC_Init(SinglePFC_Typedef *pfc,
                    ADC_HandleTypeDef* hadc,
                    hrtim_sontim* tim1,
                    hrtim_sontim* tim2,
                    SPLL_1PH_SOGI* PLL,
                    float ac_fre,
                    float bus_volt_coeff,
                    float L_current_coeff,
                    float ac_volt_coeff,
                    Pos_PID_t *LCurrent_pid,
                    Pos_PID_t *BusVol_pid,
                    DCL_DF22  *Bus_Volt_Notch,
                    DCL_DF22  *L_Current_PR,
                    POWER_MEAS_SINE_ANALYZER* measure
                    );

void Single_PFC_GetADCvalue(SinglePFC_Typedef* pfc);
bool Single_PFC_CaliADC(SinglePFC_Typedef* pfc);
uint8_t Single_PFC_Protection(SinglePFC_Typedef* pfc);
void Single_PFC_GetADCvalue(SinglePFC_Typedef* pfc);
bool Single_PFC_PLL(SinglePFC_Typedef* pfc);
void Single_PFC_GateControl(SinglePFC_Typedef* pfc,bool open_close);
void Single_PFC_CurrentLoop(SinglePFC_Typedef* pfc,float current_ref);
float Single_PFC_VoltageLoop(SinglePFC_Typedef* pfc);
void Single_PFC_ChangePF(SinglePFC_Typedef* pfc,float pf);
void Single_PFC_Run(SinglePFC_Typedef* pfc);
void Single_PFC_Start(SinglePFC_Typedef* pfc);

float Slew_Func(float *slewVal, float refVal, float slewRate);


#endif // !__SINGLE_PFC_H__
