#include "ComInclude.h"


TPC_TASK TaskComps[]=
{
	{0, 250,  250, LED1on},			// 4*250=1s
//	{0, 250,  250, TaskLineLevel},
//	{0, 500,  500, Draw_Menu},
//	{0, 250,  250, TaskDrawNum},
//	{0, 5,	  5,   teesst},//没加任务....
//	{0, 250,  250, AdcValue},
	{0,	250,  250, UsartDmaTest},
	{0,	100,	100,  TaskDso},
	{0,	5,	  5,   DMAReadAdc},
//	{0,	25,	  25,  TdsoRun},

//	{0, 500,  500,  TaskLineVertical},
};



void TaskInit(void)
{
	LedGpioConfig();		//PC1,PC3,PC13
	SystickInit();
	Lcd_Init();				//PB9-PB10,pa4-pa7
//	Gui_DrawFont_GBK16(16,140,RED,GRAY0, "Welcome");
//	Draw_Menu();			//
	TIM3_PWM_init();		//pwm初始化 PB0,PB1
	KeyExit_pb7_config();	//按键初始化 PB7

#if USART1_DMA_RT
	UART1_Init();
#else
	uart_init(115200);		//串口初始化 115.2k;pa9,pa10
#endif
	InitDso();
//	TDsoInit();				//pb15
//	ADC1Init();				//PC.0
}



void TPCRemarks( TPC_TASK *pTask)
{
	u8 i;
	
	for ( i=0; i<TASKS_MAX; i++)
	{
		if( pTask[i].Timer > 0)
		{
			pTask[i].Timer--;
			if (pTask[i].Timer == 0)
			{
				pTask[i].Timer = pTask[i].ItvTime;
				pTask[i].Run = TPC_RUN_STM; 
			}
		}
	}
}

void TPCProcess(TPC_TASK *pTask)
{
	u8 i;

	for (i=0; i<TASKS_MAX; i++)
	{
		if (pTask[i].Run == TPC_RUN_STM)
		{
			pTask[i].TaskHook();
			pTask[i].Run = TPC_RUN_CLM;
		}

	}
}



