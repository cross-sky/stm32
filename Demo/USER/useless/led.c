/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
<<<<<<< HEAD
 * �ļ���  ��led.c;
 * ����    ��led Ӧ�ú�����  ;       
 * ʵ��ƽ̨��Ұ��STM32������;
 * Ӳ�����ӣ�-----------------;
=======
 * �ļ���  ��led.c;
 * ����    ��led Ӧ�ú�����   ;      
 * ʵ��ƽ̨��Ұ��STM32������;
 * Ӳ�����ӣ�-----------------;
>>>>>>> 335796abe5a527f8f02456764720a4efa9dfba1f
 *          |   PC1 - LED1     |
 *          |   PC3 - LED2     |
 *          |   PC13 - LED3     |
 *           ----------------- 
<<<<<<< HEAD
 * ��汾  ��ST3.5.0;
 * ����    ��wildfire team ;
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html;
 * �Ա�    ��http://firestm32.taobao.com;
=======
 * ��汾  ��ST3.5.0;
 * ����    ��wildfire team ;
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html;
 * �Ա�    ��http://firestm32.taobao.com;
>>>>>>> 335796abe5a527f8f02456764720a4efa9dfba1f
**********************************************************************************/
#include "led.h"

/*
<<<<<<< HEAD
 * ��������LED_GPIO_Config;
 * ����  ������LED�õ���I/O��;
 * ����  ����;
 * ���  ����;
=======
 * ��������LED_GPIO_Config
 * ����  ������LED�õ���I/O��;
 * ����  ����;
 * ���  ����;
>>>>>>> 335796abe5a527f8f02456764720a4efa9dfba1f
 */
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


void leds( void )
{

		LED1( ON );			  // ��;
		Delay(0x0FFFEF);
		Delay(0x0FFFEF);
		LED1( OFF );		  // ��;

		LED1( ON );			  // ��;
		Delay(0x0FFFEF);
		Delay(0x0FFFEF);
		LED1( OFF );		  // ��;

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


void Delay(__IO u32 nCount)	 //�򵥵���ʱ����;
{
	for(; nCount != 0; nCount--);
} 

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/