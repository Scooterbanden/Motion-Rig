/*
 * control.h
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#define CLOCKFREQ 20000000
#define RPM2FREQ 25/3

extern uint8_t sequenceFlag;
extern uint32_t controlCounter;

void controlInit(void);
void controlLoop(void);
void setMotorSpeed(int16_t rpm, servo_t servo);

#endif /* INC_CONTROL_H_ */
