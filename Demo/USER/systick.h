#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "led.h"
//static u8 s_systic_1s_flag=0;

void systick_init( void );
void delay_us( __IO u32 ntime);

void led_systick_delay( int ntime );

#endif


