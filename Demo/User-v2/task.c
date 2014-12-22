#include "cominc.h"


TPC_TASK TaskComps[]=
{
	{0, 0, 250,  250, LED1on},			// 4*250=1s   ok
	{0,	0,250,  250, UsartDmaTest},	//ok
	{0,	0,125,  125, MenusDisplay},	//主菜单函数，主任务
	{0,	0,12,  12,  ButtonRead},	//按键，主任务，
	{1,	1,10,	10,  DMAReadAdc},	//读取ad值，与TaskDso任务协同
	{1,	1,100,	100,  TaskDso},		//可挂起任务
	{0,	0,250,  250, MpuRead}		//可挂起任务
};



void TaskInit(void)
{
	UB_LED_Init();		//PC1,PC3,PC13  ok
	
	Lcd_Init();				//PB9-PB10,pa4-pa7  ok		
	TIM3_PWM_init();		//pwm初始化 PB0,PB1
	UB_Button_Init();	//按键初始化 PB7
	mpu6050m_init();

	UART1_Init();

	InitDso();
	InitMenuTree();  // 
	SystickInit();
}



void TPCRemarks( TPC_TASK *pTask)
{
	u8 i;
	
	for ( i=0; i<TASKS_MAX; i++)
	{
		if( pTask[i].Timer > 0 && pTask[i].Suspend == 0)
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
		if (pTask[i].Run == TPC_RUN_STM && pTask[i].Suspend == 0)
		{
			pTask[i].TaskHook();
			pTask[i].Run = TPC_RUN_CLM;
		}

	}
}



