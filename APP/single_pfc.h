#ifndef __SINGLE_PFC_H__
#define __SINGLE_PFC_H__

#include "hrtim.h"
#include "adc.h"

#include "spll_1ph_sogi.h"
#include "PID.h"
#include "DCL.h"
#include "DCLF32.h"
#include "CalCoeff.h"

#include "stdbool.h"
#include "SEGGER_RTT.h"
#include "arm_math.h"

//高分辨率定时器时钟频率
#define CLOCK_FREQ                      (170000000.0f*16.0f)
#define SINGLE_PFC_PLL_B0               (166.8806032f)
#define SINGLE_PFC_PLL_B1               (-166.3598518f)

#define SINGLE_PFC_BUS_MAX              (48.0669f)
#define SINGLE_PFC_AC_MAC               (60.97336f)
#define SINGLE_PFC_CURRENT_MAX          (15.3099f)

#define SINGLE_PFC_BUS_PROTECTION       
#define SINGLE_PFC_CURRENT_PROTECTION

#define SINGLE_PFC_PF_MIN               (0.80f)

#define SINGLE_PFC_CV_FRE_RATE          (100)//电压环电流环频率比值

#define SINGLE_PFC_SOFT_STARTUP_VAL     (15.0f)        //启动值

#define SINGLE_PFC_PUCOE                (0.000244140625f)           //1/4096

#define my_abs(vara)                    (((vara) < 0) ? (-(vara)) : (vara))

 
#define Single_PFC_SetBusVolt(pfc,bus_volt)     (pfc.Volt_Ref = bus_volt/SINGLE_PFC_BUS_MAX) 
#define Single_PFC_SetLCurrent(pfc,L_Current)   (pfc.Current_Ref = L_Current/SINGLE_PFC_CURRENT_MAX)

#define GATE_OPEN   true
#define GATE_CLOSE  false

// #define SINGLE_PFC_GATEENABLE(pfc)      (HAL_HRTIM_WaveformOutputStart(&hhrtim1,pfc->tim[0]->tim_channel[0]);
//                                         HAL_HRTIM_WaveformOutputStart(&hhrtim1,pfc->tim[0]->tim_channel[1]);
//                                         HAL_HRTIM_WaveformOutputStart(&hhrtim1,pfc->tim[1]->tim_channel[0]);
//                                         HAL_HRTIM_WaveformOutputStart(&hhrtim1,pfc->tim[1]->tim_channel[1]);)

// #define SINGLE_PFC_GATEDISABLE(pfc)     (HAL_HRTIM_WaveformOutputStop(&hhrtim1,pfc->tim[0]->tim_channel[0]);
//                                         HAL_HRTIM_WaveformOutputStop(&hhrtim1,pfc->tim[0]->tim_channel[1]);
//                                         HAL_HRTIM_WaveformOutputStop(&hhrtim1,pfc->tim[1]->tim_channel[0]);
//                                         HAL_HRTIM_WaveformOutputStop(&hhrtim1,pfc->tim[1]->tim_channel[1]);)

typedef struct{
    HRTIM_Timerx_TypeDef*   htimx;              //HRTIM子定时器
    uint32_t                tim_channel[2];     //定时器通道
    uint32_t                tim_id;             //定时器标识
    uint32_t                tim_cmp;            //定时器比较寄存器
}hrtim_sontim;

typedef enum{
    SINGLE_PFC_OUTPUT,    //输出
    SINGLE_PFC_OLIP,      //过流保护 电感电流
    SINGLE_PFC_OVP,       //过压保护  输出电压
    SINGLE_PFC_CZ,        //检测正向过零
    SINGLE_PFC_PLL,       //等待锁相

}Single_PFC_State;

typedef struct{

    Single_PFC_State        state;              //描述系统状态
    
    //外设相关
    ADC_HandleTypeDef*      hadc;               //ADC外设
    hrtim_sontim*           tim[2];             //HRTIM子定时器
    SPLL_1PH_SOGI*          PLL;                //锁相环  

    int32_t                raw_adc[3];         //ADC 原始数据
    int32_t                adc_rawoffset_ac;   //ADC ac偏移量
    int32_t                adc_rawoffset_bus;  //ADC bus偏移量
    int32_t                adc_rawoffset_L;    //ADC 电流偏移量

    float                   angle_offset;      //相位偏移量

    float                   loop_fre;          //环路周期周期

    //采样
    float                   bus_volt;           //母线电压
    float                   ac_volt;            //输入交流电压
    float                   L_current;          //电感电流

    float                   bus_volt_filter;    
    float                   ac_volt_filter;            

    //运放系数
    float                   bus_volt_coeff;     //输出电压系数
    float                   L_current_coeff;    //电感电流系数
    float                   ac_volt_coeff;      //输入交流电压系数

    //电流电压保护阈值
    float                   L_current_Protect;  //电感电流保护
    float                   bus_volt_Protect;   //输出电压保护

    //控制期望
    float                   Current_Ref;        //输入电流期望
    float                   Volt_Ref;           //输出电压期望

    //数字算法
    Pos_PID_t*              LCurrent_pid;       //电感电流控制器
    Pos_PID_t*              BusVol_pid;         //输出电压 

    DCL_DF22*               L_Current_PR;    //电感电流陷波器
    DCL_DF22*               Bus_Volt_Notch;     //母线电压陷波器

    float                   duty_out;
    float                   feed_forward;
    
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
                    DCL_DF22  *L_Current_PR
                    );

void Single_PFC_GetADCvalue(SinglePFC_Typedef* pfc);
bool Single_PFC_CaliADC(SinglePFC_Typedef* pfc);
void Single_PFC_Protection(SinglePFC_Typedef* pfc);
void Single_PFC_GetADCvalue(SinglePFC_Typedef* pfc);
bool Single_PFC_PLL(SinglePFC_Typedef* pfc);
void Single_PFC_GateControl(SinglePFC_Typedef* pfc,bool open_close);
void Single_PFC_CurrentLoop(SinglePFC_Typedef* pfc,float current_ref);
float Single_PFC_VoltageLoop(SinglePFC_Typedef* pfc);

void Single_PFC_ChangePF(SinglePFC_Typedef* pfc,float pf);

void Single_PFC_Run(SinglePFC_Typedef* pfc);

float Slew_Func(float *slewVal, float refVal, float slewRate);


#endif // !__SINGLE_PFC_H__
