/*
 * servo.c
 *
 *  Created on: Apr 2, 2025
 *      Author: jmk
 *
 *      Code for servomotors.
 *      This segment has been ostensibly well defined at the behest of our lord and saviour, ChatGPT.
 *      To forego the unpleasantries of a laborious switch-case construct, a dynamic function handle array is summoned upon.
 *      Here, each element poised to engage with the identifier of the disruptor.
 *      This, in turn, instigates a precisely ordained function to act in accordance with the inscrutable will of the Creator.
 *      There exists no permutation more optimal.
 *      Peak efficiency has been achieved.
 *      Do not entertain doubt.
 *      It is good.
 */

#include <customMain.h>
#include <servos.h>
#include "stdlib.h"

void ReadyFunc(servo_t* s) {
	s->readyFlag = !HAL_GPIO_ReadPin(s->readyPin.port, s->readyPin.pin);
}

void TreachFunc(servo_t* s) {
	s->TreachFlag = !HAL_GPIO_ReadPin(s->TreachPin.port, s->TreachPin.pin);
}

void EncZFunc(servo_t* s) {
	uint32_t encoder_count = __HAL_TIM_GET_COUNTER(s->encoder);
	uint32_t countDifference = encoder_count - s->lastZ;
	if ((abs(countDifference) < 5) | (abs(abs(countDifference)- 10000) < 5)) {
		// Pretty much fine
	} else {
		// Encoder Error handling
	}
}

// Struct instances for EXTI-related events
servo_t servo[] = {
    {0, {GPIOD, S1_Ready_Pin}, 0, {GPIOD, S1_Treach_Pin}, 0, {GPIOG, S1_Enable_Pin}, 0, {GPIOG, S1_Direction_Pin}, 0, &htim14, TIM_CHANNEL_1, NULL, NULL, 0, 0}
	//{0, {GPIOA, S2_Ready_Pin}, 0, {GPIOA, S2_Treach_Pin}, 0, {GPIOD, S2_Enable_Pin}, 0, {GPIOC, S2_Direction_Pin}, 0, &htim12, TIM_CHANNEL_1, HRTIM_TIMERINDEX_TIMER_B, &htim2, 0, 0},
	//{0, {GPIOB, S3_Ready_Pin}, 0, {GPIOD, S3_Treach_Pin}, 0, {GPIOD, S3_Enable_Pin}, 0, {GPIOD, S3_Direction_Pin}, 0, &htim14, TIM_CHANNEL_1, HRTIM_TIMERINDEX_TIMER_A, &htim4, 0, 0},
	//{0, {GPIOE, S4_Ready_Pin}, 0, {GPIOB, S4_Treach_Pin}, 0, {GPIOE, S4_Enable_Pin}, 0, {GPIOE, S4_Direction_Pin}, 0, &htim13, TIM_CHANNEL_1, 0, &htim1, 0, 0}
};

// Individual handler functions that call the struct update function
void Handle_EXTI0(void)  { ReadyFunc(&servo[0]); }
void Handle_EXTI1(void)  { TreachFunc(&servo[0]); }
void Handle_EXTI2(void)  { btnCallbackB(); }
void Handle_EXTI3(void) { btnCallbackEnc(); }
void Handle_EXTI4(void)  { ReadyFunc(&servo[0]); }
void Handle_EXTI5(void)  { btnCallbackEnc(); }
void Handle_EXTI7(void)  { ReadyFunc(&servo[3]); }
void Handle_EXTI8(void) { ReadyFunc(&servo[1]); }
void Handle_EXTI9(void)  { TreachFunc(&servo[1]); }
void Handle_EXTI10(void)  { TreachFunc(&servo[2]); }
void Handle_EXTI11(void)  { EncZFunc(&servo[3]); }
void Handle_EXTI12(void) { EncZFunc(&servo[1]); }
void Handle_EXTI13(void)  { TreachFunc(&servo[3]); }
void Handle_EXTI14(void)  { EncZFunc(&servo[2]); }
void Handle_EXTI15(void) { ReadyFunc(&servo[2]); }

// Lookup table indexed by pin number (NULL for unused pins)
typedef void (*ExtiHandler)(void);
ExtiHandler extiHandlers[] = {
	Handle_EXTI0, Handle_EXTI1, Handle_EXTI2, Handle_EXTI3, Handle_EXTI4, Handle_EXTI5, NULL, Handle_EXTI7, Handle_EXTI8,
	Handle_EXTI9, Handle_EXTI10, Handle_EXTI11, Handle_EXTI12, Handle_EXTI13, Handle_EXTI14, Handle_EXTI15
};

// EXTI Callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint8_t pinNumber = __builtin_ctz(GPIO_Pin); // Get bit position of the pin
    if (pinNumber < sizeof(extiHandlers) / sizeof(ExtiHandler) && extiHandlers[pinNumber]) {
        extiHandlers[pinNumber](); // Call the corresponding handler
    }
}
