/*
 * control.h
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#define CLOCKFREQ 20000000
#define RPM2FREQ ((float)25/3)
#define TCONTROL ((float)1/1000)
#define CALIBRATIONSPEED 100
#define UPPERLIMIT ((int32_t)STROKE_T*0.9*MM2PULSE)
#define LOWERLIMIT ((int32_t)STROKE_T*0.1*MM2PULSE)

typedef enum {
	IDLE = 0U,
	SEQUENCE,
	SPEED,
	STEP,
	GAME,
	CONSTSPEED,
	REALIGN,
	CALIBRATE,
	PARK,
	VALIDATION
} ControlMode;

typedef enum {
	INDIVIDUAL_CONSTANT = 0U,
	INDIVIDUAL_VARYING,
	ALL_CONSTANT,
	ALL_VARYING
} ValidationMode;

extern bool calibrated;
extern float freqScale;
extern float rpmAmplitude;
extern float freqInitial;

extern uint16_t stepRef;
extern uint32_t controlCounter;
extern ValidationMode validationMode;
extern ControlMode controlMode;

void controlInit(void);
void controlLoop(void);
float shapedSpeed(uint32_t k, double mu, double sigma, double maxSpeed);
bool nextSpeed(void);
bool limitCheck(servo_t servo, float rpmSP);
void setMotorSpeed(int16_t rpm, servo_t* servo);
void send_int32_uart(int32_t value, int32_t value2);

#endif /* INC_CONTROL_H_ */
