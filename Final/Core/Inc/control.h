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

typedef enum {
	IDLE = 0U,
	SEQUENCE,
	SPEED,
	STEP,
	GAME,
	CONSTSPEED,
	REALIGN
} ControlMode;

extern float freqScale;
extern float rpmAmplitude;
extern float freqInitial;

extern uint16_t stepRef;
extern uint32_t controlCounter;
extern ControlMode controlMode;

void controlInit(void);
void controlLoop(void);
void setMotorSpeed(int16_t rpm, servo_t* servo);
void send_int32_uart(int32_t value, int32_t value2);

#endif /* INC_CONTROL_H_ */
