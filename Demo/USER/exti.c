#include "exti.h"

// ��Ӧ����PB7��PB8��PB9

/*
 * ��������NVIC_Configuration;
 * ����  ������Ƕ�������жϿ�����NVIC;
 * ����  ����;
 * ���  ����;
 * ����  ���ڲ�����;
 */

static void nvic_configuration( void )
{
	NVIC_InitTypeDef nvic_init_structure;
	//���ȼ���һ��;

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_1 );

	//����EXTI��5-9���ж�����;
	nvic_init_structure.NVIC_IRQChannel = EXTI9_5_IRQn;

	nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
	//��ռ���ȼ�0-���;
	nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;
	//��Ӧ���ȼ�0-���;

	nvic_init_structure.NVIC_IRQChannelSubPriority = 4;

	//nvic_init_structure.NVIC_IRQChannelSubPriority = 0;


	NVIC_Init( &nvic_init_structure );


}

/*
 * ��������EXTI_PB7_Config;
 * ����  ������ PB7 Ϊ���жϿڣ��������ж����ȼ�;
 * ����  ����;
 * ���  ����;
 * ����  ���ⲿ����;
 */

void exit_pb7_config( void )
{
	GPIO_InitTypeDef gpio_init_structure;
	EXTI_InitTypeDef exti_init_structure;

	//ʹ��EXTIx�ߵ�ʱ�Ӻ͵ڶ�����AFIOʱ��;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	//����EXTIx�ߵ��ж����ȼ�;
	nvic_configuration();

	//����EXTI�ж���IO
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IPU;  //�����ӵ͵�ƽ��Ҫѡ����������;
	gpio_init_structure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init( GPIOB, &gpio_init_structure );

	//ѡ��Ҫ���õ�EXTI��i/o���ߣ��Լ�����ģʽ.;
	GPIO_EXTILineConfig( GPIO_PortSourceGPIOB, GPIO_PinSource7);

	exti_init_structure.EXTI_Line = EXTI_Line7;
	exti_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init_structure.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_init_structure.EXTI_LineCmd = ENABLE;

	EXTI_Init( &exti_init_structure );


}



