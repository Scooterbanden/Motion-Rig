/*
 * Custom.c
 * File separated from code generation
 *  Created on: Mar 9, 2025
 *      Author: jakob
 */

#include "main.h"
#include "stm32f7xx_hal_tim.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "custom.h"

extern I2C_HandleTypeDef hi2c2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

//extern UART_HandleTypeDef huart2;

extern const unsigned char garfield_128x64[];// Very important. Prints on oled at initialization

// UART communication buffers
uint8_t RxUART[256];
uint8_t tempData[2];
int bufIdx = 0;

uint8_t oledStatus = 0;				// Used to prevent oled use if not connected
uint32_t timer;						// timer for main loop
uint32_t btnTimer;
uint8_t controlMode = 0;			// ControlMode selection. 0: Off, 1: Position, 2: Speed
uint8_t ledSelect = 0;				// Just iteratively toggles 3 different LEDs
int16_t prevRPM = 0;
uint8_t slowdown = 0;				// Set true if control mode is switched from speed, slows down calmly (maybe)

void customInit(void) {
	//HAL_UART_Receive_IT(&huart2, tempData, 1);
	__HAL_TIM_SET_COUNTER(&htim4, BASE);				// Encoder starts in middle and above/below middle is positive/negative speed
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);		// User encoder
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);		// Motor encoder
	HAL_TIM_PWM_Init(&htim3);							// Pulse output
	//__HAL_RCC_TIM3_CLK_ENABLE();
	//HAL_TIM_Base_Start(&htim3);
	oledCheck();
	timer = HAL_GetTick();
	btnTimer = HAL_GetTick();
}

void customLoop(void) {
	while (1) {
		/*
		if (tempData[0] == '\n') {
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2);
			// Full message received from interface, call to interpret
			char cmd[bufIdx];
			memcpy(cmd, RxUART, bufIdx);
			//Parse_Command(cmd);

			tempData[0] = 'c';		// Change tempData from \n
			bufIdx = 0;
		}*/

		if (HAL_GetTick()-timer > 500) {
			/*
			switch (ledSelect) {
			case 0:
				HAL_GPIO_TogglePin(GPIOB,LD1_Pin);
				ledSelect = 1;
				break;
			case 1:
				HAL_GPIO_TogglePin(GPIOB,LD2_Pin);
				ledSelect = 2;
				break;
			case 2:
				HAL_GPIO_TogglePin(GPIOB,LD3_Pin);
				ledSelect = 0;
				break;
			}
			*/
			int32_t encoder_count = __HAL_TIM_GET_COUNTER(&htim4);
			int32_t motorEncoder_count = __HAL_TIM_GET_COUNTER(&htim1);
			if (oledStatus) {
				ssd1306_Fill(Black);
				ssd1306_SetCursor(2, 0);
			}
			//HAL_GPIO_WritePin(GPIOB,LD1_Pin, GPIO_PIN_SET);
			switch (controlMode) {
			case 0:
				break;
			case 1:		// Position mode
				if (prevRPM == 0 && HAL_GPIO_ReadPin(SigIn1_GPIO_Port, SigIn1_Pin) == GPIO_PIN_RESET) {
					HAL_GPIO_WritePin(SigIn1_GPIO_Port, SigIn1_Pin, GPIO_PIN_SET);		// Disable motor
					HAL_GPIO_WritePin(GPIOB,LD1_Pin, GPIO_PIN_RESET);
				}
				setMotorSpeed(0);
				break;
			case 2:		// Speed mode
				if (HAL_GPIO_ReadPin(SigIn1_GPIO_Port, SigIn1_Pin) == GPIO_PIN_SET) {
					HAL_GPIO_WritePin(SigIn1_GPIO_Port, SigIn1_Pin, GPIO_PIN_RESET);	// Enable motor
					HAL_GPIO_WritePin(GPIOB,LD1_Pin, GPIO_PIN_SET);
				}

				int16_t rpm = (int16_t)(encoder_count - BASE);
				if (rpm < -RPMLIMIT) {
					__HAL_TIM_SET_COUNTER(&htim4, BASE-RPMLIMIT);
					rpm = -RPMLIMIT;
				} else if (rpm > RPMLIMIT) {
					__HAL_TIM_SET_COUNTER(&htim4, BASE+RPMLIMIT);
					rpm = RPMLIMIT;
				}
				if (HAL_GPIO_ReadPin(SigOut1_GPIO_Port, SigOut1_Pin) == GPIO_PIN_RESET) {	// Check if motor ready
					HAL_GPIO_WritePin(GPIOB,LD2_Pin, GPIO_PIN_SET);
					setMotorSpeed(rpm);
				} else {
					HAL_GPIO_WritePin(GPIOB,LD2_Pin, GPIO_PIN_RESET);
					setMotorSpeed(0);
				}

				if (oledStatus) {
					int len = snprintf(NULL, 0, "Speed[rpm]:");
					char *line1 = malloc(len + 1);
					snprintf(line1, len + 1, "Speed[rpm]:");
					ssd1306_WriteString(line1, Font_11x18, White);
					free(line1);

					ssd1306_SetCursor(2, 20);
					len = snprintf(NULL, 0, "%d",rpm);
					char *line2 = malloc(len + 1);
					snprintf(line2, len + 1, "%d",rpm);
					ssd1306_WriteString(line2, Font_11x18, White);
					free(line2);

					ssd1306_SetCursor(2, 40);
					len = snprintf(NULL, 0, "Pos: %ld",motorEncoder_count);
					char *line3 = malloc(len + 1);
					snprintf(line3, len + 1, "Pos: %ld",motorEncoder_count);
					ssd1306_WriteString(line3, Font_11x18, White);
					free(line3);

					ssd1306_UpdateScreen();
				}

				break;
			}
			timer = HAL_GetTick();
		}
	}
}

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
		HAL_GPIO_WritePin(GPIOB,LD3_Pin, GPIO_PIN_RESET);
		HAL_TIM_OC_Stop(&htim3, TIM_CHANNEL_1);
	} else {
		HAL_GPIO_WritePin(GPIOB,LD3_Pin, GPIO_PIN_SET);
		if (rpm < 0) {
			rpm = - rpm;
			HAL_GPIO_WritePin(Dir_GPIO_Port, Dir_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(Dir_GPIO_Port, Dir_Pin, GPIO_PIN_RESET);
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

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Receive UART data
	if (huart == &huart2) {
		memcpy(RxUART+bufIdx, tempData, 1);
		if (++bufIdx >= 256) bufIdx = 0;
		HAL_UART_Receive_IT(&huart2, tempData, 1);
	}
}*/

void oledCheck(void) {		// To check if an oled is present
	if (HAL_I2C_IsDeviceReady(&hi2c2, SSD1306_I2C_ADDR, 1, 10) == HAL_OK) {
		if (!oledStatus) {	// Initializes if oled has been offline
			HAL_Delay(200);
			ssd1306_Init();
			ssd1306_Fill(White);
			ssd1306_DrawBitmap(0,0,garfield_128x64,128,64,Black);
			ssd1306_UpdateScreen();
		}
		oledStatus = 1;
	} else {
		oledStatus = 0;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_3) // Check if it's the button
    {
    	if (HAL_GetTick() - btnTimer > 200) {
    		if (controlMode < 2) {
    			controlMode = 2;
    			slowdown = 0;
    		} else {					// Switch to position control (currently not implemented, just slows down
    			controlMode = 1;
    			slowdown = 1;
    			__HAL_TIM_SET_COUNTER(&htim4, BASE);
    		}
    		btnTimer = HAL_GetTick();
    	}
    }
}


