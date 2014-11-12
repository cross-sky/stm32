#include "ComInclude.h"

u16 POINT_COLOR = 0x0000, BACK_COLOR = 0xffff;

static void TFT_SPI_Write_Byte( u8 num)
{
	u8 i = 0;
	for ( i=0; i<8; i++)
	{
		CLR_CLK();  //上升沿有效;
		if( num&0x80 ) 
			SET_SDA();
		else
			CLR_SDA();
		num <<= 1;
		SET_CLK();

	}
}

void LCD_WR_REG( u8 CMD )
{
	CLR_CS();
	CLR_CLK();
	CLR_SDA();
	SET_CLK();
	TFT_SPI_Write_Byte( CMD );
	SET_CS();

}

void LCD_WR_DATA(u8 dat )
{
	CLR_CS();
	CLR_CLK();
	SET_SDA();
	SET_CLK();
	TFT_SPI_Write_Byte(dat);
	SET_CS();
}

//开始写GRAM;
void Lcd_writeRam_Prepare(void)
{
	LCD_WR_REG( 0x2c );
}

//LCD开启显示;
void Lcd_Display(void)
{
	LCD_WR_REG(0x29); //26万色显示开启;
}

void Lcd_DisplayOff(void)
{
	LCD_WR_REG(0x28);////关闭显示;
}


//设置光标位置;
//Xpos:横坐标;
//Ypos:纵坐标;
__inline void Lcd_SetCursor( u16 Xpos, u16 Ypos)
{
#ifdef USE_HORIZONTAL
	LCD_WR_REG(0x2b);
	LCD_WR_DATA(Ypos>>8);
	LCD_WR_DATA(Ypos);
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(Xpos>>8);
	LCD_WR_DATA(Xpos);

#else
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(Xpos>>8 );
	LCD_WR_DATA(0xff&Xpos);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA(Ypos>>8);
	LCD_WR_DATA(0xff&Ypos);
#endif
}

//画点
//x:0~239
//y:0~319
//POINT_COLOR:此点的颜色;
void Lcd_DrawPoint(u16 x, u16 y, u16 color)
{
	Lcd_SetCursor(x,y);
	Lcd_writeRam_Prepare();
	LCD_WR_DATA(color>>8);
	LCD_WR_DATA(color);
}

static delay20ms(u16 tim)
{
	u16 i;
	while(tim--)
	{
		i=7200;
		while( i-- );
	}

}

void LcdInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	LCD_WR_REG(0xCF);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0xC1); 
	LCD_WR_DATA(0X30); 
	LCD_WR_REG(0xED);  
	LCD_WR_DATA(0x64); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0X12); 
	LCD_WR_DATA(0X81); 
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA(0x85); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x7A); 
	LCD_WR_REG(0xCB);  
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x34); 
	LCD_WR_DATA(0x02); 
	LCD_WR_REG(0xF7);  
	LCD_WR_DATA(0x20); 
	LCD_WR_REG(0xEA);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xC0);    //Power control 
	LCD_WR_DATA(0x1B);   //VRH[5:0] 
	LCD_WR_REG(0xC1);    //Power control 
	LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
	LCD_WR_REG(0xC5);    //VCM control 
	LCD_WR_DATA(0x30); 	 //3F
	LCD_WR_DATA(0x30); 	 //3C
	LCD_WR_REG(0xC7);    //VCM control2 
	LCD_WR_DATA(0XB7); 

	LCD_WR_REG(0x36);    // Memory Access Control 
	LCD_WR_DATA(0x08); 

	LCD_WR_REG(0x3A);   
	LCD_WR_DATA(0x55); 

	LCD_WR_REG(0xB1);   
	LCD_WR_DATA(0x00);   
	LCD_WR_DATA(0x1A); 

	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0xA2); 

	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA(0x01); 

	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x0E); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x54); 
	LCD_WR_DATA(0XA9); 
	LCD_WR_DATA(0x43); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 		 

	LCD_WR_REG(0XE1);    //Set Gamma 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x15); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x11); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x2B); 
	LCD_WR_DATA(0x56); 
	LCD_WR_DATA(0x3C); 
	LCD_WR_DATA(0x05); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_REG(0x2B); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);	 
	LCD_WR_REG(0x11); //Exit Sleep

	delay20ms(200);
	LCD_WR_REG(0x29);

	Lcd_Clear(GRED);
	
}


void Lcd_Clear(u16 color)
{
	u32 index = 0;
	Lcd_SetCursor(0x00,0x0000);
	Lcd_writeRam_Prepare();
	for(index=0; index<76800;index++)//240*320=76800
	{
		LCD_WR_DATA(color>>8);
		LCD_WR_DATA(color );
	}
}

//在指定区域内填充指定颜色;
//区域大小:;
//  (xend-xsta)*(yend-ysta)
void Lcd_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
	u16 i, j;
	u16 xlen = 0;

#if USE_HORIZONTAL == 1
	xlen = yend - ysta + 1;
	for (i=xsta;i<xend; i++)
	{
		Lcd_SetCursor(i,ysta);
		Lcd_writeRam_Prepare();
		for (j=0; j<xlen; j++)
		{
			LCD_WR_DATA(color>>8);
			LCD_WR_DATA(color);
		}
	}
#else
	xlen = xend - xsta +1;
	for (i=ysta; i<yend; i++)
	{
		Lcd_SetCursor(xsta, i);
		Lcd_writeRam_Prepare();
		for (j=0; j<xlen; j++)
		{
			LCD_WR_DATA(color>>8);
			LCD_WR_DATA(color);
		}
	}
#endif
}

//画线
//x1,y1:起点坐标;
//x2,y2:终点坐标 ;
void Lcd_drawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t;
	int xerr=0, yerr=0, delta_x,delta_y,distance;
	int incx, incy,uRow,uCol;

	delta_x = x2 - x1;
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;

	if(delta_x>0) 
		incx=1;
	else if( delta_x == 0)
		incx=0;
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}

	if(delta_y>0)
		incy = 1;
	else if( delta_y == 0)
		incy=0;
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}

	if(delta_x > delta_y)
		distance = delta_x;
	else
		distance = delta_y;

	for (t=0; t<=distance; t++)
	{
		Lcd_DrawPoint(uRow,uCol,color);
		xerr += delta_x;
		yerr += delta_y;

		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}

		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}

}

void TaskLineVertical(void)
{
	static u16 vertical=0,level=0;
	u8 i=2;

	if( vertical + i > LCD_H )
	{
		vertical = 0;

		level += i;

		if ( level > LCD_W )
			level = 0;
		
	}
	Lcd_drawLine(vertical, vertical+i, level, level,LIGHTGREEN);
	
}

void TaskLineLevel(void)
{
	static u16 vertical=0,level=0;
	u8 i=2;

	if( level +i  > LCD_W)
	{
		level = 0;

		vertical += i;
		if ( vertical  > LCD_H )
			vertical = 0;
	}

	Lcd_drawLine(vertical, vertical, level, level+i,LIGHTBLUE);
	

}



