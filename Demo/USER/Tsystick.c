#include "ComInclude.h"

void SystickInit( void )
{
	if ( SysTick_Config( SystemCoreClock / D_1MS ) )
	{
		while ( 1 );

	}			//配置不成功，则进入死循环;

	//关闭滴答定时器;

//	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;

}




