/*
 * SERVO_program.c
 *
 * Created: 16/09/2021 05:01:26 م
 *  Author: Ali Emad
 */ 
#include "STD_TYPES.h"
#include "manipPIN_interface.h"
#include "CLOCK_interface.h"

#include "SERVO_interface.h"
#include "SERVO_private.h"


void HSERVO_voidInit(SERVO_t* S)
{
	const u16 TIM0_1_prescalers [5] = {1, 8, 64, 256, 1024};
	const u16 TIM2_prescalers [7] = {1, 8, 32, 64, 128, 256, 1024};
		
	u16 desired_prescaler;
	u8 prescaler;
	
	if(S->Pin == B3) //timer0
	{
		desired_prescaler = MCLOCK_u32GetSystemClock() / ( 256UL * S->PWMFrequency );
		//finding proper prescaler :
		for(prescaler=0; prescaler<5; prescaler++)
			if (TIM0_1_prescalers[prescaler] > desired_prescaler)
				break;
				
		if(prescaler == 5) //the wanted frequency can't be obtained 
			return;
		
		//initializing timer :
		TCCR0_register = prescaler+1;
		TCCR0_register |= (PSERVO_TIM0_MODE_FPWM&1) << WGM00_bit ;
		TCCR0_register |= (PSERVO_TIM0_MODE_FPWM&2)/2 << WGM01_bit ;
		TCCR0_register |= PSERVO_TIM0_FPWM_NON_INVERT << 4 ;
		
		//Actual frequency:
		S->PWMFrequency = (f32)MCLOCK_u32GetSystemClock() / (f32)(256UL * TIM0_1_prescalers[prescaler]);
		
		//Assigning ChangeDutyCycle function :
		S->ChangeDutyCycle = PSERVO_TIM0_voidSetDutyCycle;
	}
	
	else if (S->Pin == D4 || S->Pin == D5)
	{
		desired_prescaler = MCLOCK_u32GetSystemClock() / ( 1024 * S->PWMFrequency );
		//finding proper prescaler :
		for(prescaler=0; prescaler<5; prescaler++)
			if (TIM0_1_prescalers[prescaler] > desired_prescaler)
				break;
		
		if(prescaler == 5) //the wanted frequency can't be obtained
			return;
		
		if(S->Pin == D5) //OC1A :
		{
			//initializing timer :
			TCCR1A_register = (PSERVO_TIM1_MODE_FPWM10bit&3) | (PSERVO_TIM1_FPWM_NONINVERT<<6) | (PSERVO_TIM1_FPWM_NONINVERT<<4);
			TCCR1B_register = (prescaler+1) | ( (PSERVO_TIM1_MODE_FPWM10bit&12)<<1 );
			//Assigning ChangeDutyCycle function :
			S->ChangeDutyCycle = PSERVO_TIM1ChA_voidSetDutyCycle;
		}
		else //if (S->Pin == D4) //OC1B
		{
			//initializing timer :
			TCCR1A_register = (PSERVO_TIM1_MODE_FPWM10bit&3) | (PSERVO_TIM1_FPWM_NONINVERT<<4);
			TCCR1B_register = (prescaler+1) | ( (PSERVO_TIM1_MODE_FPWM10bit & 12)<<1 );
			//Assigning ChangeDutyCycle function :
			S->ChangeDutyCycle = PSERVO_TIM1ChA_voidSetDutyCycle;
		}
		
		//Actual frequency:
		S->PWMFrequency = (f32)MCLOCK_u32GetSystemClock() / (f32)(1024UL * TIM0_1_prescalers[prescaler]);
	}
	
	
	else if(S->Pin == D7) //timer2
	{
		desired_prescaler = MCLOCK_u32GetSystemClock() / ( 256UL * S->PWMFrequency );
		//finding proper prescaler :
		for(prescaler=0; prescaler<7; prescaler++)
			if (TIM2_prescalers[prescaler] > desired_prescaler)
				break;
		
		if(prescaler == 7) //the wanted frequency can't be obtained
			return;

		//initializing timer :
		TCCR2_register = prescaler+1;
		TCCR2_register |= (PSERVO_TIM2_MODE_FPWM&1) << WGM20_bit ;
		TCCR2_register |= (PSERVO_TIM2_MODE_FPWM&2)/2 << WGM21_bit ;
		TCCR2_register |= PSERVO_TIM2_FPWM_NON_INVERT << 4 ;
		
		//Actual frequency:
		S->PWMFrequency = (f32)MCLOCK_u32GetSystemClock() / (f32)(256UL * TIM2_prescalers[prescaler]);
		
		//Assigning ChangeDutyCycle function :
		S->ChangeDutyCycle = PSERVO_TIM2_voidSetDutyCycle;
	}
	
	else //the S->Pin is not connected to any of the 3 timers.
		return;
		
	init_PIN(S->Pin, OUTPUT);
}

void HSERVO_voidGoToAngle(SERVO_t* S, s16 Angle)
{
	//if(Angle<0 || Angle>180)
		//return;
	
	f32 DutyCycle = ( 1.45f + (f32)Angle / 180.0f ) * S->PWMFrequency / 1000.0f;
	
	S->ChangeDutyCycle(DutyCycle);
}


void PSERVO_TIM0_voidSetDutyCycle(f32 DutyCycle)
{
	OCR0_register = (u8)(DutyCycle * 255.0f);
}

void PSERVO_TIM1ChA_voidSetDutyCycle(f32 DutyCycle)
{
	OCR1A_register = (u16)(DutyCycle * 1023.0f);
}

void PSERVO_TIM1ChB_voidSetDutyCycle(f32 DutyCycle)
{
	OCR1B_register = (u16)(DutyCycle * 1023.0f);
}

void PSERVO_TIM2_voidSetDutyCycle(f32 DutyCycle)
{
	OCR2_register = (u8)(DutyCycle * 255.0f);
}

