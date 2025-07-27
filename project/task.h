#ifndef __TASK_H__
#define __TASK_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "gpio.h"
#include "hrtim.h"
#include "adc.h"
#include "spi.h"

#include "PID.h"
#include "power_meas_sine_analyzer.h"
#include "spll_1ph_sogi.h"
#include "SPWM.h"
#include "CalCoeff.h"
#include "arm_math.h"

#include "oled.h"
#include "OLEDUI.h"
#include "comKey.h"
#include "encoder.h"

#include "page.h"
#include "page_setting.h"

#include "single_pfc.h"

#include "SEGGER_RTT.h"

#include "single_pfc_ob.h"


void task_init();
void task_run();



#endif // !__TASK_H__