#include "ComInclude.h"


void UsartDmaTest(void)
{
	
#if USART1_DMA_RT
	teesst();
#else
	Usart1DmaSendTest();		//���ڳ�ʼ�� 115.2k;pa9,pa10
#endif
}

