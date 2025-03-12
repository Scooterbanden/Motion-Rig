/*
 * Custom.h
 *
 *  Created on: Mar 9, 2025
 *      Author: jakob
 */

#ifndef INC_CUSTOM_H_
#define INC_CUSTOM_H_

#define BASE 32768
#define RPMLIMIT 3000

void customInit(void);
void customLoop(void);
void setMotorSpeed(int16_t rpm);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void oledCheck(void);

#endif /* INC_CUSTOM_H_ */
