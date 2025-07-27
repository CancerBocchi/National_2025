#include "task.h"




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
  
  //
  //init alg
  //
  Single_PFC_ob_Init();

  

  //
  //Human machine interact init
  //
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
    memset(ScreenBuffer,0,1024); //Çå¿Õ»º³åÇø
    
    current_page->update_ui();
    
    flash();
    oled_tick = HAL_GetTick();

  }

}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc){

  static int tick = 0;

  if(hadc == &hadc2){

    Single_PFC_Run(&PFC);


    tick++;
    if(tick == 1){
      print_f = 1;
      tick = 0;
    }


  }
}

