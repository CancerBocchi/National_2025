//
// Created by ZheWana on 2024/10/21.
//

#include "key_matrix.h"

gpioPin_t inputPins[NUM_OF_KEY_IN] = {
        KEY_ROW0_Pin, KEY_ROW1_Pin, KEY_ROW2_Pin,
        KEY_ROW3_Pin};

gpioPort_t inputPorts[NUM_OF_KEY_IN] = {
        KEY_ROW0_GPIO_Port, KEY_ROW1_GPIO_Port, KEY_ROW2_GPIO_Port,
        KEY_ROW3_GPIO_Port};

gpioPin_t outputPins[NUM_OF_KEY_OUT] = {
        KEY_COL0_Pin, KEY_COL1_Pin,
        KEY_COL2_Pin, KEY_COL3_Pin
    };

gpioPort_t outputPorts[NUM_OF_KEY_OUT] = {
        KEY_COL0_GPIO_Port, KEY_COL1_GPIO_Port,
        KEY_COL2_GPIO_Port, KEY_COL3_GPIO_Port
        };

void KM_GPIO_Output(gpioPort_t Port, gpioPin_t Pin, int state) {
    HAL_GPIO_WritePin(Port, Pin, state);
}

int KM_GPIO_Input(gpioPort_t Port, gpioPin_t Pin) {
    return HAL_GPIO_ReadPin(Port, Pin);
}



uint32_t KM_GetKeyValue() {
    uint32_t res = 0;

    for (int i = 0; i < NUM_OF_KEY_OUT; i++) {
        // Let the output line be 0.
        for (int j = 0; j < NUM_OF_KEY_OUT; ++j) {
            KM_GPIO_Output(outputPorts[j], outputPins[j], (i == j ? 1 : 0));

        }
        // Check the input line, 1 if the Key is pressed.
        for (int j = 0; j < NUM_OF_KEY_IN; ++j) {
            volatile uint32_t keyVal = KM_GPIO_Input(inputPorts[j], inputPins[j]);
            if (keyVal != 0)
                res |= keyVal << (i * NUM_OF_KEY_IN + j);
            else
                res |= keyVal << (i * NUM_OF_KEY_IN + j);
        }
    }

    return res;
}