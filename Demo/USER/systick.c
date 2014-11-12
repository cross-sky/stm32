#include "systick.h"

#include "uart.h"
#include "adc.h"

#include "led.h"

#include "mpu6050m.h"


#define D_1MS	1000			//1ms中断1次,数字不能放前面，编译有问题;
#define D_100US	10000
#define D_10US	100000		//10us中断1次;
#define D_1US 	1000000		//1us中断1次;


static __IO u32 time_delay;

void SystickInit( void )
{
	if ( SysTick_Config( SystemCoreClock / D_1MS ) )
	{
		while ( 1 );

	}			//配置不成功，则进入死循环;

	//关闭滴答定时器;
	//SysTick->CTRL &= ~SysTick_CTRL_ENABLE_MASK; //多了一个字，还敲错了;

	  SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	
}
	
void delay_us( __IO u32 ntime)
{
	time_delay = ntime;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	while ( time_delay != 0 );
}

void time_delay_decrement( void )
{
	if( time_delay != 0x00 )
		time_delay--;
}

void led_systick_delay( int ntime )
{
	u8 i;
	usart1_sendback();
	 
  LED1( 0 ); 
	  delay_us(ntime);    	// 50000 * 10us = 500ms
//	  printf( "1\n");	  
		LED1( 1 );
	  
		LED2( 0 );
	delay_us( ntime );
//	  i2c_dma_read(0x3b);
//		Delay(0xf000);		// 50000 * 10us = 500ms
//	  for (i=0; i<7;i++)
//	  {
//		  printf("%d  ",accel_gyro[i]);
//	  }
    mpu_read_xyz(MPU6050_Addr);
		for (i=0; i<7;i++)
		{
			printf("%d,",mympu[i]);
		}
		LED2( 1 );

		LED3( 0 );
	  delay_us( ntime );		// 50000 * 10us = 500ms
		printf("%f,", adc1_value());
//	  delay_us(ntime);    	// 50000 * 10us = 500ms
	  printf("%3d,",tempadc());
		LED3( 1 );		
}

