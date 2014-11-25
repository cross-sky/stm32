#include "ComInclude.h"

enum
{
	S_HELP		= 0,	/* 帮助页面 */
	S_RUN		= 1,	/* 采集状态 */
	S_HOLD		= 2		/* 暂停状态 */
};

/* 时间分度表， g_DSO.TimeBaseId作为数组索引  */
const u8 g_TimeTable[] =
{
	50, 	100, 	200	
};

const u16 TimeBuffSize[] = 
{
	4000,  2000, 1000
};

/* 衰减倍数表  
	g_DSO.Ch1Attenuation = 23
	幅度衰减因子:
	(1) 28 === 1V  (1:1)
*/
#define ATT_COUNT	6

const u16 g_AttTable[ATT_COUNT][2] =
{
	/* {除数,每大格电压}  1:1 */
	{1400,	5000},
	{560,	2000},
	{280,	1000},
	{140,	500},
	{56,	200},
	{28,	100},
};

DSO_T g_DSO;	/* 全局变量，是一个结构体 */

//static void DsoHelp(uint8_t *pMainStatus);
//static void DsoRun(uint8_t *pMainStatus);

static void InitDSO(void)
{
	/* 配置GPIO.  */
	
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef	ADC_InitStructure;

	/* 打开GPIO_C 和 AFIO 的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC	| RCC_APB2Periph_AFIO, ENABLE);

	/* 配置PC0为模拟输入模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* 使能 DMA1 时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);		/* 复位DMA1寄存器到缺省状态 */
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	/* 选择ADC1的数据寄存器作为源 */
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch1Buf;	/* 目标地址 */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	/* 设置DMA传输方向，外设(ADC)作为源 */
	DMA_InitStructure.DMA_BufferSize = g_DSO.TimeBuf;	/* 设置缓冲区大小 */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	/* 外设地址不自增 */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	/* 存储器地址需要自增 */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	/* 选择内存传输单位：16bit */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	/* 无需循环模式   */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	/* 选择DMA优先级 */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	/* DMA传输类型，不是内存到内存 */
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* 使能 DMA1 通道1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* 配置ADC1  */
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		/* 连续转换静止 */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;	/* 选择TIM1的CC3做触发 */

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	/* 数据右对齐,高位为0 */
	ADC_InitStructure.ADC_NbrOfChannel = 1;	/* 1个通道 */
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 规则通道配置 */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);

	/* 使能 ADC1 外部触发转换 */
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);

	/* 使能 ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* 使能 ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* 使能 ADC1 复位校准寄存器 */
	ADC_ResetCalibration(ADC1);
	/* 检测复位校准寄存器 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 开始 ADC1 校准 */
	ADC_StartCalibration(ADC1);
	/* 等待校准结束 */
	while(ADC_GetCalibrationStatus(ADC1));

	SetSampRate(g_DSO.SampleFreq);
}

/*
*********************************************************************************************************
*	函 数 名: SetSampRate
*	功能说明: 修改采样频率
*	形    参：freq : 采样频率 单位Hz
*	返 回 值: 无
*********************************************************************************************************
*/
static void SetSampRate(uint32_t freq)
{
	/* 第4步： 配置定时器2第2通道CC2, CC2作为ADC1的触发源 */
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	uint16_t Period;

	TIM_Cmd(TIM1, DISABLE);

	/*
	采样频率计算公式 ：
		period = 72000000 / freq ;
		
		1200 == 60KHz 采样频率
	*/
	Period = 72000 / freq -1;		

	/* 使能 TIM1 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Period = Period;          
	TIM_TimeBaseStructure.TIM_Prescaler = 0;   /* 计数频率 = 72000 000 / 18 = 4000 000	 */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* TIM1 通道1配置为PWM模式 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
	TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period / 2; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 计数器使能 */
	TIM_Cmd(TIM1, ENABLE);

	/* TIM1 输出使能 */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);  
}


/*
*********************************************************************************************************
*	函 数 名: DispHelp1
*	功能说明: 显示操作提示
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispHelp1(void)
{
	FONT_T tFont;		/* 定义一个字体结构体变量，用于设置字体参数 */
	uint16_t y;			/* Y坐标 */
	uint16_t usLineCap;	/* 行高 */

	Lcd_Clear(CL_BLUE);  		/* 清屏，背景蓝色 */

	/* 设置字体属性 */
	tFont.usFontCode = FC_ST_16X16;		/* 字体选择宋体16点阵，高16x宽15) */
	tFont.usTextColor = CL_WHITE;		/* 字体颜色设置为白色 */
	tFont.usBackColor = CL_BLUE;	 	/* 文字背景颜色，蓝色 */
	tFont.usSpace = 0;					/* 字符水平间距, 单位 = 像素 */

	y = 0;
	usLineCap = 18; /* 行间距 */
	Gui_DrawFont_GBK16(20, y, "QQ:1295744630", &tFont);
	y += usLineCap;

	y += usLineCap;
	Gui_DrawFont_GBK16(30, y, "xxxa", &tFont);
	y += usLineCap;
	Gui_DrawFont_GBK16(50, y, "oh my god", &tFont);
	y += usLineCap;
	Gui_DrawFont_GBK16(50, y, "wow", &tFont);

}

/*
*********************************************************************************************************
*	函 数 名: InitDsoParam
*	功能说明: 初始化全局参数变量
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDsoParam(void)
{
	g_DSO.Ch1Attenuation = 23;	/* 设置缺省衰减系数 */
//	g_DSO.Ch2Attenuation = 23;	/* 设置缺省衰减系数 */
	g_DSO.Ch1VScale = 1000;	/* 缺省是1V */
//	g_DSO.Ch2VScale = 1000;	/* 缺省是1V */

	g_DSO.Ch1VOffset = 70; 	/* 通道1 GND线位置 */
//	g_DSO.Ch2VOffset = 170; /* 通道2 GND线位置 */

	g_DSO.ActiveCH = 1;		/* 缺省是CH1 */
	g_DSO.AdjustMode = 1;	/* 缺省是调节垂直偏移， 可以切换到2调节幅度 */

	g_DSO.HoldEn = 0; 		/* 暂停刷屏标志 */

	g_DSO.TimeBaseId = 0;	/* 时间分度查询表索引 */								 
	g_DSO.TimeBase = 	g_TimeTable[g_DSO.TimeBaseId];	 /* 缺省时间分度 */
	g_DSO.SampleFreq = 2000 / g_DSO.TimeBase;  /* 计算采样频率 */
	g_DSO.TimeBuf = TimeBuffSize[g_DSO.TimeBaseId];

	g_DSO.Ch1AttId = 2;
	g_DSO.Ch1Attenuation = g_AttTable[g_DSO.Ch1AttId][0];
	g_DSO.Ch1VScale =  g_AttTable[g_DSO.Ch1AttId][1];

// 	g_DSO.Ch2AttId = 2;
// 	g_DSO.Ch2Attenuation = g_AttTable[g_DSO.Ch2AttId][0];
// 	g_DSO.Ch2VScale =  g_AttTable[g_DSO.Ch2AttId][1];

}


/*
*********************************************************************************************************
*	函 数 名: DispChInfo
*	功能说明: 显示通道信息
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispChInfo(void)
{
	char buf[32];   	/* 字符显示缓冲区 */
	FONT_T tFont;		/* 定义一个字体结构体变量，用于设置字体参数 */

	/* 设置字体属性 */
	tFont.usFontCode = FC_ST_16X16;		/* 字体选择宋体16点阵，高16x宽15) */
	tFont.usTextColor = CL_WHITE;		/* 字体颜色设置为白色 */
	tFont.usBackColor = CL_BLUE;	 	/* 文字背景颜色，蓝色 */
	tFont.usSpace = 0;					/* 字符水平间距, 单位 = 像素 */

	/* 显示示波器程序版本 */
	Gui_DrawFont_GBK16(10, 2, "v.0", &tFont);

	/* 显示通道1信息 */
	if (g_DSO.Ch1DC == 1)
	{
		strcpy(buf, "CH1 DC");
	}
	else
	{
		strcpy(buf, "CH1 AC ");
	}

	if (g_DSO.Ch1VScale >= 1000)
	{
		sprintf(&buf[7], "%d.00V", g_DSO.Ch1VScale / 1000);
	}
	else
	{
		sprintf(&buf[7], "%dmV", g_DSO.Ch1VScale);
	}

	if (g_DSO.ActiveCH == 1)
	{
		tFont.usTextColor = CL_YELLOW;		/* 设置字体颜色，黄色 */
		tFont.usBackColor = CL_MAGENTA;	 	/* 文字背景颜色，紫色 */
	}
	else
	{
		tFont.usTextColor = CL_YELLOW;		/* 设置字体颜色，黄色 */
		tFont.usBackColor = CL_BLUE;	 	/* 文字背景颜色，蓝色 */
	}

	Gui_DrawFont_GBK16(10, 224, buf, &tFont);

// 	/* 显示通道2信息 */
// 	if (g_DSO.Ch2DC == 1)
// 	{
// 		strcpy(buf, "CH2 DC ");
// 	}
// 	else
// 	{
// 		strcpy(buf, "CH2 AC ");
// 	}
// 
// 	if (g_DSO.Ch2VScale >= 1000)
// 	{
// 		sprintf(&buf[7], "%d.00V", g_DSO.Ch2VScale / 1000);
// 	}
// 	else
// 	{
// 		sprintf(&buf[7], "%dmV", g_DSO.Ch2VScale);
// 	}
// 	if (g_DSO.ActiveCH == 2)
// 	{
// 		tFont.usTextColor = CL_GREEN;		/* 设置字体颜色，绿色 */
// 		tFont.usBackColor = CL_MAGENTA;	 	/* 文字背景颜色，紫色 */
// 	}
// 	else
// 	{
// 		tFont.usTextColor = CL_GREEN;		/* 设置字体颜色，绿色 */
// 		tFont.usBackColor = CL_BLUE;	 	/* 文字背景颜色，蓝色 */
// 	}
// 	Gui_DrawFont_GBK16(120, 224, buf, &tFont);

	/* 显示时基 */
	tFont.usTextColor = CL_WHITE;		/* 设置字体颜色，白色 */
	tFont.usBackColor = CL_BLUE;	 	/* 文字背景颜色，蓝色 */

	/* 显示每格多长时间，即时间分度 */
	if (g_DSO.TimeBase < 1000)
	{
		sprintf(buf, "Time %3dus", g_DSO.TimeBase);
	}
	else if (g_DSO.TimeBase < 1000000)
	{
		sprintf(buf, "Time %3dms", g_DSO.TimeBase / 1000);
	}
	else
	{
		sprintf(buf, "Time %3ds ", g_DSO.TimeBase / 1000000);	
	}
	Gui_DrawFont_GBK16(230, 224, buf, &tFont);


	/* 显示上下键调节模式: 调节位置或者调节幅度 */
	tFont.usTextColor = CL_WHITE;		/* 设置字体颜色，白色 */
	tFont.usBackColor = CL_BLUE;	 	/* 文字背景颜色，蓝色 */
	if (g_DSO.AdjustMode == 1)
	{
		Gui_DrawFont_GBK16(245, 2, (uint8_t *)"Set", &tFont);
	}
	else
	{
		Gui_DrawFont_GBK16(245, 2, (uint8_t *)"Vm", &tFont);
	}

	/* 显示采样频率 */
	sprintf(buf, "f:%7dHz",	g_DSO.SampleFreq);
	Gui_DrawFont_GBK16(75, 2, buf, &tFont);
}

/*
*********************************************************************************************************
*	函 数 名: DispFrame
*	功能说明: 显示波形窗口的边框和刻度线
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispFrame(void)
{
	uint16_t x, y;

	/* 绘制一个实线矩形框 x, y, h, w */
	LCD_DrawRect(9, 19, 202, 302, CL_WHITE);

	/* 绘制垂直刻度点 */
	for (x = 0; x < 13; x++)
	{
		for (y = 0; y < 41; y++)
		{
			Gui_DrawPoint(10 + (x * 25), 20 + (y * 5), CL_WHITE);
		}
	}

	/* 绘制水平刻度点 */
	for (y = 0; y < 9; y++)
	{
		for (x = 0; x < 61; x++)
		{
			Gui_DrawPoint(10 + (x * 5), 20 + (y * 25), CL_WHITE);
		}
	}

	/* 绘制垂直中心刻度点 */
	for (y = 0; y < 41; y++)
	{	 
		Gui_DrawPoint(9 + (6 * 25), 20 + (y * 5), CL_WHITE);
		Gui_DrawPoint(11 + (6 * 25), 20 + (y * 5), CL_WHITE);
	}

	/* 绘制水平中心刻度点 */
	for (x = 0; x < 61; x++)
	{	 
		Gui_DrawPoint(10 + (x * 5), 19 + (4 * 25), CL_WHITE);
		Gui_DrawPoint(10 + (x * 5), 21 + (4 * 25), CL_WHITE);
	}
}


/*
*********************************************************************************************************
*	函 数 名: DispCh1Wave
*	功能说明: 显示通道1波形
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispCh1Wave(void)
{
	int16_t i;		/* 有符号数 */
	uint16_t *px;
	uint16_t *py;
	int16_t iTemp;

	static uint8_t s_DispFirst = 0;		/* 用于第一次调用时刷屏 */

	/* 显示通道1电平标记, 一个短的水平横线，在波形最左边 */
	{
		static uint16_t y = 70;

		Gui_DrawLine(1, y, 6, y, CL_BLUE);

		y = g_DSO.Ch1VOffset;

		if (y < 20)
		{
			y = 20;
		}
		else if (y > 220)
		{
			y = 220;
		}
		Gui_DrawLine(1, y, 6, y, CL_YELLOW);
	}

	if (s_DispFirst == 0)
	{
		/* 只在进入的时候清一次屏，以后不再清屏，否则会有严重的闪烁感 */
		s_DispFirst = 1;
//		Lcd_Clear(CL_BLUE);  	/* 清屏，背景蓝色 */
	}

	if (g_DSO.BufUsed == 0)
	{
		g_DSO.BufUsed = 1;
	}
	else
	{
		g_DSO.BufUsed = 0;
	}

	if (g_DSO.BufUsed == 0)
	{
		px = g_DSO.xCh1Buf1;
		py = g_DSO.yCh1Buf1;
	}
	else
	{
		px = g_DSO.xCh1Buf2;
		py = g_DSO.yCh1Buf2;
	}

	/* CH1通道, 输入0V时，ADC采样值 = 1872 ,这是根据多个开发板求到的平均值 */
	for (i = 0; i < 300; i++)
	{
		px[i] = 10 + i;
		iTemp = g_DSO.Ch1VOffset + (int16_t)((1872 - g_DSO.Ch1Buf[i + 1]) * 10) / g_DSO.Ch1Attenuation;

		if (iTemp > 220)
		{
			iTemp = 220;
		}
		else if (iTemp < 20)
		{
			iTemp = 20;
		}
		py[i] = iTemp;
	}

	/* 下面的代码采取清除上帧波形，然后再重新描绘新波形，避免整屏刷新的闪烁感 */
	/* 清除上帧波形 */
	if (g_DSO.BufUsed == 0)
	{
		LCD_DrawPoints(g_DSO.xCh1Buf2, g_DSO.yCh1Buf2, 300, CL_BLUE);
	}
	else
	{
		LCD_DrawPoints(g_DSO.xCh1Buf1, g_DSO.yCh1Buf1, 300, CL_BLUE);
	}

	/* 显示更新的波形 */
	LCD_DrawPoints((uint16_t *)px, (uint16_t *)py, 300, CL_YELLOW);
//	DMA_SetCurrDataCounter(DMA1_Channel4,10);
//		DMA1_Channel4->CNDTR = 10;
//	DMA1_Channel4->CMAR = (uint32_t)&px[0];
//	DMA_Cmd(DMA1_Channel4,ENABLE);
}

void TDsoInit(void)
{
	Lcd_Clear(CL_BLUE);
	InitDsoParam();
	InitDSO();
	DispFrame();
	DispChInfo();
}

void TdsoRun(void)
{
	#if TestEx
//	TestExcuteTime(1);
	G_TestExcut=1;
#endif
	

	DMA_Cmd(DMA1_Channel1, DISABLE);
	TIM_Cmd(TIM1, DISABLE);

	DispCh1Wave();
//	DMASendAD(&g_DSO);
//	DMA_SetCurrDataCounter(DMA1_Channel1,g_DSO.TimeBuf);
	DMA1_Channel1->CNDTR = g_DSO.TimeBuf;
	DMA_Cmd(DMA1_Channel1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
	
	#if TestEx
//	TestExcuteTime(1);
	G_TestExcut=0;
#endif
	
}

