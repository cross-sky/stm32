#include "cominc.h"


LED_t LED[]={
	// Name    ,PORT , PIN       , CLOCK              , Init
	{LED1 ,GPIOC,GPIO_Pin_1,RCC_APB2Periph_GPIOC,LED_OFF},   // 
	{LED3  ,GPIOC,GPIO_Pin_3,RCC_APB2Periph_GPIOC,LED_OFF},   // 
	{LED13  ,GPIOC,GPIO_Pin_13,RCC_APB2Periph_GPIOC,LED_OFF},
};

void UB_LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	u8 led_name;
	
	for (led_name=0; led_name < LED_ANZ; led_name++)
	{
		RCC_APB2PeriphClockCmd(LED[led_name].LED_CLK , ENABLE); 

		GPIO_InitStructure.GPIO_Pin = LED[led_name].LED_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(LED[led_name].LED_PORT, &GPIO_InitStructure);

		// Default Wert einstellen
		if(LED[led_name].LED_INIT==LED_OFF) {
			UB_Led_Off((LED_NAME_t)led_name);
		}
		else {
			UB_Led_On((LED_NAME_t)led_name);
		}
	}
}

void UB_Led_Off(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->BRR = LED[led_name].LED_PIN;
}

void UB_Led_On(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->BSRR = LED[led_name].LED_PIN;
} 

void UB_Led_Toggle(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->ODR ^= LED[led_name].LED_PIN;
}

void UB_Led_Switch(LED_NAME_t led_name, LED_STATUS_t status)
{
	if(status==LED_OFF) {
		UB_Led_Off(led_name);
	}
	else {
		UB_Led_On(led_name);
	}
}

void LED1on(void)
{
	UB_Led_Toggle(LED1);
}


// void LedGpioConfig(void)
// {		
// 	/*定义一个GPIO_InitTypeDef类型的结构体*/
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 
// 	/*开启GPIOC的外设时钟*/
// 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 
// 
// 	/*选择要控制的GPIOC引脚*/															   
// 	// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
// 
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_13;
// 
// 	/*设置引脚模式为通用推挽输出*/;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
// 
// 	/*设置引脚速率为50MHz */   
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
// 
// 
// 	/*调用库函数，初始化GPIOC*/;
// 	GPIO_Init(GPIOC, &GPIO_InitStructure);		  
// 
// 	/* 关闭所有led灯	*/;
// 
// 
// 	GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_13);	 
// }
// 
// 
// void LEDdisplay(u8 LightChannel)
// {
// 	switch(LightChannel)
// 	{
// 
// 		case 1:	GPIO_WriteBit( GPIOC, GPIO_Pin_1,
// 				( BitAction)( (1-GPIO_ReadOutputDataBit( GPIOC, GPIO_Pin_1))));
// 				break;
// 		case 3: GPIO_WriteBit( GPIOC, GPIO_Pin_3,
// 				( BitAction)( (1-GPIO_ReadOutputDataBit( GPIOC, GPIO_Pin_3))));
// 				break;
// 		case 13: GPIO_WriteBit( GPIOC, GPIO_Pin_13,
// 					( BitAction)( (1-GPIO_ReadOutputDataBit( GPIOC, GPIO_Pin_13))));
// 				break;
// 		default: break;
// 
// 	}
// 	
// 
// }
// 
// void LED1on(void)
// {
// #if TestEx
// 	TestExcuteTime(1);
// 	G_TestExcut=1;
// #endif
// 
// 	GPIO_WriteBit( GPIOC, GPIO_Pin_1,
// 		( BitAction)( (1-GPIO_ReadOutputDataBit( GPIOC, GPIO_Pin_1))));
// 
// #if TestEx
// 	TestExcuteTime(0);
// 	G_TestExcut=0;
// #endif
// }

