#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "led.h"
//static u8 s_systic_1s_flag=0;

#define D_4MS	250
#define D_1MS	1000			//1ms中断1次,数字不能放前面，编译有问题;
#define D_100US	10000
#define D_10US	100000		//10us中断1次;
#define D_1US 	1000000		//1us中断1次;



void systick_init( void );
void delay_us( __IO u32 ntime);

void led_systick_delay( int ntime );

#endif


