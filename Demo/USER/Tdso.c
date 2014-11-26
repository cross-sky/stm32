#include "ComInclude.h"

enum
{
	S_HELP		= 0,	/* ����ҳ�� */
	S_RUN		= 1,	/* �ɼ�״̬ */
	S_HOLD		= 2		/* ��ͣ״̬ */
};

/* ʱ��ֶȱ��� g_DSO.TimeBaseId��Ϊ��������  */
const u8 g_TimeTable[] =
{
	50, 	100, 	200	
};

const u16 TimeBuffSize[] = 
{
	4000,  2000, 1000
};

/* ˥��������  
	g_DSO.Ch1Attenuation = 23
	����˥������:
	(1) 28 === 1V  (1:1)
*/
#define ATT_COUNT	6

const u16 g_AttTable[ATT_COUNT][2] =
{
	/* {����,ÿ����ѹ}  1:1 */
	{1400,	5000},
	{560,	2000},
	{280,	1000},
	{140,	500},
	{56,	200},
	{28,	100},
};

DSO_T g_DSO;	/* ȫ�ֱ�������һ���ṹ�� */

//static void DsoHelp(uint8_t *pMainStatus);
//static void DsoRun(uint8_t *pMainStatus);

static void InitDSO(void)
{
	/* ����GPIO.  */
	
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef	ADC_InitStructure;

	/* ��GPIO_C �� AFIO ��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC	| RCC_APB2Periph_AFIO, ENABLE);

	/* ����PC0Ϊģ������ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ʹ�� DMA1 ʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);		/* ��λDMA1�Ĵ�����ȱʡ״̬ */
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	/* ѡ��ADC1�����ݼĴ�����ΪԴ */
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch1Buf;	/* Ŀ���ַ */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	/* ����DMA���䷽������(ADC)��ΪԴ */
	DMA_InitStructure.DMA_BufferSize = g_DSO.TimeBuf;	/* ���û�������С */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	/* �����ַ������ */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	/* �洢����ַ��Ҫ���� */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* ѡ�����贫�䵥λ��16bit */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	/* ѡ���ڴ洫�䵥λ��16bit */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	/* ����ѭ��ģʽ   */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	/* ѡ��DMA���ȼ� */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	/* DMA�������ͣ������ڴ浽�ڴ� */
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* ʹ�� DMA1 ͨ��1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* ����ADC1  */
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		/* ����ת����ֹ */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;	/* ѡ��TIM1��CC3������ */

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	/* �����Ҷ���,��λΪ0 */
	ADC_InitStructure.ADC_NbrOfChannel = 1;	/* 1��ͨ�� */
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 ����ͨ������ */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);

	/* ʹ�� ADC1 �ⲿ����ת�� */
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);

	/* ʹ�� ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* ʹ�� ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* ʹ�� ADC1 ��λУ׼�Ĵ��� */
	ADC_ResetCalibration(ADC1);
	/* ��⸴λУ׼�Ĵ��� */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* ��ʼ ADC1 У׼ */
	ADC_StartCalibration(ADC1);
	/* �ȴ�У׼���� */
	while(ADC_GetCalibrationStatus(ADC1));

	SetSampRate(g_DSO.SampleFreq);
}

/*
*********************************************************************************************************
*	�� �� ��: SetSampRate
*	����˵��: �޸Ĳ���Ƶ��
*	��    �Σ�freq : ����Ƶ�� ��λHz
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void SetSampRate(uint32_t freq)
{
	/* ��4���� ���ö�ʱ��2��2ͨ��CC2, CC2��ΪADC1�Ĵ���Դ */
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	uint16_t Period;

	TIM_Cmd(TIM1, DISABLE);

	/*
	����Ƶ�ʼ��㹫ʽ ��
		period = 72000000 / freq ;
		
		1200 == 60KHz ����Ƶ��
	*/
	Period = 72000 / freq -1;		

	/* ʹ�� TIM1 ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Period = Period;          
	TIM_TimeBaseStructure.TIM_Prescaler = 0;   /* ����Ƶ�� = 72000 000 / 18 = 4000 000	 */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* TIM1 ͨ��1����ΪPWMģʽ */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
	TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period / 2; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 ������ʹ�� */
	TIM_Cmd(TIM1, ENABLE);

	/* TIM1 ���ʹ�� */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);  
}


/*
*********************************************************************************************************
*	�� �� ��: DispHelp1
*	����˵��: ��ʾ������ʾ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispHelp1(void)
{
	FONT_T tFont;		/* ����һ������ṹ���������������������� */
	uint16_t y;			/* Y���� */
	uint16_t usLineCap;	/* �и� */

	Lcd_Clear(CL_BLUE);  		/* ������������ɫ */

	/* ������������ */
	tFont.usFontCode = FC_ST_16X16;		/* ����ѡ������16���󣬸�16x��15) */
	tFont.usTextColor = CL_WHITE;		/* ������ɫ����Ϊ��ɫ */
	tFont.usBackColor = CL_BLUE;	 	/* ���ֱ�����ɫ����ɫ */
	tFont.usSpace = 0;					/* �ַ�ˮƽ���, ��λ = ���� */

	y = 0;
	usLineCap = 18; /* �м�� */
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
*	�� �� ��: InitDsoParam
*	����˵��: ��ʼ��ȫ�ֲ�������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDsoParam(void)
{
	g_DSO.Ch1Attenuation = 23;	/* ����ȱʡ˥��ϵ�� */
//	g_DSO.Ch2Attenuation = 23;	/* ����ȱʡ˥��ϵ�� */
	g_DSO.Ch1VScale = 1000;	/* ȱʡ��1V */
//	g_DSO.Ch2VScale = 1000;	/* ȱʡ��1V */

	g_DSO.Ch1VOffset = 70; 	/* ͨ��1 GND��λ�� */
//	g_DSO.Ch2VOffset = 170; /* ͨ��2 GND��λ�� */

	g_DSO.ActiveCH = 1;		/* ȱʡ��CH1 */
	g_DSO.AdjustMode = 1;	/* ȱʡ�ǵ��ڴ�ֱƫ�ƣ� �����л���2���ڷ��� */

	g_DSO.HoldEn = 0; 		/* ��ͣˢ����־ */

	g_DSO.TimeBaseId = 0;	/* ʱ��ֶȲ�ѯ������ */								 
	g_DSO.TimeBase = 	g_TimeTable[g_DSO.TimeBaseId];	 /* ȱʡʱ��ֶ� */
	g_DSO.SampleFreq = 2000 / g_DSO.TimeBase;  /* �������Ƶ�� */
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
*	�� �� ��: DispChInfo
*	����˵��: ��ʾͨ����Ϣ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispChInfo(void)
{
	char buf[32];   	/* �ַ���ʾ������ */
	FONT_T tFont;		/* ����һ������ṹ���������������������� */

	/* ������������ */
	tFont.usFontCode = FC_ST_16X16;		/* ����ѡ������16���󣬸�16x��15) */
	tFont.usTextColor = CL_WHITE;		/* ������ɫ����Ϊ��ɫ */
	tFont.usBackColor = CL_BLUE;	 	/* ���ֱ�����ɫ����ɫ */
	tFont.usSpace = 0;					/* �ַ�ˮƽ���, ��λ = ���� */

	/* ��ʾʾ��������汾 */
	Gui_DrawFont_GBK16(10, 2, "v.0", &tFont);

	/* ��ʾͨ��1��Ϣ */
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
		tFont.usTextColor = CL_YELLOW;		/* ����������ɫ����ɫ */
		tFont.usBackColor = CL_MAGENTA;	 	/* ���ֱ�����ɫ����ɫ */
	}
	else
	{
		tFont.usTextColor = CL_YELLOW;		/* ����������ɫ����ɫ */
		tFont.usBackColor = CL_BLUE;	 	/* ���ֱ�����ɫ����ɫ */
	}

	Gui_DrawFont_GBK16(10, 224, buf, &tFont);

// 	/* ��ʾͨ��2��Ϣ */
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
// 		tFont.usTextColor = CL_GREEN;		/* ����������ɫ����ɫ */
// 		tFont.usBackColor = CL_MAGENTA;	 	/* ���ֱ�����ɫ����ɫ */
// 	}
// 	else
// 	{
// 		tFont.usTextColor = CL_GREEN;		/* ����������ɫ����ɫ */
// 		tFont.usBackColor = CL_BLUE;	 	/* ���ֱ�����ɫ����ɫ */
// 	}
// 	Gui_DrawFont_GBK16(120, 224, buf, &tFont);

	/* ��ʾʱ�� */
	tFont.usTextColor = CL_WHITE;		/* ����������ɫ����ɫ */
	tFont.usBackColor = CL_BLUE;	 	/* ���ֱ�����ɫ����ɫ */

	/* ��ʾÿ��೤ʱ�䣬��ʱ��ֶ� */
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


	/* ��ʾ���¼�����ģʽ: ����λ�û��ߵ��ڷ��� */
	tFont.usTextColor = CL_WHITE;		/* ����������ɫ����ɫ */
	tFont.usBackColor = CL_BLUE;	 	/* ���ֱ�����ɫ����ɫ */
	if (g_DSO.AdjustMode == 1)
	{
		Gui_DrawFont_GBK16(245, 2, (uint8_t *)"Set", &tFont);
	}
	else
	{
		Gui_DrawFont_GBK16(245, 2, (uint8_t *)"Vm", &tFont);
	}

	/* ��ʾ����Ƶ�� */
	sprintf(buf, "f:%7dHz",	g_DSO.SampleFreq);
	Gui_DrawFont_GBK16(75, 2, buf, &tFont);
}

/*
*********************************************************************************************************
*	�� �� ��: DispFrame
*	����˵��: ��ʾ���δ��ڵı߿�Ϳ̶���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispFrame(void)
{
	uint16_t x, y;

	/* ����һ��ʵ�߾��ο� x, y, h, w */
	LCD_DrawRect(9, 19, 202, 302, CL_WHITE);

	/* ���ƴ�ֱ�̶ȵ� */
	for (x = 0; x < 13; x++)
	{
		for (y = 0; y < 41; y++)
		{
			Gui_DrawPoint(10 + (x * 25), 20 + (y * 5), CL_WHITE);
		}
	}

	/* ����ˮƽ�̶ȵ� */
	for (y = 0; y < 9; y++)
	{
		for (x = 0; x < 61; x++)
		{
			Gui_DrawPoint(10 + (x * 5), 20 + (y * 25), CL_WHITE);
		}
	}

	/* ���ƴ�ֱ���Ŀ̶ȵ� */
	for (y = 0; y < 41; y++)
	{	 
		Gui_DrawPoint(9 + (6 * 25), 20 + (y * 5), CL_WHITE);
		Gui_DrawPoint(11 + (6 * 25), 20 + (y * 5), CL_WHITE);
	}

	/* ����ˮƽ���Ŀ̶ȵ� */
	for (x = 0; x < 61; x++)
	{	 
		Gui_DrawPoint(10 + (x * 5), 19 + (4 * 25), CL_WHITE);
		Gui_DrawPoint(10 + (x * 5), 21 + (4 * 25), CL_WHITE);
	}
}


/*
*********************************************************************************************************
*	�� �� ��: DispCh1Wave
*	����˵��: ��ʾͨ��1����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispCh1Wave(void)
{
	int16_t i;		/* �з����� */
	uint16_t *px;
	uint16_t *py;
	int16_t iTemp;

	static uint8_t s_DispFirst = 0;		/* ���ڵ�һ�ε���ʱˢ�� */

	/* ��ʾͨ��1��ƽ���, һ���̵�ˮƽ���ߣ��ڲ�������� */
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
		/* ֻ�ڽ����ʱ����һ�������Ժ�������������������ص���˸�� */
		s_DispFirst = 1;
//		Lcd_Clear(CL_BLUE);  	/* ������������ɫ */
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

	/* CH1ͨ��, ����0Vʱ��ADC����ֵ = 1872 ,���Ǹ��ݶ���������󵽵�ƽ��ֵ */
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

	/* ����Ĵ����ȡ�����֡���Σ�Ȼ������������²��Σ���������ˢ�µ���˸�� */
	/* �����֡���� */
	if (g_DSO.BufUsed == 0)
	{
		LCD_DrawPoints(g_DSO.xCh1Buf2, g_DSO.yCh1Buf2, 300, CL_BLUE);
	}
	else
	{
		LCD_DrawPoints(g_DSO.xCh1Buf1, g_DSO.yCh1Buf1, 300, CL_BLUE);
	}

	/* ��ʾ���µĲ��� */
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
