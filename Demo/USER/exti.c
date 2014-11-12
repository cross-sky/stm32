#include "exti.h"

// 对应按键PB7，PB8，PB9

/*
 * 函数名：NVIC_Configuration;
 * 描述  ：配置嵌套向量中断控制器NVIC;
 * 输入  ：无;
 * 输出  ：无;
 * 调用  ：内部调用;
 */

static void nvic_configuration( void )
{
	NVIC_InitTypeDef nvic_init_structure;
	//优先级第一组;

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_1 );

	//配置EXTI第5-9线中断向量;
	nvic_init_structure.NVIC_IRQChannel = EXTI9_5_IRQn;

	nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
	//抢占优先级0-最高;
	nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;
	//响应优先级0-最高;

	nvic_init_structure.NVIC_IRQChannelSubPriority = 4;

	//nvic_init_structure.NVIC_IRQChannelSubPriority = 0;


	NVIC_Init( &nvic_init_structure );


}

/*
 * 函数名：EXTI_PB7_Config;
 * 描述  ：配置 PB7 为线中断口，并设置中断优先级;
 * 输入  ：无;
 * 输出  ：无;
 * 调用  ：外部调用;
 */

void exit_pb7_config( void )
{
	GPIO_InitTypeDef gpio_init_structure;
	EXTI_InitTypeDef exti_init_structure;

	//使能EXTIx线的时钟和第二功能AFIO时钟;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	//配置EXTIx线的中断优先级;
	nvic_configuration();

	//配置EXTI中断线IO
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IPU;  //按键接低电平的要选择上拉输入;
	gpio_init_structure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init( GPIOB, &gpio_init_structure );

	//选定要配置的EXTI的i/o口线，以及工作模式.;
	GPIO_EXTILineConfig( GPIO_PortSourceGPIOB, GPIO_PinSource7);

	exti_init_structure.EXTI_Line = EXTI_Line7;
	exti_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init_structure.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_init_structure.EXTI_LineCmd = ENABLE;

	EXTI_Init( &exti_init_structure );


}




