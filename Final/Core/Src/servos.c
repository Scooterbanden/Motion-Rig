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

// Added for validation sequence, controlling each servo independently
int activeServos = 0;
int currentServo;

// Struct instances
servo_t servo[] = {
    {false, {GPIOD, S1_Ready_Pin}, false, {GPIOD, S1_Treach_Pin}, false, {GPIOD, S1_Enable_Pin}, {GPIOD, S1_Direction_Pin}, 0, &htim8, TIM_CHANNEL_4, HRTIM_TIMERINDEX_TIMER_C, {&htim3, 0, {GPIOD, S1_EncZ_Pin}, 0, 0}, false, {&htim5, 0, 0}},
	{false, {GPIOA, S2_Ready_Pin}, false, {GPIOA, S2_Treach_Pin}, false, {GPIOD, S2_Enable_Pin}, {GPIOC, S2_Direction_Pin}, 0, &htim15, TIM_CHANNEL_2, HRTIM_TIMERINDEX_TIMER_B, {&htim2, 0, {GPIOC, S2_EncZ_Pin}, 0, 0}, false, {&htim1, 0, 0}},
	{false, {GPIOB, S3_Ready_Pin}, false, {GPIOD, S3_Treach_Pin}, false, {GPIOD, S3_Enable_Pin}, {GPIOD, S3_Direction_Pin}, 0, &htim14, TIM_CHANNEL_1, HRTIM_TIMERINDEX_TIMER_A, {&htim4, 0, {GPIOD, S3_EncZ_Pin}, 0, 0}, false, {NULL, 0, 0}},
	{false, {GPIOE, S4_Ready_Pin}, false, {GPIOB, S4_Treach_Pin}, false, {GPIOE, S4_Enable_Pin}, {GPIOE, S4_Direction_Pin}, 0, &htim13, TIM_CHANNEL_1, NULL, {&htim1, 0, {GPIOB, S4_EncZ_Pin}, 0, 0}, false, NULL}
};

void ReadyFunc(servo_t* s) {
	//s->readyFlag = !HAL_GPIO_ReadPin(s->readyPin.port, s->readyPin.pin);
}

void TreachFunc(servo_t* s) {
	s->TreachFlag = true;
	HAL_TIM_OC_Stop(s->pulseTimerGP, s->TIM_CH_GP);
	//HAL_GPIO_TogglePin(LEDs[8].port, LEDs[8].pin);
}

void EncZFunc(servo_t* s) {
	uint32_t encoder_count = __HAL_TIM_GET_COUNTER(s->encoder.encoder);
	uint32_t countDifference = encoder_count - s->encoder.lastZ;
	if ((abs(countDifference) < 5) | (abs(abs(countDifference)- 10000) < 5)) {
		// Pretty much fine
	} else {
		// Encoder Error handling
	}
}

void servoInit(void) {
	activeServos = 0;
	calibrated = false;

	for (int i = 0; i < 4; i++) {
		if ((getGPIO(servo[i].readyPin) == GPIO_PIN_RESET)) {
			if (!activeServos) {
				currentServo = i;
			}
			activeServos++;
			setGPIO(servo[i].enablePin, GPIO_PIN_SET);
			servo[i].enableFlag = true;

			__HAL_TIM_SET_COUNTER(servo[i].encoder.encoder, 0);
			HAL_TIM_Encoder_Start(servo[i].encoder.encoder, TIM_CHANNEL_ALL);
			if (i != 2) {
				HAL_TIM_Encoder_Start(servo[i].counter.timer, TIM_CHANNEL_ALL);
			} else {
				HAL_TIM_Base_Start_IT(servo[i].pulseTimerGP);
			}
		}
	}
}

int32_t get_servo_position(servo_t* s)
{
    int32_t pos1, pos2;
    uint16_t cnt1, cnt2;
    uint32_t dir1, dir2;

    do {
        pos1 = s->encoder.position;
        cnt1 = __HAL_TIM_GET_COUNTER(s->encoder.encoder);
        dir1 = READ_BIT(s->encoder.encoder->Instance->CR1, TIM_CR1_DIR);
        pos2 = s->encoder.position;
        cnt2 = __HAL_TIM_GET_COUNTER(s->encoder.encoder);
        dir2 = READ_BIT(s->encoder.encoder->Instance->CR1, TIM_CR1_DIR);
    } while (pos1 != pos2 || cnt1 != cnt2 || dir1 != dir2);

    return pos1 + cnt1;
}



int32_t get_sent_pulses(pulseCounter_t* c)
{
	// Race condition resistant position reading
    int32_t pos1, pos2;
    uint16_t cnt1;

    do
    {
        pos1 = c->count;
        cnt1 = __HAL_TIM_GET_COUNTER(c->timer);
        pos2 = c->count;
    } while (pos1 != pos2);

    return pos1 + cnt1;
}


// Individual handler functions that call the struct update function
void Handle_EXTI0(void)  { EncZFunc(&servo[0]); }
void Handle_EXTI1(void)  { btnCallbackA(); }
void Handle_EXTI2(void)  { btnCallbackB(); }
void Handle_EXTI3(void) { TreachFunc(&servo[0]); }
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
