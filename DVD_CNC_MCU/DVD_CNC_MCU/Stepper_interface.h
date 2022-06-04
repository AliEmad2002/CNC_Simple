/*
 * Stepper_include.h
 *
 * Created: 23/01/2022 07:35:42 م
 *  Author: Ali Emad
 */ 


#ifndef STEPPER_INCLUDE_H_
#define STEPPER_INCLUDE_H_

enum HStepper_step_size{HStepper_full_step=4, HStepper_half_step=8};
	
enum HStepper_lim_switch_pull_state {HStepper_lim_switch_pull_DOWN=0, HStepper_lim_switch_pull_UP=1};

typedef struct
{
	u8 pins[4] ;
	u8 lim_switch_pin ;
	s16 current_step_number ;
	u16 steps_total ;
	u32 ticks_delay_between_steps ;
	u32 last_time_stamp ;
	enum HStepper_step_size step_size ;
	enum HStepper_lim_switch_pull_state lim_switch_pull_state ;
}Stepper_t;

void HStepper_init(Stepper_t* stepper, enum HStepper_step_size stepSize, u16 steps_per_rev, u8 pin1, u8 pin2, u8 pin3, u8 pin4, u8 lim_pin, enum HStepper_lim_switch_pull_state lim_pull_state) ;
void HStepper_set_speed(Stepper_t* stepper, u16 speed) ;
void HStepper_step(Stepper_t* stepper, s16 steps) ;
void HStepper_step_from_seq(Stepper_t* stepper, u8 n) ;
void HStepper_zreo_axis(Stepper_t* stepper) ;


#endif /* STEPPER_INCLUDE_H_ */


//PROBLEMS & SOLUTIONS:
// 1:
// if the stepper moves x steps forward, then comes back a bit, then move x steps forward, and so on...
// then the number of steps per rev passed to init function is less than the motor's
//
// 2:
//