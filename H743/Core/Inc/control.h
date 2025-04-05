/*
 * control.h
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#define CLOCKFREQ 1500000
#define RPM2FREQ 25/3

extern uint32_t pc;
extern uint32_t startTime;

void setMotorSpeed(int16_t rpm, servo_t servo);

#endif /* INC_CONTROL_H_ */
