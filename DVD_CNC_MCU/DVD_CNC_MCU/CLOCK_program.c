
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "CLOCK_interface.h"
#include "CLOCK_private.h"
#include "CLOCK_config.h"



void MCLOCK_voidDelayMS(u32 ms){
	register u32 Ticks = (CLOCK_SYSTEM_FREQUENCY/7000)*ms - 10;
	while(Ticks--)
		__asm__ volatile("NOP");
}

u32 MCLOCK_u32GetSystemClock(void){
	return CLOCK_SYSTEM_FREQUENCY;
}