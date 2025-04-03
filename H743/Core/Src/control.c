/*
 * control.c
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 *
 *      Functions for servo motor control
 *
 *      setMotorSpeed(rpm) - sets pulse frequency for the desired rpm
 */

#include <customMain.h>
#include "control.h"

int16_t prevRPM = 0;

void setMotorSpeed(int16_t rpm) {
	int32_t ARR;
	int16_t rpmChange = rpm-prevRPM;
	if (abs(rpmChange) > 10) {				// Gradual speed change
		if (rpmChange > 0) {
			rpm = prevRPM + 10;
		} else if (rpmChange < 0) {
			rpm = prevRPM - 10;
		}
	}
	prevRPM = rpm;

	if (rpm == 0) {
		//HAL_GPIO_WritePin(GPIOB,LD3_Pin, GPIO_PIN_RESET);
		HAL_TIM_OC_Stop(&htim3, TIM_CHANNEL_1);
	} else {
		//HAL_GPIO_WritePin(GPIOB,LD3_Pin, GPIO_PIN_SET);
		if (rpm < 0) {
			rpm = - rpm;
			//HAL_GPIO_WritePin(Dir_GPIO_Port, Dir_Pin, GPIO_PIN_SET);
		} else {
			//HAL_GPIO_WritePin(Dir_GPIO_Port, Dir_Pin, GPIO_PIN_RESET);
		}
		ARR = 42000000/((rpm*25)/3);
		if (ARR > 65535) {			// Max ARR (min speed)
			ARR = 65535;
		} else if (ARR < 1680) {	// Min ARR (max speed)
			ARR = 1680;
		}
		__HAL_TIM_SET_AUTORELOAD(&htim3, ARR);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,ARR/2);
		HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);
		//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	}

}
