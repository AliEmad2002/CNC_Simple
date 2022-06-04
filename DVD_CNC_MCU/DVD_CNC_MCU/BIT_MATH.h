/*
 * BIT_MATH.h
 *
 * Created: 7/31/2021 7:11:33 PM
 *  Author: MohammedGamalEleish
 *
 *	Version :	V02
 *	Date	:	2 August 2021
 */ 


#ifndef BIT_MATH_H_
#define BIT_MATH_H_


#define SET_BIT(VAR,BIT)		(VAR |= (1<<BIT))
#define CLR_BIT(VAR,BIT)		(VAR &= (~(1<<BIT)))
#define TOG_BIT(VAR,BIT)		(VAR ^= (1<<BIT))
#define GET_BIT(VAR,BIT)		((VAR>>BIT) & 1)

#define SET_REG(REG)			(REG = 255)
#define CLR_REG(REG)			(REG = 0)
#define TOG_REG(REG)			(REG ^= 255)

#define SET_DWORD(DWORD)		(DWORD = 0xFFFF)
#define CLR_DWORD(DWORD)		(DWORD = 0)
#define TOG_DWORD(DWORD)		(DWORD ^= 0xFFFF)


#endif /* BIT_MATH_H_ */