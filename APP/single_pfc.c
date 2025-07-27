#include "single_pfc.h"

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
                    ){

    //外设初始化
    pfc->hadc = hadc;
    pfc->tim[0] = tim1;
    pfc->tim[1] = tim2;   
    pfc->ac_fre = ac_fre;

    //采样系数初始化
    pfc->ac_volt_coeff = ac_volt_coeff;
    pfc->bus_volt_coeff = bus_volt_coeff;
    pfc->L_current_coeff = L_current_coeff;

    //需要调用的数字算法                    
    pfc->LCurrent_pid = LCurrent_pid;
    pfc->BusVol_pid = BusVol_pid;
    pfc->Bus_Volt_Notch = Bus_Volt_Notch;
    pfc->L_Current_PR = L_Current_PR;
    pfc->measure = measure;

    //环路时间
    pfc->loop_fre = CLOCK_FREQ/(pfc->tim[0]->htimx->PERxR)/2;

    //初始化Notch PR
    computeDF22_NotchFltrCoeff(pfc->Bus_Volt_Notch, pfc->loop_fre, 2*ac_fre, 0.25f, 0.00001f);

    computeDF22_PRcontrollerCoeff(pfc->L_Current_PR,2.5f,50.0f,2.0*3.1415926f*ac_fre,
                                pfc->loop_fre,0.5f);
    //初始化SOGI
    pfc->PLL = PLL;
    SPLL_1PH_SOGI_config(pfc->PLL,pfc->ac_fre,pfc->loop_fre,SINGLE_PFC_PLL_B0,SINGLE_PFC_PLL_B1);//最后两个参数根据实际情况来调整
    
    POWER_MEAS_SINE_ANALYZER_reset(pfc->measure);
    POWER_MEAS_SINE_ANALYZER_config(pfc->measure, pfc->loop_fre,0,55,45);

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,1); //关断mos
    //开启定时器时钟 主定时器时钟用于同步
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,HRTIM_TIMERID_MASTER);
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,pfc->tim[0]->tim_id);
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,pfc->tim[1]->tim_id);

    pfc->state = SINGLE_PFC_IDLE;
    //开启ADC注入转换 并且校准ADC
    HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);

    HAL_Delay(100);

    HAL_ADCEx_InjectedStart(&hadc1);
    HAL_ADCEx_InjectedStart_IT(&hadc2);

    pfc->BusVol_pid->Ref = SINGLE_PFC_SOFT_START_V/SINGLE_PFC_BUS_MAX;

    pfc->protection_state = 0;

    pfc->state = SINGLE_PFC_IDLE;
    
    
}

/**
 * @brief PFC 调整PF值
 * 
 * @param pfc pfc控制块
 * @param pf 目标pf值
 */
void Single_PFC_ChangePF(SinglePFC_Typedef* pfc,float pf){
    if(pf <= 1 && pf >= SINGLE_PFC_PF_MIN){
        pfc->angle_offset = acosf(pf);
    }
}

/**
 * @brief  PFC电流环
 * 
 * @param pfc 
 * @param current_ref :输入电流期望，电流期望是电流的最大值
 */
void Single_PFC_CurrentLoop(SinglePFC_Typedef* pfc,float current_ref){
    
    float pid_out;

    Slew_Func(&pfc->angle_off,pfc->angle_offset,0.05f/pfc->loop_fre);
    //  期望正弦变化
    // pfc->LCurrent_pid->Ref = current_ref*pfc->PLL->sine;
    pfc->LCurrent_pid->Ref = current_ref * arm_sin_f32(pfc->PLL->theta + pfc->angle_off); 

    pid_out = DCL_runDF22_C4(pfc->L_Current_PR,pfc->LCurrent_pid->Ref - pfc->L_current);
    //前馈补偿
    //pfc->feed_forward = pfc->ac_volt/(2.0f*(pfc->bus_volt+0.001f));

    pid_out -= pfc->feed_forward;

    pfc->duty_out = pid_out + 0.5f;

    pfc->duty_out = (pfc->duty_out >= 0.999)? 0.999 : pfc->duty_out;
    pfc->duty_out = (pfc->duty_out <= 0.001)? 0.001 : pfc->duty_out;

    // static float an = 0;
    // an += 2*3.1415926f*50/pfc->loop_fre;

    // pfc->duty_out = 0.4*sinf(an)+0.5;

    pfc->tim[0]->htimx->CMP1xR = (uint32_t)(pfc->duty_out*pfc->tim[0]->htimx->PERxR);
    pfc->tim[1]->htimx->CMP1xR = (uint32_t)(pfc->duty_out*pfc->tim[1]->htimx->PERxR);

    // __HAL_HRTIM_SetCompare(&hhrtim1,pfc->tim[0]->tim_id,pfc->tim[0]->tim_cmp,(uint32_t)(pfc->duty_out*pfc->tim[0]->htimx->PERxR));
    // __HAL_HRTIM_SetCompare(&hhrtim1,pfc->tim[1]->tim_id,pfc->tim[1]->tim_cmp,(uint32_t)(pfc->duty_out*pfc->tim[1]->htimx->PERxR));

}

/**
 * @brief  PFC电压环
 * 
 * @param pfc 
 * @return float 返回电流峰峰值
 */
float Single_PFC_VoltageLoop(SinglePFC_Typedef* pfc){

    float pid_out;
    // pfc->BusVol_pid->Ref = pfc->Volt_Ref;
    //缓启动
    Slew_Func(&(pfc->BusVol_pid->Ref),pfc->Volt_Ref,(SINGLE_PFC_SOFT_STARTUP_VAL/SINGLE_PFC_BUS_MAX/pfc->loop_fre*SINGLE_PFC_CV_FRE_RATE));

    pid_out = Pos_PID_Controller(pfc->BusVol_pid,pfc->bus_volt_filter);
    return pid_out;

}

/**
 * @brief  PFC栅极控制函数
 * 
 */
void Single_PFC_GateControl(SinglePFC_Typedef* pfc,bool open_close){

    if(open_close){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,0);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1,pfc->tim[0]->tim_channel[0]);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1,pfc->tim[0]->tim_channel[1]);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1,pfc->tim[1]->tim_channel[0]);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1,pfc->tim[1]->tim_channel[1]);
    }
    else{
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,1);
        HAL_HRTIM_WaveformOutputStop(&hhrtim1,pfc->tim[0]->tim_channel[0]);
        HAL_HRTIM_WaveformOutputStop(&hhrtim1,pfc->tim[0]->tim_channel[1]);
        HAL_HRTIM_WaveformOutputStop(&hhrtim1,pfc->tim[1]->tim_channel[0]);
        HAL_HRTIM_WaveformOutputStop(&hhrtim1,pfc->tim[1]->tim_channel[1]);

    }
}

uint8_t Single_PFC_CheckUpZero(SinglePFC_Typedef* pfc){
    static int last_ac;
    uint8_t r_value;
    if(pfc->ac_volt_filter - last_ac > 0.0004f 
        && pfc->ac_volt_filter < 0.01f && pfc->ac_volt_filter > -0.01f)
        r_value = 1;
    else
        r_value = 0;
        
    last_ac = pfc->ac_volt_filter;
    
    return r_value;

}

void Single_PFC_reset(SinglePFC_Typedef* pfc){
    SPLL_1PH_SOGI_reset(pfc->PLL);
    SPLL_1PH_SOGI_config(pfc->PLL,pfc->ac_fre,pfc->loop_fre,SINGLE_PFC_PLL_B0,SINGLE_PFC_PLL_B1);//最后两个参数根据实际情况来调整

    DCL_resetDF22(pfc->L_Current_PR);
    DCL_resetDF22(pfc->Bus_Volt_Notch);

    computeDF22_NotchFltrCoeff(pfc->Bus_Volt_Notch, pfc->loop_fre, 2*pfc->ac_fre, 0.25f, 0.00001f);
    computeDF22_PRcontrollerCoeff(pfc->L_Current_PR,5.0f,50.0f,2.0*3.1415926f*pfc->ac_fre,
                                pfc->loop_fre,0.5f);

    POWER_MEAS_SINE_ANALYZER_reset(pfc->measure);
    POWER_MEAS_SINE_ANALYZER_config(pfc->measure, pfc->loop_fre,0,55,45);

    Pos_PID_Reset(pfc->BusVol_pid);

    pfc->BusVol_pid->Ref = SINGLE_PFC_SOFT_START_V/SINGLE_PFC_BUS_MAX;
    pfc->Current_Ref = 0;

    pfc->LACV_tick = 0;
    pfc->OACI_tick = 0;
    pfc->OACV_tick = 0;
    pfc->OBUSI_tick = 0;
    pfc->OBUSV_tick = 0;

    pfc->angle_off = 0;
}

/**
 * @brief 启动 PFC
 * 
 * @param pfc 
 */
void Single_PFC_Start(SinglePFC_Typedef* pfc){
    if(pfc->state == SINGLE_PFC_IDLE || pfc->state == SINGLE_PFC_PROTECTION){
        if(pfc->state == SINGLE_PFC_PROTECTION)
            Single_PFC_reset(pfc);

        pfc->state = SINGLE_PFC_PLL;
        pfc->protection_flag = 0;
    }
}

void Single_PFC_Stop(SinglePFC_Typedef* pfc){
    if(pfc->state != SINGLE_PFC_IDLE ){
        Single_PFC_GateControl(pfc,GATE_CLOSE);
        pfc->state = SINGLE_PFC_IDLE;
        Single_PFC_reset(pfc);
    }
}

/**
 * @brief  PFC运行，主要是状态机的运行
 * 
 * @param pfc 
 */
void Single_PFC_Run(SinglePFC_Typedef* pfc){

    static int tick;

    Single_PFC_GetADCvalue(pfc);
    Single_PFC_PLL(pfc);
    Single_PFC_Protection(pfc);

    //根据状态机进行控制
    switch(pfc->state){

        case SINGLE_PFC_PLL:
            if(tick++ >= pfc->loop_fre*0.5f)
                pfc->state = SINGLE_PFC_CZ;
            break;

        case SINGLE_PFC_CZ:
            if(Single_PFC_CheckUpZero(pfc)){
                Single_PFC_GateControl(pfc,GATE_OPEN);
                pfc->state = SINGLE_PFC_OUTPUT;
                tick = 0;
            }

            break;

        case SINGLE_PFC_OUTPUT:

            // if(Single_PFC_Protection(pfc)){
            //     pfc->state = SINGLE_PFC_PROTECTION;
            //     Single_PFC_GateControl(pfc,GATE_CLOSE);
            // }

            tick++;
            if(tick == SINGLE_PFC_CV_FRE_RATE){
                pfc->Current_Ref = Single_PFC_VoltageLoop(pfc);
                tick = 0;
            }
            // pfc->Current_Ref = 0.08f;
            Single_PFC_CurrentLoop(pfc,pfc->Current_Ref);
            break;

        case SINGLE_PFC_PROTECTION:
            Single_PFC_reset(pfc);
            break;

        case SINGLE_PFC_IDLE:

            break;
    }
}

/**
 * @brief 单相PFC的锁相环
 * 
 * @param pfc 
 * @return true  锁相成功
 * @return false 未能锁相成功
 */
bool Single_PFC_PLL(SinglePFC_Typedef* pfc){
    SPLL_1PH_SOGI_run(pfc->PLL,pfc->ac_volt);

    return  (my_abs(pfc->PLL->u_Q[0]) < 0.00001f)?(true):(false);

}

/**
 * @brief 单相PFC的ADC校准
 * 
 * @param pfc pfc对象
 * @return true 校准完成
 * @return false 校准未完成
 */
bool Single_PFC_CaliADC(SinglePFC_Typedef* pfc){
    static int n = 0;
    if(n < 1000){
        pfc->adc_rawoffset_ac += HAL_ADCEx_InjectedGetValue(&hadc1,ADC_INJECTED_RANK_1);
        pfc->adc_rawoffset_bus += HAL_ADCEx_InjectedGetValue(&hadc1,ADC_INJECTED_RANK_2);
        pfc->adc_rawoffset_L += HAL_ADCEx_InjectedGetValue(&hadc1,ADC_INJECTED_RANK_3);
        return false;
    }
    else{
        pfc->adc_rawoffset_ac /= 1000;
        pfc->adc_rawoffset_bus /= 1000;
        pfc->adc_rawoffset_L /= 1000;
        return true;
    }
}

/**
 * @brief 单相PFC的保护，当电流电压保护的时候，会关闭输出
 * 
 * @param pfc 
 */
uint8_t Single_PFC_Protection(SinglePFC_Typedef* pfc){


    pfc->measure->i = pfc->L_current;
    pfc->measure->v = pfc->ac_volt;
    POWER_MEAS_SINE_ANALYZER_run(pfc->measure);

    // if(pfc->measure->iRms)
    if(pfc->state == SINGLE_PFC_OUTPUT){
        
        pfc->OACI_tick = (pfc->measure->iRms > SINGLE_PFC_OACI_TH)?(pfc->OACI_tick + 1): 0;

        pfc->OACV_tick = (pfc->measure->vRms > SINGLE_PFC_OACV_TH)?pfc->OACV_tick + 1: 0;
        pfc->LACV_tick = (pfc->measure->vRms < SINGLE_PFC_LACV_TH)?pfc->LACV_tick + 1: 0;

        pfc->OBUSV_tick = (pfc->bus_volt > SINGLE_PFC_OBUSV_TH)?pfc->OBUSV_tick + 1: 0;

        if(pfc->OACI_tick > pfc->loop_fre*SINGLE_PFC_AC_PTICK)
            pfc->protection_flag |= SINGLE_PFC_OACIP;
        if(pfc->OACV_tick > pfc->loop_fre*SINGLE_PFC_AC_PTICK)
            pfc->protection_flag |= SINGLE_PFC_OACIP;
        if(pfc->LACV_tick > pfc->loop_fre*SINGLE_PFC_AC_PTICK)
            pfc->protection_flag |= SINGLE_PFC_LACVP;
        if(pfc->OBUSV_tick > pfc->loop_fre*SINGLE_PFC_DC_PTICK)
            pfc->protection_flag |= SINGLE_PFC_OBUSVP;

        if(pfc->protection_flag)
            return 1;

    }
    return 0;
}

/**
 * @brief 获取采样的真实值
 * 
 * @param pfc 
 */
void Single_PFC_GetADCvalue(SinglePFC_Typedef* pfc){

    pfc->raw_adc[0] = HAL_ADCEx_InjectedGetValue(&hadc2,ADC_INJECTED_RANK_3) - pfc->adc_rawoffset_ac;
    pfc->raw_adc[1] = HAL_ADCEx_InjectedGetValue(&hadc1,ADC_INJECTED_RANK_1) - pfc->adc_rawoffset_bus;
    pfc->raw_adc[2] = HAL_ADCEx_InjectedGetValue(&hadc2,ADC_INJECTED_RANK_2) - pfc->adc_rawoffset_L;


    pfc->ac_volt = (float)((int)pfc->raw_adc[0]) * pfc->ac_volt_coeff;
    pfc->bus_volt = (float)((int)pfc->raw_adc[1]) * pfc->bus_volt_coeff;
    pfc->L_current = (float)((int)pfc->raw_adc[2]) * pfc->L_current_coeff;

    //低通滤波 截至频率 5K
    pfc->ac_volt_filter += 1.1549972f*(pfc->ac_volt - pfc->ac_volt_filter);
    pfc->bus_volt_filter += 0.11549972f*(pfc->bus_volt - pfc->bus_volt_filter);

    //陷波器
    pfc->bus_volt_filter = DCL_runDF22_C4(pfc->Bus_Volt_Notch,pfc->bus_volt_filter);

}

