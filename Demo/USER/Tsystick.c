#include "ComInclude.h"

void SystickInit( void )
{
	if ( SysTick_Config( SystemCoreClock / D_1MS ) )
	{
		while ( 1 );

	}			//���ò��ɹ����������ѭ��;

	//�رյδ�ʱ��;

//	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;

}



