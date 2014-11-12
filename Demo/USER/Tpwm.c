#include "ComInclude.h"

static void TIM3_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	/*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&GPIO_Initstructure);

	/*GPIOB Configuration: TIM3 channel 3 and 4 as alternate function push-pull */
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;

	GPIO_Init(GPIOB,&GPIO_Initstructure);
}

static void TIM3_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitstructure;

	//如果是电机pwm调速，占空比会变化的;
	u16 ccr1_val = 500;
	u16 ccr2_val = 375;
	u16 ccr3_val = 250;
	u16 ccr4_val = 125;

	TIM_TimeBaseStructure.TIM_Period = 999;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitstructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstructure.TIM_Pulse = ccr1_val;
	TIM_OCInitstructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitstructure);
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);

	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstructure.TIM_Pulse = ccr2_val;
	TIM_OC2Init(TIM3, &TIM_OCInitstructure);
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);

	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstructure.TIM_Pulse = ccr3_val;
	TIM_OC3Init(TIM3, &TIM_OCInitstructure);
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);

	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstructure.TIM_Pulse = ccr4_val;
	TIM_OC4Init(TIM3, &TIM_OCInitstructure);
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3,ENABLE);

}

void TIM3_PWM_init(void)
{
	TIM3_GPIO_Config();
	TIM3_Mode_Config();
}


