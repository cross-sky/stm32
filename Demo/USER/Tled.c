#include "ComInclude.h"


void LedGpioConfig(void)
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOC的外设时钟*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 

	/*选择要控制的GPIOC引脚*/															   
	// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_13;

	/*设置引脚模式为通用推挽输出*/;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 


	/*调用库函数，初始化GPIOC*/;
	GPIO_Init(GPIOC, &GPIO_InitStructure);		  

	/* 关闭所有led灯	*/;

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




