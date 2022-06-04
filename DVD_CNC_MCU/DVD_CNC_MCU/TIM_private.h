/*
 * TIM_private.h
 *
 * Created: 31/08/2021 09:37:00 م
 *  Author: Ali Emad
 */ 


#ifndef TIM_PRIVATE_H_
#define TIM_PRIVATE_H_


/************************************************************************/
/*                             TIMER 0                                  */
/************************************************************************/
u16 PTIM0_prescalers[5] = {1, 8, 64, 256, 1024};

u8 PTIM0_Current_ClockSource;
u8 PTIM0_current_Mode;

f32 PTIM0_get_Frequency(void);
void PTIM0_change_prescaler(enum TIM0ClockSource Prescaler);

#define TCCR0_register ( *(volatile u8 *) 0x53 )
#define OCR0_register  ( *(volatile u8 *) 0x5C )
#define TCNT0_register ( *(volatile u8 *) 0x52 )
#define TIMSK_register ( *(volatile u8 *) 0x59 )
#define TIFR_register  ( *(volatile u8 *) 0x58 )

#define CS00_bit 0
#define CS01_bit 1
#define CS02_bit 2
#define WGM01_bit 3
#define COM00_bit 4
#define COM01_bit 5
#define WGM00_bit 6

#define TOIE0_bit 0
#define OCIE0_bit 1

#define PTIM0_VECT_ISR_OVF __vector_9	//(9 on atmega16), change it to 11 on atmega32
#define PTIM0_VECT_ISR_CTC __vector_19 //(19 on atmega16), change it to 10 on atmega32

void PTIM0_VECT_ISR_OVF(void) __attribute__((signal, used));
void PTIM0_VECT_ISR_CTC(void) __attribute__((signal, used));

void (*PTIM0_callback_OVF)(void);
void (*PTIM0_callback_CTC)(void);



/************************************************************************/
/*                             TIMER 1                                  */
/************************************************************************/
u16 PTIM1_prescalers[5] = {1, 8, 64, 256, 1024};

#define TCCR1A_register ( *(volatile u8  *) 0x4F )
#define TCCR1B_register ( *(volatile u8  *) 0x4E )
#define TCNT1_register  ( *(volatile u16 *) 0x4C ) //nice trick, but lucky because both registers are one after the other
#define OCR1A_register  ( *(volatile u16 *) 0x4A )
#define OCR1B_register  ( *(volatile u16 *) 0x48 )
#define ICR1_register   ( *(volatile u16 *) 0x46 )

#define WGM10_bit 0
#define WGM11_bit 1
#define COM1B0_bit 4
#define COM1B1_bit 5
#define COM1A0_bit 6
#define COM1A1_bit 7

#define CS10_bit 0
#define CS11_bit 1
#define CS12_bit 2
#define WGM12_bit 3
#define WGM13_bit 4
#define ICES1_bit 6
#define ICNC1_bit 7

#define TOIE1_bit 2
#define OCIE1B_bit 3
#define OCIE1A_bit 4
#define TICIE1_bit 5

#define TOV1_bit 2
#define OCF1B_bit 3
#define OCF1A_bit 4
#define ICF1_bit 5

#define PTIM1_VECT_ISR_OVF __vector_8	//(8 on atmega16), change it to  on atmega32
#define PTIM1_VECT_ISR_CTCB __vector_7 //(7 on atmega16), change it to  on atmega32
#define PTIM1_VECT_ISR_CTCA __vector_6 //(6 on atmega16), change it to  on atmega32
#define PTIM1_VECT_ISR_Capture __vector_5 //(5 on atmega16), change it to 10 on atmega32

void PTIM1_VECT_ISR_OVF    (void) __attribute__((signal, used));
void PTIM1_VECT_ISR_CTCB   (void) __attribute__((signal, used));
void PTIM1_VECT_ISR_CTCA   (void) __attribute__((signal, used));
void PTIM1_VECT_ISR_Capture(void) __attribute__((signal, used));

void (*PTIM1_callback[3])(void);
void (*PTIM1_callback_Capture)(u16);

u8 PTIM1_Current_ClockSource;
u8 PTIM1_current_Mode;
u32 PTIM1_u32OverFlowCount; //used in function MTIM1_voidStartTickMeasure AND MTIM1_u32GetElapsedTicks
u16 PTIM1_u16FirstValue;	//used in function MTIM1_voidStartTickMeasure AND MTIM1_u32GetElapsedTicks

/************************************************************************/
/*                             TIMER 2                                  */
/************************************************************************/
u16 PTIM2_prescalers[7] = {1, 8, 32, 64, 128, 256, 1024};

u8 PTIM2_Current_ClockSource;
u8 PTIM2_current_Mode;

void PTIM2_change_prescaler(enum TIM2ClockSource Prescaler);

#define TCCR2_register ( *(volatile u8 *) 0x45 )
#define OCR2_register  ( *(volatile u8 *) 0x43 )
#define TCNT2_register ( *(volatile u8 *) 0x44 )

#define CS20_bit 0
#define CS21_bit 1
#define CS22_bit 2
#define WGM21_bit 3
#define COM20_bit 4
#define COM21_bit 5
#define WGM20_bit 6
#define FOC2_bit 7

#define TOIE2_bit 6
#define OCIE2_bit 7

#define PTIM2_VECT_ISR_OVF __vector_4	//(4 on atmega16), change it to 11 on atmega32
#define PTIM2_VECT_ISR_CTC __vector_3   //(3 on atmega16), change it to 10 on atmega32

void PTIM2_VECT_ISR_OVF(void) __attribute__((signal, used));
void PTIM2_VECT_ISR_CTC(void) __attribute__((signal, used));

void (*PTIM2_callback_OVF)(void);
void (*PTIM2_callback_CTC)(void);
#endif /* TIM_PRIVATE_H_ */