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
                    DCL_DF22  *L_Current_PR
                    ){

    //�����ʼ��
    pfc->hadc = hadc;
    pfc->tim[0] = tim1;
    pfc->tim[1] = tim2;   

    //����ϵ����ʼ��
    pfc->ac_volt_coeff = ac_volt_coeff;
    pfc->bus_volt_coeff = bus_volt_coeff;
    pfc->L_current_coeff = L_current_coeff;

    //��Ҫ���õ������㷨                    
    pfc->LCurrent_pid = LCurrent_pid;
    pfc->BusVol_pid = BusVol_pid;
    pfc->Bus_Volt_Notch = Bus_Volt_Notch;
    pfc->L_Current_PR = L_Current_PR;

    //��·ʱ��
    pfc->loop_fre = CLOCK_FREQ/(pfc->tim[0]->htimx->PERxR)/2;

    //��ʼ��Notch PR
    computeDF22_NotchFltrCoeff(pfc->Bus_Volt_Notch, pfc->loop_fre, 2*ac_fre, 0.25f, 0.00001f);

    computeDF22_PRcontrollerCoeff(pfc->L_Current_PR,5.0f,50.0f,2.0*3.1415926f*ac_fre,
                                pfc->loop_fre,0.5f);
    //��ʼ��SOGI
    pfc->PLL = PLL;
    SPLL_1PH_SOGI_config(pfc->PLL,ac_fre,pfc->loop_fre,SINGLE_PFC_PLL_B0,SINGLE_PFC_PLL_B1);//���������������ʵ�����������
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,1); //�ض�mos
    //������ʱ��ʱ�� ����ʱ��ʱ������ͬ��
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,HRTIM_TIMERID_MASTER);
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,pfc->tim[0]->tim_id);
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,pfc->tim[1]->tim_id);

    pfc->state = SINGLE_PFC_CZ;
    //����ADCע��ת�� ����У׼ADC
    HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);

    HAL_Delay(100);

    HAL_ADCEx_InjectedStart(&hadc1);
    HAL_ADCEx_InjectedStart_IT(&hadc2);

    pfc->BusVol_pid->Ref = 10.0f/SINGLE_PFC_BUS_MAX;
    
    
}

/**
 * @brief PFC ����PFֵ
 * 
 * @param pfc pfc���ƿ�
 * @param pf Ŀ��pfֵ
 */
void Single_PFC_ChangePF(SinglePFC_Typedef* pfc,float pf){
    if(pf <= 1 && pf >= SINGLE_PFC_PF_MIN){
        pfc->angle_offset = acosf(pf);
    }

}

/**
 * @brief  PFC������
 * 
 * @param pfc 
 * @param current_ref :����������������������ǵ��������ֵ
 */
void Single_PFC_CurrentLoop(SinglePFC_Typedef* pfc,float current_ref){
    
    float pid_out;
    static float angle_off = 0;

    Slew_Func(&angle_off,pfc->angle_offset,0.05f/pfc->loop_fre);
    //  �������ұ仯
    // pfc->LCurrent_pid->Ref = current_ref*pfc->PLL->sine;
    pfc->LCurrent_pid->Ref = current_ref * arm_sin_f32(pfc->PLL->theta + angle_off); 

    pid_out = DCL_runDF22_C4(pfc->L_Current_PR,pfc->LCurrent_pid->Ref - pfc->L_current);
    //ǰ������
    //pfc->feed_forward = pfc->ac_volt/(2.0f*(pfc->bus_volt+0.001f));

    pid_out -= pfc->feed_forward;

    pfc->duty_out = pid_out + 0.5f;

    pfc->duty_out = (pfc->duty_out >= 0.99)? 0.99 : pfc->duty_out;
    pfc->duty_out = (pfc->duty_out <= 0.01)? 0.01 : pfc->duty_out;

    // static float an = 0;
    // an += 2*3.1415926f*50/pfc->loop_fre;

    // pfc->duty_out = 0.4*sinf(an)+0.5;

    pfc->tim[0]->htimx->CMP1xR = (uint32_t)(pfc->duty_out*pfc->tim[0]->htimx->PERxR);
    pfc->tim[1]->htimx->CMP1xR = (uint32_t)(pfc->duty_out*pfc->tim[1]->htimx->PERxR);

    // __HAL_HRTIM_SetCompare(&hhrtim1,pfc->tim[0]->tim_id,pfc->tim[0]->tim_cmp,(uint32_t)(pfc->duty_out*pfc->tim[0]->htimx->PERxR));
    // __HAL_HRTIM_SetCompare(&hhrtim1,pfc->tim[1]->tim_id,pfc->tim[1]->tim_cmp,(uint32_t)(pfc->duty_out*pfc->tim[1]->htimx->PERxR));

}

/**
 * @brief  PFC��ѹ��
 * 
 * @param pfc 
 * @return float ���ص������ֵ
 */
float Single_PFC_VoltageLoop(SinglePFC_Typedef* pfc){

    float pid_out;
    // pfc->BusVol_pid->Ref = pfc->Volt_Ref;
    //������
    Slew_Func(&(pfc->BusVol_pid->Ref),pfc->Volt_Ref,(SINGLE_PFC_SOFT_STARTUP_VAL/SINGLE_PFC_BUS_MAX/pfc->loop_fre*SINGLE_PFC_CV_FRE_RATE));

    pid_out = Pos_PID_Controller(pfc->BusVol_pid,pfc->bus_volt_filter);
    return pid_out;

}

/**
 * @brief  PFCդ�����ƺ���
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

/**
 * @brief  PFC���У���Ҫ��״̬��������
 * 
 * @param pfc 
 */
void Single_PFC_Run(SinglePFC_Typedef* pfc){

    static int tick;

    Single_PFC_GetADCvalue(pfc);
    Single_PFC_PLL(pfc);

    //����״̬�����п���
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
            tick++;
            if(tick == SINGLE_PFC_CV_FRE_RATE){
                pfc->Current_Ref = Single_PFC_VoltageLoop(pfc);
                tick = 0;
            }
            // pfc->Current_Ref = 0.08f;
            Single_PFC_CurrentLoop(pfc,pfc->Current_Ref);
            break;

        case SINGLE_PFC_OLIP:
            Single_PFC_GateControl(pfc,GATE_CLOSE);
            break;

        case SINGLE_PFC_OVP:
            Single_PFC_GateControl(pfc,GATE_CLOSE);
            break;

    }
}

/**
 * @brief ����PFC�����໷
 * 
 * @param pfc 
 * @return true  ����ɹ�
 * @return false δ������ɹ�
 */
bool Single_PFC_PLL(SinglePFC_Typedef* pfc){
    SPLL_1PH_SOGI_run(pfc->PLL,pfc->ac_volt);

    return  (my_abs(pfc->PLL->u_Q[0]) < 0.00001f)?(true):(false);

}

/**
 * @brief ����PFC��ADCУ׼
 * 
 * @param pfc pfc����
 * @return true У׼���
 * @return false У׼δ���
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
 * @brief ����PFC�ı�������������ѹ������ʱ�򣬻�ر����
 * 
 * @param pfc 
 */
void Single_PFC_Protection(SinglePFC_Typedef* pfc){
    static uint8_t OIP_tick = 0;
    static uint8_t OVP_tick = 0;

    OIP_tick = (pfc->L_current < pfc->L_current_Protect) ? OIP_tick + 1  : false;
    OVP_tick = (pfc->bus_volt > pfc->bus_volt_Protect) ? OVP_tick + 1  : false;

    if(OIP_tick > 100)
        pfc->state = SINGLE_PFC_OLIP;
    if(OVP_tick > 100)
        pfc->state = SINGLE_PFC_OVP;
}

/**
 * @brief ��ȡ��������ʵֵ
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

    //��ͨ�˲� ����Ƶ�� 5K
    pfc->ac_volt_filter += 1.1549972f*(pfc->ac_volt - pfc->ac_volt_filter);
    pfc->bus_volt_filter += 0.11549972f*(pfc->bus_volt - pfc->bus_volt_filter);

    //�ݲ���
    pfc->bus_volt_filter = DCL_runDF22_C4(pfc->Bus_Volt_Notch,pfc->bus_volt_filter);

}

