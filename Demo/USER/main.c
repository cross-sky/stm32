/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c;
 * 描述    ：LED流水灯，频率可调……  ;       
 * 实验平台：野火STM32开发板;
 * 库版本  ：ST3.5.0;
 *
 * 作者    ：wildfire team ;
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html;
 * 淘宝    ：http://firestm32.taobao.com;
*/
#include "ComInclude.h"

/*
 * 函数名：main;
 * 描述  ：主函数;
 * 输入  ：无;
 * 输出  ：无;
 */
int main(void)
{	
	/* 初始化 */;
//=======
	TaskInit();
	
	while (1)
	{
		TPCProcess(TaskComps);
		
	}
}




/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
