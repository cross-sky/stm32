#include "ComInclude.h"


TPC_TASK TaskComps[]=
{
	{0, 1000, 1000, LED1on},
	{0, 1000,  1000,  TaskLineLevel},
	{0, 2000, 2000, Draw_Menu},
	{0,1000,  1000, TaskDrawNum},
//	{0, 500,  500,  TaskLineVertical},
};



void TaskInit(void)
{
	LedGpioConfig();
	SystickInit();
	Lcd_Init();
//	Gui_DrawFont_GBK16(16,140,RED,GRAY0, "Welcome");
	Draw_Menu();
	TIM3_PWM_init();
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



