/*
 * manipPIN_interface.h
 *
 * Created: 2021-08-06 2:16:06 PM
 * Author : a
 */
#ifndef	_MANIPPIN_INTERFACE
#define _MANIPPIN_INTERFACE

#define A0 0xA0
#define A1 0xA1
#define A2 0xA2
#define A3 0xA3
#define A4 0xA4
#define A5 0xA5
#define A6 0xA6
#define A7 0xA7

#define B0 0xB0
#define B1 0xB1
#define B2 0xB2
#define B3 0xB3
#define B4 0xB4
#define B5 0xB5
#define B6 0xB6
#define B7 0xB7

#define C0 0xC0
#define C1 0xC1
#define C2 0xC2
#define C3 0xC3
#define C4 0xC4
#define C5 0xC5
#define C6 0xC6
#define C7 0xC7

#define D0 0xD0
#define D1 0xD1
#define D2 0xD2
#define D3 0xD3
#define D4 0xD4
#define D5 0xD5
#define D6 0xD6
#define D7 0xD7

enum Pin_mode {INPUT, OUTPUT};
	
enum Pin_state {LOW, HIGH, TOGGLE};


void init_PIN(unsigned char pin, enum Pin_mode set);

void set_PIN(unsigned char pin, enum Pin_state set);

unsigned char digital_read (unsigned char pin);

#endif	/* manipPIN_interface.h*/
