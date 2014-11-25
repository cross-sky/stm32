#include "ComInclude.h"

void Draw_Menu(void)
{
	FONT_T tFont;
	tFont.usFontCode = FC_ST_16X16;		/* 字体选择宋体16点阵，高16x宽15) */
	tFont.usTextColor = CL_WHITE;		/* 字体颜色设置为白色 */
	tFont.usBackColor = CL_BLUE;	 	/* 文字背景颜色，蓝色 */
	tFont.usSpace = 0;					/* 字符水平间距, 单位 = 像素 */
#if TestEx
	TestExcuteTime(1);
	G_TestExcut=1;
#endif

//	Lcd_Clear(LIGHTBLUE);
//	LED2(1);

	Gui_Circle(30,30,10,RED);
	Gui_DrawFont_GBK16(16,140, "Welcome",&tFont);
//  LED3(0);

#if TestEx
	TestExcuteTime(0);
		G_TestExcut=0;
#endif
}

void TaskLineLevel(void)
{
	static u16 vertical=0,level=0;
	u8 i= 40 ;
	static u8 flag=0;
	
	#if TestEx
	TestExcuteTime(1);
	G_TestExcut=1;
#endif


//	Lcd_Clear(YELLOW);
	if (flag==0)
	{
		if( level +i  > X_MAX_PIXEL)
		{
			level = 0;

			vertical += i;
			if ( vertical  > Y_MAX_PIXEL )
				vertical = 0;

			if((level==0) && (vertical==0))
				flag=1;
		}

		Gui_DrawLine(level,vertical,  level+i,  vertical,RED);

		level += i ;
	}
	else
	{
		if( level +i  > X_MAX_PIXEL)
		{
			level = 0;

			vertical += i;
			if ( vertical  > Y_MAX_PIXEL )
				vertical = 0;

			if((level==0) && (vertical==0))
				flag=0;
		}

		Gui_DrawLine(level,vertical,  level+i,  vertical,CL_BLUE);

		level += i ;

	}


#if TestEx
	TestExcuteTime(0);
	G_TestExcut=0;
#endif

}

void TaskDrawNum(void)
{
		static u8 i=0;
#if TestEx
	TestExcuteTime(1);
	G_TestExcut=1;
#endif


	Gui_DrawFont_Num16(120,20,RED,GRAY0,i);
	i++;
	if( i >14 ) i=0;

#if TestEx
	TestExcuteTime(0);
	G_TestExcut=0;
#endif
}

