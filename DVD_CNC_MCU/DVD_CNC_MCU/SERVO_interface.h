/*
 * SERVO_interface.h
 *
 * Created: 9/9/2021 6:34:21 PM
 *  Author: MohammedGamalEleish
 */


#ifndef SERVO_INTERFACE_H_
#define SERVO_INTERFACE_H_


typedef struct{
	u8 Pin;
	f32 PWMFrequency;
	void (*ChangeDutyCycle)(f32); //user does not use it. can I make it private?
}SERVO_t;

void HSERVO_voidInit(SERVO_t* S);

void HSERVO_voidGoToAngle(SERVO_t* S, s16 Angle);

#endif /* SERVO_INTERFACE_H_ */
