/*
 * manipPIN_program.c
 *
 * Created: 2021-08-06 2:16:06 PM
 * Author : Ali Emad
 */
#include "manipPIN_interface.h"

void init_PIN(unsigned char pin, enum Pin_mode set)
{
	//finding DDRn : by analyzing char pin MSN :
	unsigned char n = pin/0x0F;
	if(n*0x0F > pin-0x0A)
		n--;
	char* setAddress = (char*)(88 - 3 * n); //according to the register summary chapter in the datasheet : I concluded this eqn.
	unsigned char pinNumber = pin - 0x0F * n - n;
	if (set == OUTPUT)
		*setAddress |= 1<<pinNumber;
	else if (set == INPUT)
		*setAddress &= ~(1<<pinNumber);
}

void set_PIN(unsigned char pin, enum Pin_state set)
{
	//finding PORTn : by analyzing char pin MSN :
	unsigned char n = pin/0x0F;
	if(n*0x0F > pin-0x0A)
		n--;
	char* portAddress = (char*)(89 - 3 * n); //according to the register summary chapter in the datasheet : I concluded this eqn.
	unsigned char pinNumber = pin - 0x0F * n - n;
	if (set == LOW)
		*portAddress &= ~(1<<pinNumber);
	else if(set == HIGH)
		*portAddress |= 1<<pinNumber;
	else if(set == TOGGLE)
		*portAddress ^= 1<<pinNumber;
}

unsigned char digital_read (unsigned char pin)
{
	//finding PINn : by analyzing char pin MSN :
	unsigned char n = pin/0x0F;
	if(n*0x0F > pin-0x0A)
		n--;
	char* pinAddress = (char*)(87 - 3 * n); //according to the register summary chapter in the datasheet : I concluded this eqn.
	unsigned char pinNumber = pin - 0x0F * n - n;
	return *pinAddress & (1<<pinNumber);
}
