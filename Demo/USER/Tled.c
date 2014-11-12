#include "ComInclude.h"


void LedGpioConfig(void)
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����GPIOC������ʱ��*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 

	/*ѡ��Ҫ���Ƶ�GPIOC����*/															   
	// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_13;

	/*��������ģʽΪͨ���������*/;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 


	/*���ÿ⺯������ʼ��GPIOC*/;
	GPIO_Init(GPIOC, &GPIO_InitStructure);		  

	/* �ر�����led��	*/;

	GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_13);	 
}


void LEDdisplay(u8 LightChannel)
{
	switch(LightChannel)
	{

		case 1:	GPIO_WriteBit( GPIOC, GPIO_Pin_1,
				( BitAction)( (1-GPIO_ReadOutputDataBit( GPIOC, GPIO_Pin_1))));
				break;
		case 3: GPIO_WriteBit( GPIOC, GPIO_Pin_3,
				( BitAction)( (1-GPIO_ReadOutputDataBit( GPIOC, GPIO_Pin_3))));
				break;
		case 13: GPIO_WriteBit( GPIOC, GPIO_Pin_13,
					( BitAction)( (1-GPIO_ReadOutputDataBit( GPIOC, GPIO_Pin_13))));
				break;
		default: break;

	}
	

}

void LED1on(void)
{
	GPIO_WriteBit( GPIOC, GPIO_Pin_1,
		( BitAction)( (1-GPIO_ReadOutputDataBit( GPIOC, GPIO_Pin_1))));
}



