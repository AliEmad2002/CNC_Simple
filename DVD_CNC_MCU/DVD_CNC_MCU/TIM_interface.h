/*
 * TIM_interface.h
 *
 * Created: 4/24/2021 7:27:57 PM
 *  Author: MohammedGamalEleish
 */ 

#ifndef TIM_INTERFACE_H_
#define TIM_INTERFACE_H_

/************************************************************************/
/*                             TIMER 0                                  */
/************************************************************************/

enum TIM0ClockSource{TIM0_CS_STOP,TIM0_CS_DIV_1,TIM0_CS_DIV_8,TIM0_CS_DIV_64,TIM0_CS_DIV_256,TIM0_CS_DIV_1024,TIM0_CS_T0_FALLING,TIM0_CS_T0_RISING};

enum TIM0Mode{TIM0_MODE_NORMAL,TIM0_MODE_PCPWM,TIM0_MODE_CTC,TIM0_MODE_FPWM};

enum TIM0CTC{TIM0_CTC_DIS,TIM0_CTC_TOG,TIM0_CTC_CLR,TIM0_CTC_SET};
	
enum TIM0PCPWM{TIM0_PCPWM_DIS,TIM0_PCPWM_NON_INVERT=2,TIM0_PCPWM_INVERT};

enum TIM0FPWM{TIM0_FPWM_DIS,TIM0_FPWM_NON_INVERT=2,TIM0_FPWM_INVERT};

enum TIM0Interrupt{TIM0_INTERRUPT_OVERFLOW,TIM0_INTERRUPT_COMPARE};

void MTIM0_voidInit(enum TIM0ClockSource ClockSource, enum TIM0Mode Mode, u8 ChannelMode, u8 Preload);

u8 MTIM0_u8GetCounter(void);

void MTIM0_voidSetCounter(u8 CounterValue);

void MTIM0_voidStart(void);

void MTIM0_voidStop(void);

void MTIM0_voidSetDutyCycle(u8 DutyCycle);//DutyCycle will be a number between 0=>255 representing a duty cycle 0=>100%

void MTIM0_voidSetFrequency(u16 Frequency);//Sets CTC Frequency

void MTIM0_voidACCURATE_SetFrequency(f32 Frequency);

void MTIM0_voidDelay_ticks(u32 TimeTicks);

void MTIM0_voidDelay_ms(f32 ms);

void MTIM0_voidDelay_us (f32 us);

void MTIM0_voidEnableInterrupt(enum TIM0Interrupt);

void MTIM0_voidDisableInterrupt(enum TIM0Interrupt);

void MTIM0_voidSetCallBack(enum TIM0Interrupt, void(*CallBack)(void));


/************************************************************************/
/*                             TIMER 1                                  */
/************************************************************************/
enum TIM1ClockSource{TIM1_CS_STOP,TIM1_CS_DIV_1,TIM1_CS_DIV_8,TIM1_CS_DIV_64,TIM1_CS_DIV_256,TIM1_CS_DIV_1024,TIM1_CS_T1_FALLING,TIM1_CS_T1_RISING};

enum TIM1Mode{TIM1_MODE_NORMAL, TIM1_MODE_PCPWM8bit, TIM1_MODE_PCPWM9bit, TIM1_MODE_PCPWM10bit, TIM1_MODE_CTC_OCR1A, TIM1_MODE_FPWM8bit, TIM1_MODE_FPWM9bit, TIM1_MODE_FPWM10bit, TIM1_MODE_PFCPWM_ICR1, TIM1_MODE_PFCPWM_OCR1A, TIM1_MODE_PCPWM_ICR1, TIM1_MODE_PCPWM_OCR1A, TIM1_MODE_CTC_ICR1, TIM1_MODE_FPWM_ICR1=14, TIM1_MODE_FPWM_OCR1A};

enum TIM1_Ch_Mode {TIM1_Ch_DIS, TIM1_Ch_TOG, TIM1_Ch_CLR, TIM1_Ch_SET};

enum TIM1Interrupt{TIM1_INTERRUPT_OVERFLOW, TIM1_INTERRUPT_COMPARE_B, TIM1_INTERRUPT_COMPARE_A, TIM1_INTERRUPT_CAPTURE};

enum TIM1CaptureInterrupt {TIM1_Capture_FallingEdge, TIM1_Capture_RisingEdge};

enum TIM11Output_Channel {TIM1ChannelB, TIM1ChannelA};
	
enum TIM1InputPulseType{TIM1_PULSE_LOW,TIM1_PULSE_HIGH};

void MTIM1_voidInit(enum TIM1ClockSource ClockSource, enum TIM1Mode Mode, u8 ChannelAMode, u8 ChannelBMode, u16 Preload);

u16 MTIM1_u16GetCounter(void);

void MTIM1_voidSetCounter(u16 CounterValue);

void MTIM1_voidStart(void);

void MTIM1_voidStop(void);

void MTIM1_voidSetDutyCycle(u16 DutyCycle, enum TIM11Output_Channel Channel);//DutyCycle will be a number between 0=>255 representing a duty cycle 0=>100%

void MTIM1_voidSetFrequency(u32 Frequency, enum TIM11Output_Channel Channel);//Sets CTC Frequency

void MTIM1_voidACCURATE_SetFrequency(f32 Frequency, enum TIM11Output_Channel Channel);

void MTIM1_voidDelay_ticks(u32 TimeTicks);

void MTIM1_voidSet_Capture_Edge (enum TIM1CaptureInterrupt CaptureEdge);

void MTIM1_voidSet_Noise_Canceller(u8 noise_canceller_state);

void MTIM1_voidEnableInterrupt(enum TIM1Interrupt);

void MTIM1_voidDisableInterrupt(enum TIM1Interrupt);

void MTIM1_voidSetCallBack(enum TIM1Interrupt, void* CallBack);

f32 MTIM1_f32GetPulseDuration(enum TIM1InputPulseType Pulse);

f32 MTIM1_f32GetSignalFrequency(void);

void MTIM1_voidStartTickMeasure(void);

u32  MTIM1_u32GetElapsedTicks(void);

u16 MTIM1_u16GetDivision(void);


/************************************************************************/
/*                             TIMER 2                                  */
/************************************************************************/
enum TIM2ClockSource{TIM2_CS_STOP,TIM2_CS_DIV_1,TIM2_CS_DIV_8, TIM2_CS_DIV_32,TIM2_CS_DIV_64, TIM2_CS_DIV_128,TIM2_CS_DIV_256,TIM2_CS_DIV_1024};

enum TIM2Mode{TIM2_MODE_NORMAL,TIM2_MODE_PCPWM,TIM2_MODE_CTC,TIM2_MODE_FPWM};

enum TIM2CTC{TIM2_CTC_DIS,TIM2_CTC_TOG,TIM2_CTC_CLR,TIM2_CTC_SET};

enum TIM2PCPWM{TIM2_PCPWM_DIS,TIM2_PCPWM_NON_INVERT=2,TIM2_PCPWM_INVERT};

enum TIM2FPWM{TIM2_FPWM_DIS,TIM2_FPWM_NON_INVERT=2,TIM2_FPWM_INVERT};

enum TIM2Interrupt{TIM2_INTERRUPT_OVERFLOW,TIM2_INTERRUPT_COMPARE};

void MTIM2_voidInit(enum TIM2ClockSource ClockSource, enum TIM2Mode Mode, u8 ChannelMode, u8 Preload);

u8 MTIM2_u8GetCounter(void);

void MTIM2_voidSetCounter(u8 CounterValue);

void MTIM2_voidStart(void);

void MTIM2_voidStop(void);

void MTIM2_voidSetDutyCycle(u8 DutyCycle);//DutyCycle will be a number between 2=>255 representing a duty cycle 2=>122%

void MTIM2_voidSetFrequency(u16 Frequency);//Sets CTC Frequency

void MTIM2_voidACCURATE_SetFrequency(f32 Frequency);

void MTIM2_voidDelay_ticks(u32 TimeTicks);

void MTIM2_voidEnableInterrupt(enum TIM2Interrupt);

void MTIM2_voidDisableInterrupt(enum TIM2Interrupt);

void MTIM2_voidSetCallBack(enum TIM2Interrupt, void(*CallBack)(void));

//Don't forget Asynchronous!

#endif /* TIM_INTERFACE_H_ */