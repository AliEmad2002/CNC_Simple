/*
 * INTERRUPT.h
 *
 * Created: 8/22/2021 12:14:15 AM
 *  Author: MohammedGamalEleish
 */ 


#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define INTERRUPT_REG_SREG				(*(volatile u8*)0x5F)

#define INTERRUPT_ENABLE_GLOBAL_INTERRUPT				(SET_BIT(INTERRUPT_REG_SREG,7))
#define INTERRUPT_DISABLE_GLOBAL_INTERRUPT				(CLR_BIT(INTERRUPT_REG_SREG,7))




#endif /* INTERRUPT_H_ */