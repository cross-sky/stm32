/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
<<<<<<< HEAD
 * 文件名  ：led.c;
 * 描述    ：led 应用函数库  ;       
 * 实验平台：野火STM32开发板;
 * 硬件连接：-----------------;
=======
 * 文件名  ：led.c;
 * 描述    ：led 应用函数库   ;      
 * 实验平台：野火STM32开发板;
 * 硬件连接：-----------------;
>>>>>>> 335796abe5a527f8f02456764720a4efa9dfba1f
 *          |   PC1 - LED1     |
 *          |   PC3 - LED2     |
 *          |   PC13 - LED3     |
 *           ----------------- 
<<<<<<< HEAD
 * 库版本  ：ST3.5.0;
 * 作者    ：wildfire team ;
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html;
 * 淘宝    ：http://firestm32.taobao.com;
=======
 * 库版本  ：ST3.5.0;
 * 作者    ：wildfire team ;
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html;
 * 淘宝    ：http://firestm32.taobao.com;
>>>>>>> 335796abe5a527f8f02456764720a4efa9dfba1f
**********************************************************************************/
#include "led.h"

/*
<<<<<<< HEAD
 * 函数名：LED_GPIO_Config;
 * 描述  ：配置LED用到的I/O口;
 * 输入  ：无;
 * 输出  ：无;
=======
 * 函数名：LED_GPIO_Config
 * 描述  ：配置LED用到的I/O口;
 * 输入  ：无;
 * 输出  ：无;
>>>>>>> 335796abe5a527f8f02456764720a4efa9dfba1f
 */
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


void leds( void )
{

		LED1( ON );			  // 亮;
		Delay(0x0FFFEF);
		Delay(0x0FFFEF);
		LED1( OFF );		  // 灭;

		LED1( ON );			  // 亮;
		Delay(0x0FFFEF);
		Delay(0x0FFFEF);
		LED1( OFF );		  // 灭;

		Delay(0x0FFFEF);
		Delay(0x0FFFEF);
		
//		LED2( ON );
//		Delay(0x0FFFEF);
//		Delay(0x0FFFEF);
//		LED2( OFF );
		
		LED3( ON );
		Delay(0x0FFFEF);
		Delay(0x0FFFEF);
		LED3( OFF ); 
}


void Delay(__IO u32 nCount)	 //简单的延时函数;
{
	for(; nCount != 0; nCount--);
} 

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
