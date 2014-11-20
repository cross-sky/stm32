#include "ComInclude.h"


#define D_4MS	250
#define D_1MS	1000			//1ms中断1次

u8 G_TestExcut;

void SystickInit( void )
{
	if ( SysTick_Config( SystemCoreClock / D_4MS ) )
	{
		while ( 1 );

	}			//配置不成功，则进入死循环;
	TestExcuteTimeGpioConfig();

}

void TestExcuteTime(u8 state)
{
	if(state == 1)
		GPIO_SetBits(GPIOC,GPIO_Pin_2);
	else
		GPIO_ResetBits(GPIOC,GPIO_Pin_2);
}

void TestExcuteTimeGpioConfig(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);		  
}


