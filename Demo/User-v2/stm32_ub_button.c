#include "cominc.h"


u8  BUTTON_ANZ =1;

BUTTON_t BUTTON[]={
  // Name    ,PORT , PIN                                            , CLOCK             , Status   ,0000 0111 1000 0000
	{BTN_USER, GPIOB, GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10, RCC_APB2Periph_GPIOB, 0, 0x0780},
};

#define BTN_MASK 0x0780 //

void UB_Button_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	BUTTON_NAME_t	 btn_name;

	for(btn_name=0; btn_name<BUTTON_ANZ; btn_name++)
	{
		RCC_APB2PeriphClockCmd(BUTTON[btn_name].BUTTON_CLK,ENABLE);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Pin = BUTTON[btn_name].BUTTON_PIN;
		GPIO_Init(BUTTON[btn_name].BUTTON_PORT, &GPIO_InitStructure);

	}
}



BUTTON_STATUS_t UB_Button_read(BUTTON_NAME_t btn_name)
{
	u16 value;
	static u16 Trg,Cont;
	u8 cnt_puls;
	//value = GPIO_ReadInputDataBit(BUTTON[btn_name].BUTTON_PORT,BUTTON[btn_name].BUTTON_PIN);
	
	value = (GPIO_ReadInputData(BUTTON[btn_name].BUTTON_PORT)&BUTTON[btn_name].BUTTON_MASK)
			^BUTTON[btn_name].BUTTON_MASK;
	Trg = value & (value ^ Cont);
	Cont = value;
	
	if (Trg & BTN_DOWN)
	{
	}
	if (value == Bit_RESET)
	{
		return(BTN_RELEASED);
	} 
	else
	{
		return(BTN_PRESSED);
	}
}


