/*
 * Stepper_motor.c
 *
 * Created: 23/01/2022 07:34:41 م
 * Author : Ali Emad
 */ 
#include "BIT_MATH.h"
#include "STD_TYPES.h"
#include "manipPIN_interface.h"
#include "TIM_interface.h"
#include "CLOCK_interface.h"
#include "Stepper_interface.h"
#include<stdlib.h>

void HStepper_init(Stepper_t* stepper, enum HStepper_step_size stepSize, u16 steps_per_rev, u8 pin1, u8 pin2, u8 pin3, u8 pin4, u8 lim_pin, enum HStepper_lim_switch_pull_state lim_pull_state)
{
	stepper->step_size = stepSize ;
	stepper->steps_total = steps_per_rev ;
	
	stepper->pins[0] = pin1 ;
	stepper->pins[1] = pin2 ;
	stepper->pins[2] = pin3 ;
	stepper->pins[3] = pin4 ;
	for (u8 i=0; i<4; i++)
		init_PIN(stepper->pins[i], OUTPUT) ;
		
	stepper->lim_switch_pin = lim_pin ;
	stepper->lim_switch_pull_state = lim_pull_state ;
	init_PIN(lim_pin, INPUT) ;
	set_PIN(lim_pin, lim_pull_state) ;
		
	stepper->last_time_stamp = 0 ;
	MTIM1_voidInit(TIM1_CS_DIV_64, TIM1_MODE_NORMAL, TIM1_Ch_DIS, TIM1_Ch_DIS, 0) ;
	MTIM1_voidStartTickMeasure() ;
}

void HStepper_set_speed(Stepper_t* stepper, u16 speed)
{
	stepper->ticks_delay_between_steps = 60L * MCLOCK_u32GetSystemClock() / stepper->steps_total / speed / MTIM1_u16GetDivision() ; //check the factor!
	// remember that :
	// time of one timer tick = N / f_cpu
	// time of whole OVF = (1+top) * N / f_cpu
}

void HStepper_step(Stepper_t* stepper, s16 steps)
{
	s16 steps_done = 0 ;
	u8 direction = (steps>0) ? 1 : 0 ;
	while (steps_done < abs(steps))
	{
		// here you should use the timestamp because the steps may occur one after another, each in individual call.
		u32 current_ticks = MTIM1_u32GetElapsedTicks() ;
		if (current_ticks - stepper->last_time_stamp   >=   stepper->ticks_delay_between_steps)
		{
			stepper->last_time_stamp = current_ticks ;
			if (direction == 1)
			{
				stepper->current_step_number++ ;
				if (stepper->current_step_number == stepper->steps_total)
					stepper->current_step_number = 0 ;
			}
			else //if (direction==0)
			{
				if (stepper->current_step_number == 0)
					stepper->current_step_number = stepper->steps_total ;
				stepper->current_step_number-- ;
			}
			HStepper_step_from_seq(stepper, stepper->current_step_number % stepper->step_size) ;
			steps_done++ ;
		}
	}
}

void HStepper_step_from_seq(Stepper_t* stepper, u8 n)
{
	if (stepper->step_size == HStepper_full_step)
		switch(n)
		{
			case 0 : //1010
				set_PIN(stepper->pins[0], HIGH);
				set_PIN(stepper->pins[1], LOW);
				set_PIN(stepper->pins[2], HIGH);
				set_PIN(stepper->pins[3], LOW);
				break;
			case 1 : //0110
				set_PIN(stepper->pins[0], LOW);
				set_PIN(stepper->pins[1], HIGH);
				set_PIN(stepper->pins[2], HIGH);
				set_PIN(stepper->pins[3], LOW);
				break;
			case 2 : //0101
				set_PIN(stepper->pins[0], LOW);
				set_PIN(stepper->pins[1], HIGH);
				set_PIN(stepper->pins[2], LOW);
				set_PIN(stepper->pins[3], HIGH);
				break;
			case 3 : //1001
				set_PIN(stepper->pins[0], HIGH);
				set_PIN(stepper->pins[1], LOW);
				set_PIN(stepper->pins[2], LOW);
				set_PIN(stepper->pins[3], HIGH);
				break;
		}
		
	else if (stepper->step_size == HStepper_half_step)
		//10 00		0
		//10 10		1
		//00 10		2
		//01 10		3
		//01 00		4
		//01 01		5
		//00 01		6
		//10 01		7
		switch (n)
		{
			case 0 : // 1000
				set_PIN(stepper->pins[0], HIGH);
				set_PIN(stepper->pins[1], LOW);
				set_PIN(stepper->pins[2], LOW);
				set_PIN(stepper->pins[3], LOW);
				break;
			case 1 : // 1010
				set_PIN(stepper->pins[0], HIGH);
				set_PIN(stepper->pins[1], LOW);
				set_PIN(stepper->pins[2], HIGH);
				set_PIN(stepper->pins[3], LOW);
				break;
			case 2 : // 0010
				set_PIN(stepper->pins[0], LOW);
				set_PIN(stepper->pins[1], LOW);
				set_PIN(stepper->pins[2], HIGH);
				set_PIN(stepper->pins[3], LOW);
				break;
			case 3 : // 0110
				set_PIN(stepper->pins[0], LOW);
				set_PIN(stepper->pins[1], HIGH);
				set_PIN(stepper->pins[2], HIGH);
				set_PIN(stepper->pins[3], LOW);
				break;
			case 4 : // 0100
				set_PIN(stepper->pins[0], LOW);
				set_PIN(stepper->pins[1], HIGH);
				set_PIN(stepper->pins[2], LOW);
				set_PIN(stepper->pins[3], LOW);
				break;
			case 5 : // 0101
				set_PIN(stepper->pins[0], LOW);
				set_PIN(stepper->pins[1], HIGH);
				set_PIN(stepper->pins[2], LOW);
				set_PIN(stepper->pins[3], HIGH);
				break;
			case 6 : // 0001
				set_PIN(stepper->pins[0], LOW);
				set_PIN(stepper->pins[1], LOW);
				set_PIN(stepper->pins[2], LOW);
				set_PIN(stepper->pins[3], HIGH);
				break;
			case 7 : // 1001
				set_PIN(stepper->pins[0], HIGH);
				set_PIN(stepper->pins[1], LOW);
				set_PIN(stepper->pins[2], LOW);
				set_PIN(stepper->pins[3], HIGH);
				break;
		}
}

