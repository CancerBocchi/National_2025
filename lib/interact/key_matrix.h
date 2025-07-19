//
// Created by ZheWana on 2024/10/21.
//

#ifndef KEY_MATRIX_H
#define KEY_MATRIX_H

#include "gpio.h"

#define NUM_OF_KEY_IN (4)
#define NUM_OF_KEY_OUT (4)

typedef uint16_t gpioPin_t;
typedef GPIO_TypeDef* gpioPort_t;

uint32_t KM_GetKeyValue();

#endif //KEY_MATRIX_H
