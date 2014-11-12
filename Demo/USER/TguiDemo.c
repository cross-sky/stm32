#include "ComInclude.h"

void Draw_Menu(void)
{
//	Lcd_Clear(LIGHTBLUE);
//	LED2(1);
	Gui_Circle(30,30,10,RED);
	Gui_DrawFont_GBK16(16,140,RED,GRAY0, "Welcome");
//  LED3(0);
}

void TaskLineLevel(void)
{

	static u16 vertical=0,level=0;
	u8 i= 40 ;
//	Lcd_Clear(YELLOW);
	if( level +i  > X_MAX_PIXEL)
	{
		level = 0;

		vertical += i;
		if ( vertical  > Y_MAX_PIXEL )
			vertical = 0;
		
		if((level==0) && (vertical==0))
			Lcd_Clear(YELLOW);
	}

	Gui_DrawLine(level,vertical,  level+i,  vertical,RED);
	
	level += i ;

}

void TaskDrawNum(void)
{
	static u8 i=0;
	Gui_DrawFont_Num16(120,20,RED,GRAY0,i);
	i++;
	if( i >14 ) i=0;
}

