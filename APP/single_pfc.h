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

//高分辨率定时器时钟频率
#define CLOCK_FREQ                      (170000000.0f*16.0f)
//锁相环参数
#define SINGLE_PFC_PLL_B0               (166.8806032f)
#define SINGLE_PFC_PLL_B1               (-166.3598518f)

//采样最高值 国际单位
#define SINGLE_PFC_BUS_MAX              (48.0669f)
#define SINGLE_PFC_AC_MAX               (39.5f)
#define SINGLE_PFC_CURRENT_MAX          (15.3099f)
#define SINGLE_PFC_DC_CURR_MAX          (0.0f)

//保护阈值
#define SINGLE_PFC_OBUSV_TH             (40.0f/SINGLE_PFC_BUS_MAX)
#define SINGLE_PFC_OBUSI_TH             (5.0f/SINGLE_PFC_CURRENT_MAX)
#define SINGLE_PFC_OACV_TH              (40.0f/SINGLE_PFC_AC_MAX)
#define SINGLE_PFC_LACV_TH              (10.0f/SINGLE_PFC_AC_MAX)    
#define SINGLE_PFC_OACI_TH              (5.0f/SINGLE_PFC_CURRENT_MAX)

#define SINGLE_PFC_AC_PTICK             (0.2f)
#define SINGLE_PFC_DC_PTICK             (0.2f)

// #define SINGLE_PFC_OBUSI_TH             ()

//PF值
#define SINGLE_PFC_PF_MIN               (0.80f)
//电流环电压环频率壁纸
#define SINGLE_PFC_CV_FRE_RATE          (100)//电压环电流环频率比值
//电压环软起动参数
#define SINGLE_PFC_SOFT_STARTUP_VAL     (15.0f)//软起动速度
#define SINGLE_PFC_SOFT_START_V         (30.0f)//软起动的初始值
//归一化参数
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
    HRTIM_Timerx_TypeDef*   htimx;              //HRTIM子定时器
    uint32_t                tim_channel[2];     //定时器通道
    uint32_t                tim_id;             //定时器标识
    uint32_t                tim_cmp;            //定时器比较寄存器
}hrtim_sontim;

typedef enum{
    SINGLE_PFC_OUTPUT,    //输出
    SINGLE_PFC_PROTECTION,   //保护
    SINGLE_PFC_CZ,        //检测正向过零
    SINGLE_PFC_PLL,       //等待锁相
    SINGLE_PFC_IDLE

}Single_PFC_State;

typedef struct{

    Single_PFC_State            state;              //描述系统状态
    
    //外设相关
    ADC_HandleTypeDef*          hadc;               //ADC外设
    hrtim_sontim*               tim[2];             //HRTIM子定时器
    SPLL_1PH_SOGI*              PLL;                //锁相环  

    int32_t                     raw_adc[3];         //ADC 原始数据
    int32_t                     adc_rawoffset_ac;   //ADC ac偏移量
    int32_t                     adc_rawoffset_bus;  //ADC bus偏移量
    int32_t                     adc_rawoffset_L;    //ADC 电流偏移量

    float                       angle_offset;      //相位偏移量
    float                       angle_off;          //实际传入           

    float                       loop_fre;          //环路周期周期
    float                       ac_fre;            //交流频率   

    //采样
    float                       bus_volt;           //母线电压
    float                       ac_volt;            //输入交流电压
    float                       L_current;          //电感电流

    float                       bus_volt_filter;    
    float                       ac_volt_filter;            

    //运放系数
    float                       bus_volt_coeff;     //输出电压系数
    float                       L_current_coeff;    //电感电流系数
    float                       ac_volt_coeff;      //输入交流电压系数

    //电流电压保护阈值
    float                       L_current_Protect;  //电感电流保护
    float                       bus_volt_Protect;   //输出电压保护

    //控制期望
    float                       Current_Ref;        //输入电流期望
    float                       Volt_Ref;           //输出电压期望

    //数字算法
    Pos_PID_t*                  LCurrent_pid;       //电感电流控制器
    Pos_PID_t*                  BusVol_pid;         //输出电压 

    DCL_DF22*                   L_Current_PR;    //电感电流陷波器
    DCL_DF22*                   Bus_Volt_Notch;     //母线电压陷波器

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
