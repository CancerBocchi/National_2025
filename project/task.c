#include "task.h"

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


Pos_PID_t Current_PI; //电流环路
Pos_PID_t Volt_PI;    //电压环路
SPLL_1PH_SOGI SPLL;   //锁相环
POWER_MEAS_SINE_ANALYZER measure; //功率测量
DCL_DF22 Bus_Notch;   //总线陷波器
DCL_DF22 Current_PR;  //电流PR

SinglePFC_Typedef PFC;


float Slew_Func(float *slewVal, float refVal, float slewRate)
{
  static float diff = 0;
  diff = refVal - *slewVal;
  if(diff >= slewRate){
    *slewVal += slewRate;
    return (1);
  }
  else if (-diff >= slewRate){
    *slewVal -= slewRate;
    return (-1);
  } 
  else{
    *slewVal = refVal;
    return (0);
  }
}

void task_init(){

    //PFC Init
    Pos_PID_Init(&Current_PI,1,0.01,0);
    Current_PI.Output_Max = 0.5;
    Current_PI.Output_Min = -0.5;
    Current_PI.Value_I_Max = 10000;

    Pos_PID_Init(&Volt_PI,1.0f,0.1f,0);
    Volt_PI.Output_Max = 1;
    Volt_PI.Output_Min = 0;
    Volt_PI.Value_I_Max = 5000;

    SinglePFC_Init(&PFC,&hadc1,
                  &Bridge_Driver1,
                  &Bridge_Driver2,
                  &SPLL,
                  50,SINGLE_PFC_PUCOE,SINGLE_PFC_PUCOE,SINGLE_PFC_PUCOE,
                  &Current_PI,&Volt_PI,&Bus_Notch,&Current_PR);
    
    Single_PFC_SetBusVolt(PFC,36.0f);

    POWER_MEAS_SINE_ANALYZER_reset(&measure);
    POWER_MEAS_SINE_ANALYZER_config(&measure,CLOCK_FREQ/(50000.0f)/2,0,55,45);

    PFC.adc_rawoffset_ac  = 0.4908f * 4096;
    PFC.adc_rawoffset_bus = 0;
    PFC.adc_rawoffset_L   = 0.488f * 4096;

    // Single_PFC_ChangePF(&PFC,0.80);

    // Single_PFC_GateControl(&PFC,GATE_OPEN);

    //Human machine interact init
    __HAL_SPI_ENABLE(&hspi1);  
    OLED_Init();

    extern comkey_t matrix_key[16];
    for(int i = 0;i<16;i++)
      ComKey_Init(&matrix_key[i],1);

    route_to(&(page_setting.page.page));

}

int print_f = 0;
int flag = 0;
uint16_t key_val = 0;
void task_run(){

  
  static int key_tick = 0;
  if(!key_tick)
    key_tick = HAL_GetTick();

  if(HAL_GetTick() - key_tick >= 1){

    key_val = KM_GetKeyValue();
    ComKey_Handler();
    key_tick = HAL_GetTick();

  }

  if(print_f){
    print_f = 0;
    // SEGGER_RTT_printf(0,"%d\n",key_val);
  }



  static int led_tick = 0;
  if(!led_tick)
    led_tick = HAL_GetTick();

  if(HAL_GetTick() - led_tick >= 1){

    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_11);
    led_tick = HAL_GetTick();

  }


  static int oled_tick = 0;
  if(!oled_tick)
    oled_tick = HAL_GetTick();

  if(HAL_GetTick() - oled_tick >= 25){

    extern uint8_t ScreenBuffer[8][128];
    memset(ScreenBuffer,0,1024); //清空缓冲区
    
    current_page->update_ui();
    
    flash();
    oled_tick = HAL_GetTick();

  }

}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc){

  static int tick = 0;

  if(hadc == &hadc2){


    // if(measure.vRms >= 0.10f)
    //   flag = 1;
      
    if(flag)
      Single_PFC_Run(&PFC);
    else{
      Single_PFC_GateControl(&PFC,GATE_CLOSE);
      PFC.state = SINGLE_PFC_PLL;
    }

    measure.i = PFC.L_current;
    measure.v = PFC.ac_volt;
    POWER_MEAS_SINE_ANALYZER_run(&measure);


    tick++;
    if(tick == 1){
      print_f = 1;
      tick = 0;
    }

    // SEGGER_RTT_printf(0,"%d,%d,%d\n",(int)((PFC.duty_out)*1000),(int)(PFC.bus_volt_filter*1000),(int)(PFC.L_current *1000));



  }

  // Single_PFC_GetADCvalue(&PFC);
  // measure.v = PFC.ac_volt;


  // static int flag = 0;
  
  // if(measure.vRms >= 0.10f){
  //   Single_PFC_PLL(&PFC);
  //   if(measure.vRms >= 0.20f || flag == 1){
  //     flag = 1;
  //     static int volt_tick = 0;
  //     volt_tick++;
  //     if(volt_tick == 100){
  //       PFC.Current_Ref = Single_PFC_VoltageLoop(&PFC);
  //       volt_tick = 0;
  //     }
  //     Single_PFC_CurrentLoop(&PFC,PFC.Current_Ref);
  //     Single_PFC_GateControl(&PFC,GATE_OPEN);
  //   }
  //   // else{
  //   //   Single_PFC_GateControl(&PFC,GATE_CLOSE);
  //   // }
  // }
  // else {
  //   Single_PFC_GateControl(&PFC,GATE_CLOSE);
  // }

  
  



}

