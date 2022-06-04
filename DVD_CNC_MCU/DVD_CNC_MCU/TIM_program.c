/*
 * TIM.c
 *
 * Created: 31/08/2021 12:46:40 م
 * Author : Ali Emad
 */

#include "BIT_MATH.h"
#include "STD_TYPES.h" //for u8, u16, u32, f32
#include "INTERRUPT.h" //for INTERRUPT_ENABLE_GLOBAL_INTERRUPT
#include "CLOCK_interface.h" //for MCLOCK_u32GetSystemClock()
#include <math.h> //for round()


#include "TIM_interface.h"
#include "TIM_private.h"


/************************************************************************/
/*                             TIMER 0                                  */
/************************************************************************/

void MTIM0_voidInit(enum TIM0ClockSource ClockSource, enum TIM0Mode Mode, u8 ChannelMode, u8 Preload)
{
	PTIM0_current_Mode = Mode;

	//setting prescaler in TCCR0_register : CS00_bit, CS01_bit, CS02_bit :
	TCCR0_register = ClockSource;
	PTIM0_Current_ClockSource = ClockSource;

	//setting mode in TCCR0_register : WGM00_bit, WGM01_bit :
	TCCR0_register |= (Mode&1) << WGM00_bit ;
	TCCR0_register |= (Mode&2)/2 << WGM01_bit ;

	//setting channel mode in TCCR0_register : COM00_bit, COM01_bit :
	TCCR0_register |= ChannelMode << 4 ;
	
	//preloading TCNT0
	TCNT0_register = Preload;
}

u8 MTIM0_u8GetCounter(void)
{
	return TCNT0_register;
}

void MTIM0_voidSetCounter(u8 CounterValue)
{
	TCNT0_register = CounterValue;
}

void MTIM0_voidStart(void)
{
	TCCR0_register |= PTIM0_Current_ClockSource;
}

void MTIM0_voidStop(void)
{
	TCCR0_register &= ~7;
}

void MTIM0_voidSetDutyCycle(u8 DutyCycle)
{
	OCR0_register = DutyCycle;
}

void MTIM0_voidSetFrequency(u16 Frequency)
{
	u32 _F_CPU = MCLOCK_u32GetSystemClock();
	OCR0_register = _F_CPU / (2 * (u32)PTIM0_prescalers[PTIM0_Current_ClockSource-1] * Frequency ) - 1;
}

void MTIM0_voidACCURATE_SetFrequency(f32 Frequency) //notice that this function could change prescaler to reach to nearest possible Freq!
{
	f32 _F_CPU = MCLOCK_u32GetSystemClock();
	u8 OCR[5];
	f32 Delta_F[5];

	//calculating OCR register value, and Delta_F :
	for(u8 i=0; i<5; i++)
	{
		OCR[i] = (u8) (round(_F_CPU / (2.0 * (f32)PTIM0_prescalers[i] * Frequency ) - 1.0f));
		Delta_F[i] = (Frequency - _F_CPU / ( 2.0f * (f32)PTIM0_prescalers[i] * (1.0 + (double)OCR[i]) ));
		if(Delta_F[i] < 0)
			Delta_F[i] *= -1.0;
	}

	//finding smallest Delta_F :
	float temp = Delta_F[0];
	u8 I = 0;
	for(u8 i=1; i<5; i++)
		if(Delta_F[i]<temp)
		{
			temp = Delta_F[i];
			I = i;
		}
	//changing pre-scaler :
	TCCR0_register = I + 1;
	PTIM0_Current_ClockSource = I;

	OCR0_register = OCR[I];
}

void MTIM0_voidDelay_ticks(u32 TimeTicks)
{
	u32 ticks = 0;
	u8 temp_TCNT0;
	while(ticks < TimeTicks)
	{
		temp_TCNT0 = TCNT0_register;
		while(temp_TCNT0 == TCNT0_register); //wait for one tick to pass
		ticks++ ;
	}
}

f32 PTIM0_get_Frequency (void)
{
	f32 Frequency = 0.0;
	f32 _F_CPU = MCLOCK_u32GetSystemClock();

	if(PTIM0_current_Mode == TIM0_MODE_CTC)
		Frequency = (f32)_F_CPU / (f32)(2 * PTIM0_prescalers[PTIM0_Current_ClockSource] * (1 + OCR0_register) );
	else
		Frequency = _F_CPU / (f32)(PTIM0_prescalers[PTIM0_Current_ClockSource-1] * 256);

	return Frequency;
}

void MTIM0_voidDelay_ms (f32 ms)
{
	//Do not forget to add external clock source case!!
	f32 Frequency = PTIM0_get_Frequency();
	u32 ticks = (f32)256.0 * ms * Frequency / (f32)1000.0 ;
	MTIM0_voidDelay_ticks (ticks);
}

void MTIM0_voidDelay_us (f32 us)
{
	//Do not forget to add external clock source case!!
	f32 Frequency = PTIM0_get_Frequency();
	u32 ticks = 256.0f * us * Frequency / 1000000.0f ;
	MTIM0_voidDelay_ticks (ticks);
}

void MTIM0_voidEnableInterrupt(enum TIM0Interrupt Mode)
{
	INTERRUPT_ENABLE_GLOBAL_INTERRUPT; //enable global interrupt
	TIMSK_register |= (1<<Mode);
}

void MTIM0_voidDisableInterrupt(enum TIM0Interrupt Mode)
{
	TIMSK_register &= ~(1<<Mode);
}

void MTIM0_voidSetCallBack(enum TIM0Interrupt Mode, void(*CallBack)(void))
{
	if (Mode == TIM0_INTERRUPT_OVERFLOW)
		PTIM0_callback_OVF = CallBack;

	else if (Mode == TIM0_INTERRUPT_COMPARE)
		PTIM0_callback_CTC = CallBack;
}

void PTIM0_VECT_ISR_OVF(void)
{
	PTIM0_callback_OVF();
}

void PTIM0_VECT_ISR_CTC(void)
{
	PTIM0_callback_CTC();
}

/************************************************************************/
/*                             TIMER 1                                  */
/************************************************************************/

void MTIM1_voidInit(enum TIM1ClockSource ClockSource, enum TIM1Mode Mode, u8 ChannelAMode, u8 ChannelBMode, u16 Preload)
{
	TCCR1A_register = (Mode&3) | (ChannelAMode<<6) | (ChannelBMode<<4);
	TCCR1B_register = ClockSource | ( (Mode & 12)<<1 );
	TCNT1_register = Preload;
	PTIM1_Current_ClockSource = ClockSource;
	PTIM1_current_Mode = Mode;
}

u16 MTIM1_u16GetCounter(void)
{
	return TCNT1_register;
}

void MTIM1_voidSetCounter(u16 CounterValue)
{
	TCNT1_register = CounterValue;
}

void MTIM1_voidSetDutyCycle(u16 DutyCycle, enum TIM11Output_Channel Channel)
{
	//*(&OCR1B_register + 2*Channel) = DutyCycle;  why doesn't it work? 😭
	if(Channel == TIM1ChannelA)
		OCR1A_register = DutyCycle;
	else if (Channel == TIM1ChannelB)
		OCR1B_register = DutyCycle; //I can't find OCR1B as a top value in the datasheet?! /////////////////////////////////////////////////////////////////////
}

void MTIM1_voidSetFrequency(u32 Frequency, enum TIM11Output_Channel Channel) //depends on the mode
{
	//this only sets CTC mode Freq. can't I set other modes Freqs as well?
	u32 _F_CPU = MCLOCK_u32GetSystemClock();
	u16 OCR = _F_CPU / (2*Frequency*PTIM1_prescalers[PTIM1_Current_ClockSource-1]) - 1;
	if(Channel == TIM1ChannelA)
		OCR1A_register = OCR;
	else if(Channel == TIM1ChannelB)
		OCR1B_register = OCR;
}

void MTIM1_voidACCURATE_SetFrequency(f32 Frequency, enum TIM11Output_Channel Channel) //depends on the mode
{
	u32 _F_CPU = MCLOCK_u32GetSystemClock();
	u16 OCR[5];
	f32 Delta_F[5];

	//calculating OCR register value, and Delta_F :
	for(u8 i=0; i<5; i++)
	{

		OCR[i] = (u16) (round((f32)_F_CPU / (2.0f * (f32)Frequency * (f32)PTIM1_prescalers[PTIM1_Current_ClockSource-1]) - 1.0f));
		Delta_F[i] = ((f32)Frequency - (f32)_F_CPU / ( 2.0f * (f32)PTIM1_prescalers[i] * (1.0f + (f32)OCR[i]) ));
		if(Delta_F[i] < 0.0f)
			Delta_F[i] *= -1.0f;
	}

	//finding smallest Delta_F :
	f32 temp = Delta_F[0];
	u8 I = 0;
	for(u8 i=1; i<5; i++)
		if(Delta_F[i]<temp)
		{
			temp = Delta_F[i];
			I = i;
		}
	TCCR1B_register &= (I+1) & 0xFF; //CHANGING PRESCALER
	if(Channel == TIM1ChannelA)
		OCR1A_register = OCR[I];
	else if(Channel == TIM1ChannelB)
		OCR1B_register = OCR[I];
	//*(&OCR1B_register + 2*Channel) = OCR[I];  why doesn't it work? 😭
}

void MTIM1_voidDelay_ticks(u32 TimeTicks)
{
	u32 ticks = 0;
	u16 temp_TCNT1;
	while(ticks < TimeTicks)
	{
		temp_TCNT1 = TCNT0_register;
		while(temp_TCNT1 == TCNT1_register); //wait for one tick to pass
		ticks++ ;
	}
}

void MTIM1_voidSet_Capture_Edge (enum TIM1CaptureInterrupt CaptureEdge)
{
	TCCR1B_register |= CaptureEdge<<ICES1_bit ;
}

void MTIM1_voidSet_Noise_Canceller(u8 noise_canceller_state)
{
	TCCR1B_register |= noise_canceller_state<<ICNC1_bit ;
}

void MTIM1_voidEnableInterrupt(enum TIM1Interrupt Mode)
{
	TIMSK_register |= 1 << (Mode+2);
	TIFR_register  |= 1 << (Mode+2); //clearing Interrupt flag (it's cleared by putting 1 in it)
	INTERRUPT_ENABLE_GLOBAL_INTERRUPT;
}

void MTIM1_voidDisableInterrupt(enum TIM1Interrupt Mode)
{
	TIMSK_register &= ~( 1 << (Mode+2) );
}

void MTIM1_voidSetCallBack(enum TIM1Interrupt Mode, void* CallBack)
{
	if(Mode < TIM1_INTERRUPT_CAPTURE)
		PTIM1_callback[Mode-2] = (void(*)(void))CallBack;
	else if (Mode == TIM1_INTERRUPT_CAPTURE)
		PTIM1_callback_Capture = (void(*)(u16))CallBack;
}

f32 MTIM1_f32GetPulseDuration(enum TIM1InputPulseType Pulse)
{
	volatile u16 t1;
	volatile u16 t2;
	volatile u8  OVF_count = 0;
	TCNT1_register = 0;
	//enable noise canceler :
	TCCR1B_register |= 1<<ICNC1_bit;

	//clear OVF flag and Input capture flag :
	TIFR_register |= (1<<TOV1_bit) | (1<<ICF1_bit) ;

	if (Pulse == TIM1_PULSE_LOW)
		TCCR1B_register &= ~(1<<ICES1_bit);
	else if (Pulse == TIM1_PULSE_HIGH)
		TCCR1B_register |= 1<<ICES1_bit;

	while(  ! (TIFR_register & (1<<ICF1_bit) )  ); //wait for Input capture event.

	t1 = ICR1_register ;
	//clear input capture flag.
	TIFR_register |= 1<<ICF1_bit ;

	if (Pulse == TIM1_PULSE_LOW)
		TCCR1B_register |= 1<<ICES1_bit;
	else if (Pulse == TIM1_PULSE_HIGH)
		TCCR1B_register &= ~(1<<ICES1_bit);

	while(  ! (TIFR_register & (1<<ICF1_bit) )  ) //why do I wait for it to get 1? //////////////////////////////////////////////////////////////////////////////////////////
	//while I've just set it 1 ?
	{
		if( TIFR_register & (1<<TOV1_bit))
		{
			OVF_count++ ;
			//clear OVF flag :
			TCCR1B_register |= 1<<TOV1_bit;
		}
	}
	t2 = ICR1_register ;

	volatile u32 ticks =  t2 + OVF_count*65536 - t1;
	u32 _F_CPU = MCLOCK_u32GetSystemClock();
	volatile f32 millis = 1000.0f * (f32)ticks *  (f32)PTIM1_prescalers[PTIM1_Current_ClockSource-1] / (f32)_F_CPU ;
	return millis;
}

f32 MTIM1_f32GetSignalFrequency(void)
{
	volatile u16 t1;
	volatile u16 t2;
	volatile u8  OVF_count = 0;
	TCNT1_register = 0;
	//enable noise canceler :
	TCCR1B_register |= 1<<ICNC1_bit;

	//clear OVF flag and Input capture flag :
	TIFR_register |= (1<<TOV1_bit) | (1<<ICF1_bit) ;

	//capture rising Edge
	TCCR1B_register |= 1<<ICES1_bit;

	while(  ! (TIFR_register & (1<<ICF1_bit) )  ); //wait for Input capture event.

	t1 = ICR1_register ;

	//clear input capture flag.
	TIFR_register |= 1<<ICF1_bit ;

	while(  ! (TIFR_register & (1<<ICF1_bit) )  ) //why do I wait for it to get 1? //////////////////////////////////////////////////////////////////////////////////////////
	//while I've just set it 1 ?
	{
		if( TIFR_register & (1<<TOV1_bit))
		{
			OVF_count++ ;
			//clear OVF flag :
			TCCR1B_register |= 1<<TOV1_bit;
		}
	}
	t2 = ICR1_register ;

	volatile u32 ticks =  t2 + OVF_count*65536 - t1;
	u32 _F_CPU = MCLOCK_u32GetSystemClock();
	volatile f32 Freq = (f32)_F_CPU / ( (f32)ticks * (f32)PTIM1_prescalers[PTIM1_Current_ClockSource-1] );
	return Freq;
}

void MTIM1_voidStartTickMeasure(void)
{
	PTIM1_u32OverFlowCount = 0;
	MTIM1_voidEnableInterrupt(TIM1_INTERRUPT_OVERFLOW);
	PTIM1_u16FirstValue = TCNT1_register ;
}

u32  MTIM1_u32GetElapsedTicks(void)
{
		u32 Ticks = (u32)TCNT1_register + (u32)PTIM1_u32OverFlowCount * 65536 - (u32)PTIM1_u16FirstValue;
		return Ticks;
}

u16 MTIM1_u16GetDivision(void)
{
	return PTIM1_prescalers[PTIM1_Current_ClockSource - 1];
}

void PTIM1_VECT_ISR_OVF (void)
{
	PTIM1_u32OverFlowCount++;
	if(PTIM1_callback[0] != 0)
		PTIM1_callback[0]();
}

void PTIM1_VECT_ISR_CTCB (void)
{
	if(PTIM1_callback[1] != 0)
		PTIM1_callback[1]();
}

void PTIM1_VECT_ISR_CTCA (void)
{
	if(PTIM1_callback[2] != 0)
		PTIM1_callback[2]();
}

void PTIM1_VECT_ISR_Capture (void)
{
	if(PTIM1_callback_Capture != 0)
		PTIM1_callback_Capture (ICR1_register);
}

/************************************************************************/
/*                             TIMER 2                                  */
/************************************************************************/

void MTIM2_voidInit(enum TIM2ClockSource ClockSource, enum TIM2Mode Mode, u8 ChannelMode, u8 Preload)
{
	PTIM2_current_Mode = Mode;

	//setting prescaler in TCCR2_register : CS20_bit, CS21_bit, CS22_bit :
	TCCR2_register = ClockSource;
	PTIM2_Current_ClockSource = ClockSource;

	//setting mode in TCCR2_register : WGM20_bit, WGM21_bit :
	TCCR2_register |= (Mode&1) << WGM20_bit ;
	TCCR2_register |= (Mode&2)/2 << WGM21_bit ;

	//setting channel mode in TCCR2_register : COM20_bit, COM21_bit :
	TCCR2_register |= ChannelMode << 4 ;

	//preloading TCNT2
	TCNT2_register = Preload;
}

u8 MTIM2_u8GetCounter(void)
{
	return TCNT2_register;
}

void MTIM2_voidSetCounter(u8 CounterValue)
{
	TCNT2_register = CounterValue;
}

void MTIM2_voidStart(void)
{
	TCCR2_register |= PTIM2_Current_ClockSource;
}

void MTIM2_voidStop(void)
{
	TCCR2_register &= ~ 7;
}

void MTIM2_voidSetDutyCycle(u8 DutyCycle)
{
	OCR2_register = DutyCycle;
}

void MTIM2_voidSetFrequency(u16 Frequency)
{
	u32 _F_CPU = MCLOCK_u32GetSystemClock();
	OCR2_register = _F_CPU / (2 * (u32)PTIM2_prescalers[PTIM2_Current_ClockSource-1] * Frequency ) - 1;
}

void MTIM2_voidACCURATE_SetFrequency(f32 Frequency)
{
	f32 _F_CPU = MCLOCK_u32GetSystemClock();
	u8 OCR[5];
	f32 Delta_F[5];

	//calculating OCR register value, and Delta_F :
	for(u8 i=0; i<5; i++)
	{
		OCR[i] = (u8) (round(_F_CPU / (2.0 * (f32)PTIM2_prescalers[i] * Frequency ) - 1.0f));
		Delta_F[i] = (Frequency - _F_CPU / ( 2.0f * (f32)PTIM2_prescalers[i] * (1.0 + (f32)OCR[i]) ));
		if(Delta_F[i] < 0)
			Delta_F[i] *= -1.0;
	}

	//finding smallest Delta_F :
	float temp = Delta_F[0];
	u8 I = 0;
	for(u8 i=1; i<5; i++)
	if(Delta_F[i]<temp)
	{
		temp = Delta_F[i];
		I = i;
	}
	//changing pre-scaler :
	TCCR2_register = I + 1;
	PTIM2_Current_ClockSource = I;

	//changing Frequency:
	OCR2_register = OCR[I];
}

void MTIM2_voidDelay_ticks(u32 TimeTicks)
{
	u32 ticks = 0;
	u8 temp_TCNT2;
	while(ticks < TimeTicks)
	{
		temp_TCNT2 = TCNT2_register;
		while(temp_TCNT2 == TCNT2_register); //wait for one tick to pass
		ticks++ ;
	}
}

void MTIM2_voidEnableInterrupt(enum TIM2Interrupt Mode)
{
	INTERRUPT_ENABLE_GLOBAL_INTERRUPT; //enable global interrupt
	TIMSK_register |= 1 << (Mode+6);
}

void MTIM2_voidDisableInterrupt(enum TIM2Interrupt Mode)
{
	TIMSK_register &= ~(1 << (Mode+6));
}

void MTIM2_voidSetCallBack(enum TIM2Interrupt Mode, void(*CallBack)(void))
{
	if (Mode == TIM2_INTERRUPT_OVERFLOW)
		PTIM2_callback_OVF = CallBack;

	else if (Mode == TIM2_INTERRUPT_COMPARE)
	PTIM2_callback_CTC = CallBack;
}

void PTIM2_VECT_ISR_OVF(void)
{
	PTIM0_callback_OVF();
}

void PTIM2_VECT_ISR_CTC(void)
{
	PTIM0_callback_CTC();
}
