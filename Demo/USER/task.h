#ifndef __TASK_H
#define __TASK_H


//1.ö�ٶ��������б�;
//2.�����ʼ��;
//3.TaskComps���������ʼ��������;

#include "stm32f10x.h"

typedef struct _TPC_TASK 
{
	u8 Run;   //���б�ǣ�0-�����У�1-���У�
	u16 Timer;  //��ʱ����
	u16 ItvTime;  // �������м��ʱ�䣻
	void ( *TaskHook )(void);  //���е���������
} TPC_TASK;



typedef enum
{
	
	TPC_RUN_CLM = 0x00,  //����ֹͣ;
	TPC_RUN_STM = 0x01  //��������;
	
} TPC_RUN_MASK;


//���������б���
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
