#ifndef __TASK_H
#define __TASK_H


//1.枚举定义任务列表;
//2.任务初始化;
//3.TaskComps增加任务初始化的数据;

#include "stm32f10x.h"

typedef struct _TPC_TASK 
{
	u8 Run;   //运行标记，0-不运行，1-运行；
	u16 Timer;  //计时器；
	u16 ItvTime;  // 任务运行间隔时间；
	void ( *TaskHook )(void);  //运行的任务函数；
} TPC_TASK;



typedef enum
{
	
	TPC_RUN_CLM = 0x00,  //任务停止;
	TPC_RUN_STM = 0x01  //任务运行;
	
} TPC_RUN_MASK;


//定义任务列表；
typedef enum 
{
	TASK_LED1ON,
	TASK_LINELEVEL,
	TASK_DRAWMENU,
	TASK_DRAWNUM,
	TASKS_MAX
} TASK_LIST;

extern void TaskInit(void);
extern void TPCRemarks( TPC_TASK *pTask);
extern void TPCProcess(TPC_TASK *pTask);
extern TPC_TASK TaskComps[];

#endif

