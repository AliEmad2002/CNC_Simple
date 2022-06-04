/*
 * main.c
 *
 * Created: 23/01/2022 09:16:11 م
 *  Author: Ali Emad
 */ 
#include "BIT_MATH.h"
#include "STD_TYPES.h"
#include "manipPIN_interface.h"
#include "Stepper_interface.h"
#include "CLOCK_interface.h"
#include "simple_uart_interface.h"
#include "TIM_interface.h"

Stepper_t stepperx ;
Stepper_t steppery ;
Stepper_t stepperz ;

#define _baud_rate 9600UL

#define _x_limit_switch 0
#define _y_limit_switch 0
#define _z_limit_switch B3

s32 x_step = 0 ;
s32 y_step = 0 ;
s32 z_step = 0 ;

s32 x_step_max = 1000000 ; // foo infinity
s32 y_step_max = 1000000 ; // foo infinity
s32 z_step_max = 1000000 ; // foo infinity

#define _z_axis_head_up_pos 100

void zero_x_axis(void) ;
void zero_y_axis(void) ;
void zero_z_axis(void) ;
void maximize_z_axis(void) ;

int main(void)
{
	UART_init(_baud_rate) ;
	
	
	HStepper_init(&stepperz, HStepper_half_step, 40, A4, A5, D5, C0, _x_limit_switch, HStepper_lim_switch_pull_DOWN) ;
	HStepper_init(&steppery, HStepper_half_step, 40, A0, A1, D7, D2, _y_limit_switch, HStepper_lim_switch_pull_DOWN) ;
	HStepper_init(&stepperx, HStepper_half_step, 40, A3, A2, D3, D4, _z_limit_switch, HStepper_lim_switch_pull_DOWN) ;
	
	HStepper_set_speed(&stepperx, 200) ;
	HStepper_set_speed(&steppery, 200) ;
	HStepper_set_speed(&stepperz, 200) ;
	
	
	zero_x_axis() ;
	zero_y_axis() ;
	zero_z_axis() ;
	
	
	while(1)
	{
		char cmd = UART_RxChar() ;
		if (cmd == 'X')
		{
			x_step++;
			if(x_step > x_step_max)
			{
				UART_TxChar('E') ;
				x_step-- ;
				continue ;
			}
			HStepper_step(&stepperx, 1) ;
		}
		else if (cmd == 'x')
		{
			x_step-- ;
			if(x_step < 0)
			{
				UART_TxChar('E') ;
				x_step++ ;
				continue ;
			}
			HStepper_step(&stepperx, -1) ;
		}
		else if (cmd == 'Y')
		{
			y_step++;
			if(y_step > y_step_max)
			{
				UART_TxChar('E') ;
				y_step-- ;
				continue ;
			}
			HStepper_step(&steppery, 1) ;
		}
		else if (cmd == 'y')
		{
			y_step-- ;
			if(y_step < 0)
			{
				UART_TxChar('E') ;
				y_step++ ;
				continue ;
			}
			HStepper_step(&steppery, -1) ;
		}
		else if (cmd == 'Z')
		{
			z_step++ ;
			if(z_step > z_step_max)
			{
				UART_TxChar('E') ;
				z_step-- ;
				continue ;
			}
			HStepper_step(&stepperz, 1) ;
		}
		else if (cmd == 'z')
		{
			z_step-- ;
			if(z_step < 0)
			{
				UART_TxChar('E') ;
				z_step++ ;
				continue ;
			}
			HStepper_step(&stepperz, -1) ;
		}
		else if (cmd == 'H')
		{
			
			maximize_z_axis() ;
		}
		else if (cmd == 'h')
		{
			zero_z_axis() ;
				
		}
		
		else if (cmd == '0') //hard zero
		{
			zero_x_axis() ;
			zero_y_axis() ;
			zero_z_axis() ;
		}
		/*else if (cmd == '1') //zero here!... actually not needed
		{
			x_step = 0 ;
			y_step = 0 ;
			z_step = 0 ;
		}*/
		else if (cmd == '2') //max here!
		{
			x_step_max = x_step ;
			y_step_max = y_step ;
			z_step_max = z_step ;
		}
		/*else if (cmd == '3') //soft zero... actually not needed
		{
			zero_z_axis() ;
			maximize_z_axis() ;
			for(x_step=x_step; x_step>0; x_step--)
				HStepper_step(&stepperx, -1) ;
			for(y_step=y_step; y_step>0; y_step--)
				HStepper_step(&steppery, -1) ;
			zero_z_axis() ;
		}*/
		UART_TxChar(cmd) ;
	}
}




void zero_x_axis(void)
{
	x_step = 0 ;
	while(1)
	{
		HStepper_step(&stepperx, -1) ;
		if (digital_read(_x_limit_switch) == 0)
			return ;
	}
}

void zero_y_axis(void)
{
	y_step = 0 ;
	while(1)
	{
		HStepper_step(&steppery, 1) ;
		if (digital_read(_y_limit_switch) == 0)
			return ;
	}
}

void zero_z_axis(void)
{
	z_step = 0 ;
	while(1)
	{
		HStepper_step(&stepperz, -1) ;
		if (digital_read(_z_limit_switch) == 0)
			return ;
	}
}

void maximize_z_axis(void)
{
	for (z_step=z_step; z_step<_z_axis_head_up_pos; z_step++)
		HStepper_step(&stepperz, 1) ;
}