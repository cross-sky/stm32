/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c;
 * ����    ��LED��ˮ�ƣ�Ƶ�ʿɵ�����  ;       
 * ʵ��ƽ̨��Ұ��STM32������;
 * ��汾  ��ST3.5.0;
 *
 * ����    ��wildfire team ;
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html;
 * �Ա�    ��http://firestm32.taobao.com;
*/
#include "ComInclude.h"

/*
 * ��������main;
 * ����  ��������;
 * ����  ����;
 * ���  ����;
 */
int main(void)
{	
	/* ��ʼ�� */;
//=======
	TaskInit();
	
	while (1)
	{
		TPCProcess(TaskComps);
		
	}
}




/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/