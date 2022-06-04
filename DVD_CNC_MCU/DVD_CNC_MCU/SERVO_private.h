/*
 * SERVO_private.h
 *
 * Created: 16/09/2021 11:02:11 م
 *  Author: Ali Emad
 */ 


#ifndef SERVO_PRIVATE_H_
#define SERVO_PRIVATE_H_

#define PSERVO_TIM0_MODE_FPWM 3
#define PSERVO_TIM0_FPWM_NON_INVERT 2
#define PSERVO_TIM2_MODE_FPWM 3
#define PSERVO_TIM2_FPWM_NON_INVERT 2
#define PSERVO_TIM1_MODE_FPWM10bit 7
#define PSERVO_TIM1_FPWM_NONINVERT 2



#define TCCR0_register ( *(volatile u8 *) 0x53 )
#define OCR0_register  ( *(volatile u8 *) 0x5C )
#define WGM01_bit 3
#define WGM00_bit 6

#define OCR1A_register  ( *(volatile u16 *) 0x4A )
#define OCR1B_register  ( *(volatile u16 *) 0x48 )
#define TCCR1A_register ( *(volatile u8  *) 0x4F )
#define TCCR1B_register ( *(volatile u8  *) 0x4E )

#define OCR2_register  ( *(volatile u8 *) 0x43 )
#define TCCR2_register ( *(volatile u8 *) 0x45 )
#define WGM21_bit 3
#define WGM20_bit 6



void PSERVO_TIM0_voidSetDutyCycle(f32 DutyCycle);

void PSERVO_TIM1ChA_voidSetDutyCycle(f32 DutyCycle);

void PSERVO_TIM1ChB_voidSetDutyCycle(f32 DutyCycle);

void PSERVO_TIM2_voidSetDutyCycle(f32 DutyCycle);

#endif /* SERVO_PRIVATE_H_ */




