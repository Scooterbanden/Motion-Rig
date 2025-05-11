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

// Test times [ms]
int32_t stepTestTime = 5000;
int32_t seqTestTime = 200000;

float freqScale;
float rpmAmplitude;
float freqInitial = 0.001;

uint32_t controlCounter = 0;
ControlMode controlMode = IDLE;
uint16_t stepRef = 0;

void controlInit(void) {
	HAL_TIM_Base_Start_IT(&htim17);
}

// 1kHz control loop
void controlLoop(void) {
	int32_t servoEnc;
	int32_t servoCount;
	switch (controlMode) {
	case IDLE:
		break;
	case GAME:
		break;
	case SEQUENCE:
		float freq = (float)(controlCounter) *freqScale + freqInitial;
		float rpm = rpmAmplitude*sin(2*3.14*((freq-freqInitial)*freq));
		setMotorSpeed((int16_t)(rpm), &servo[0]);
		servoEnc = get_servo_position(&servo[0]);
		servoCount = get_servo_position(&servo[2]);
		//error = servoCount*20 - servoEnc;
<<<<<<< HEAD
		send_int32_uart(servoCount*20,servoEnc);
=======
		send_int32_uart(servoCount,servoEnc);
>>>>>>> 95061b66cf3de183b78b99d8b6c8d2de6fac6fe5
		controlCounter++;
		if (controlCounter >= seqTestTime) {
			setMotorSpeed(0, &servo[0]);
			controlCounter = 0;
			controlMode = CONSTSPEED;
		}
		break;
	case SPEED:
		int32_t encoderValue = __HAL_TIM_GET_COUNTER(&htim5);
		setMotorSpeed((int16_t)(encoderValue*ENCRPMGAIN), &servo[0]);
		break;
	case STEP:
		setMotorSpeed(stepRef,&servo[0]);
		servoEnc = get_servo_position(&servo[0]);
		servoCount = get_servo_position(&servo[2]);
		//error = servoCount*20 - servoEnc;
		send_int32_uart(servoCount,servoEnc);
		controlCounter++;
		if (controlCounter >= stepTestTime) {
			setMotorSpeed(0, &servo[0]);
			controlCounter = 0;
			controlMode = REALIGN;
		}
		break;
	case REALIGN:
		if (controlCounter == 1000) {
			servoEnc = get_servo_position(&servo[0]);
			int32_t offset = 10000 - (servoEnc%10000);
			__HAL_TIM_SET_COUNTER(&htim4,0);
			servo[2].position = 0;
			__HAL_TIM_SET_AUTORELOAD(&htim4, offset/20);
		} else if (controlCounter > 1000) {
			setMotorSpeed(50,&servo[0]);
		}
		controlCounter++;
		break;
	case CONSTSPEED:
		if (controlCounter > 1000) {
			setMotorSpeed(200,&servo[0]);
		}
		controlCounter++;
		if (controlCounter > 6000) {
			setMotorSpeed(0, &servo[0]);
			controlCounter = 0;
			controlMode = REALIGN;
		}
		break;
	}
}

void setMotorSpeed(int16_t rpm, servo_t* servo) {

	if (rpm == 0) {
		HAL_TIM_OC_Stop(servo->pulseTimerGP, servo->TIM_CH_GP);
		return;
	}
	int32_t ARR;
	int32_t freqTarget;
	GPIO_PinState direction = GPIO_PIN_RESET;

	if (rpm < 0) {
		direction = GPIO_PIN_SET;
		rpm = abs(rpm);
		htim4.Instance->CR1 |=  TIM_CR1_DIR;
	} else {
		htim4.Instance->CR1 &= ~TIM_CR1_DIR;
	}
	if (rpm > 3000) { rpm = 3000; }
	if (rpm < 50) { rpm = 50; }

	freqTarget = rpm*RPM2FREQ;
	ARR = CLOCKFREQ/freqTarget;
	if (ARR > 65535) {			// Max ARR (min speed)
		ARR = 65535;
	} else if (ARR < 1) {	// Min ARR (max speed)
		ARR = 1;
	}
	HAL_TIM_OC_Stop(servo->pulseTimerGP, servo->TIM_CH_GP);
	setGPIO(servo->directionPin, direction);
	__HAL_TIM_SET_AUTORELOAD(servo->pulseTimerGP, ARR);
	__HAL_TIM_SET_COMPARE(servo->pulseTimerGP, servo->TIM_CH_GP,ARR/2);
	HAL_TIM_OC_Start(servo->pulseTimerGP, servo->TIM_CH_GP);

}

void send_int32_uart(int32_t value, int32_t value2) {
	// modified to send two int32_t for both pulse and encoder count
    uint8_t buffer[8];

    // Convert int32_t to 4-byte buffer (little-endian)
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[2] = (uint8_t)((value >> 16) & 0xFF);
    buffer[3] = (uint8_t)((value >> 24) & 0xFF);
    buffer[4] = (uint8_t)(value2 & 0xFF);
    buffer[5] = (uint8_t)((value2 >> 8) & 0xFF);
    buffer[6] = (uint8_t)((value2 >> 16) & 0xFF);
    buffer[7] = (uint8_t)((value2 >> 24) & 0xFF);

    HAL_UART_Transmit(&huart2, buffer, 8, HAL_MAX_DELAY);
}
