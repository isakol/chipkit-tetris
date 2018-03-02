/* This file written 2018 by Alexander Leo and Isak Olsson */

#include <stdint.h>
#include <pic32mx.h>

int getsw( void ){
	return ((PORTD >> 8) & 0xf);
}

int getbtns(void){
	return ((PORTD >> 5) & 0x07) | (((PORTF >> 1) & 0x01)<< 3);
}