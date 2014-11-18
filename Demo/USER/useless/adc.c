#include "adc.h"

#define TEMPAD_EN 1



///addd000
//qww
//
///
//11-12-12550
 
//__IO u16 ADC_ConvertedValue[2]; 

/*
 * ��������ADC1_GPIO_Config;
 * ����  ��ʹ��ADC1��DMA1��ʱ�ӣ���ʼ��PC.01;
 * ����  : ��;
 * ���  ����;
 * ����  ���ڲ�����;
 */
static void adc1_gpio_config( void)
{
	GPIO_InitTypeDef gpio_init_structure;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE);
	//?/* Enable DMA clock */

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC, ENABLE);

		/* Configure PC.01  as analog input */
	gpio_init_structure.GPIO_Pin = GPIO_Pin_0;
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1; 

	gpio_init_structure.GPIO_Mode = GPIO_Mode_AIN;

	GPIO_Init( GPIOC, &gpio_init_structure);


}

/* ��������ADC1_Mode_Config
 * ����  ������ADC1�Ĺ���ģʽΪMDAģʽ;
 * ����  : ��;
 * ���  ����;
 * ����  ���ڲ�����;
 */

static void adc1_mode_config( void)
{
	DMA_InitTypeDef dma_init_structure;
	ADC_InitTypeDef adc_init_structure;

	/* ��λ ADC1,����DMA�йصļĴ������ҳ�ʼֵ; */ 
	DMA_DeInit( DMA1_Channel1);
	/*����DMA�������ַ, �����ADC1_DR_Address ���û��Լ�����ģ���Ϊ���ת������ļĴ���; 
	���������þ��Ǹ���DMAȡ���͵�ADC1_DR_Address ������ȡ��;*/
	dma_init_structure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
	/*�����ڴ����ַ��������DMAҪ����AD��ȡ�������ŵ�ADC_ConvertedValue�� ;*/
	dma_init_structure.DMA_MemoryBaseAddr = ( uint32_t)&s_adc1_converted_value;
	/*����AD������Ϊ���ݴ������Դ��������DMA�ǽ�AD�е�����ȡ���ŵ��ڴ��У����ܷ�����;*/
	dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC;

	/*ָ��DMAͨ����DMA����Ĵ�С,������DMA���ټ����ڴ�ռ䣬��������ֻȡͨ��10��AD��������ֻ�迪��һ���ڴ�ռ�;*/
	dma_init_structure.DMA_BufferSize = 1;

	//dma_init_structure.DMA_BufferSize = 2;
	/*�趨�Ĵ�����ַ�̶���������DMA��ֻ�ӹ̶���һ���ط�ȡ��;*/ 
	dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/*�趨�ڴ��ַ�̶�����ÿ��DMA����ֻ�����ᵽ�̶����ڴ���;*/ 
	dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	/*�趨�ڴ��ַ�ݼӣ���ÿ��DMA���ǽ�������Ĵ����е�ֵ���������ڴ�ռ���;*/ 
	//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;


	/*�趨�������ݿ��ȣ�������DMAҪȡ�����Ĵ�С,����(16λ);*/
	dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	 /*�趨�ڴ�ĵĿ���;*/
	dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	/*�趨DMA������ѭ������ģʽ��������DMAҪ��ͣ�İ��ˣ�����͵��;*/ 
	dma_init_structure.DMA_Mode = DMA_Mode_Circular;
	 /*�趨DMAѡ����ͨ���������ȼ�;*/
	dma_init_structure.DMA_Priority = DMA_Priority_High;
	dma_init_structure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA1_Channel1, &dma_init_structure);

	DMA_Cmd( DMA1_Channel1, ENABLE);

	/* ADC1 configuration */
	/*����ADC�����ڶ���ģʽ;*/
	adc_init_structure.ADC_Mode = ADC_Mode_Independent;
	/*�涨ADת��������ɨ��ģʽ�����Զ��ͨ������;*/ 
	//ADC_InitStructure.ADC_ScanConvMode = ENABLE

	/*�涨ADת�������ڵ���ģʽ������һ��ͨ������;*/
	adc_init_structure.ADC_ScanConvMode = DISABLE;
	/*�趨ADת��������ģʽ;*/
	adc_init_structure.ADC_ContinuousConvMode = ENABLE;
	/*��ʹ���ⲿ�ٷ�ת��;*/
	adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/*�ɼ��������ڼĴ��������Ҷ���ķ�ʽ���;*/
	adc_init_structure.ADC_DataAlign = ADC_DataAlign_Right;
	/*�趨Ҫת����ADͨ����Ŀ;*/
	adc_init_structure.ADC_NbrOfChannel = 1;
	//adc_init_structure.ADC_NbrOfChannel = 2;

	ADC_Init( ADC1, &adc_init_structure);
	/*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9MHz;*/ 
	RCC_ADCCLKConfig( RCC_PCLK2_Div8);
	/*����ADC1��ͨ��11Ϊ55.5����������; */
	ADC_RegularChannelConfig( ADC1,ADC_Channel_10,1,ADC_SampleTime_55Cycles5);
	//ADC_RegularChannelConfig( ADC1,ADC_Channel_11,1,ADC_SampleTime_55Cycles5);


	ADC_DMACmd( ADC1, ENABLE);
	ADC_Cmd( ADC1, ENABLE);

	ADC_ResetCalibration( ADC1);

	while( ADC_GetResetCalibrationStatus( ADC1) );

	ADC_StartCalibration( ADC1);
	while( ADC_GetCalibrationStatus( ADC1));
	/* ����û�в����ⲿ����������ʹ����������ADCת��; */ 
	ADC_SoftwareStartConvCmd( ADC1, ENABLE);


}


/* ��������ADC1_ModeN_Config
 * ����  ������ADC1,temp�Ĺ���ģʽΪMDAģʽ;
 * ����  : ��;
 * ���  ����;
 * ����  ���ڲ�����;
 */

static void adc1_modeN_config( void)
{
	DMA_InitTypeDef dma_init_structure;
	ADC_InitTypeDef adc_init_structure;

	/* ��λ ADC1,����DMA�йصļĴ������ҳ�ʼֵ; */ 
	DMA_DeInit( DMA1_Channel1);
	/*����DMA�������ַ, �����ADC1_DR_Address ���û��Լ�����ģ���Ϊ���ת������ļĴ���; 
	���������þ��Ǹ���DMAȡ���͵�ADC1_DR_Address ������ȡ��;*/
	dma_init_structure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
	/*�����ڴ����ַ��������DMAҪ����AD��ȡ�������ŵ�ADC_ConvertedValue�� ;*/
	dma_init_structure.DMA_MemoryBaseAddr = ( uint32_t)&s_adc1_converted_value;
	/*����AD������Ϊ���ݴ������Դ��������DMA�ǽ�AD�е�����ȡ���ŵ��ڴ��У����ܷ�����;*/
	dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC;

	/*ָ��DMAͨ����DMA����Ĵ�С,������DMA���ټ����ڴ�ռ䣬��������ֻȡͨ��10��AD��������ֻ�迪��һ���ڴ�ռ�;*/
	dma_init_structure.DMA_BufferSize = 2;

	//dma_init_structure.DMA_BufferSize = 2;
	/*�趨�Ĵ�����ַ�̶���������DMA��ֻ�ӹ̶���һ���ط�ȡ��;*/ 
	dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/*�趨�ڴ��ַ�̶�����ÿ��DMA����ֻ�����ᵽ�̶����ڴ���;*/ 
	dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	/*�趨�ڴ��ַ�ݼӣ���ÿ��DMA���ǽ�������Ĵ����е�ֵ���������ڴ�ռ���;*/ 
	//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;


	/*�趨�������ݿ��ȣ�������DMAҪȡ�����Ĵ�С,����(16λ);*/
	dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	 /*�趨�ڴ�ĵĿ���;*/
	dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	/*�趨DMA������ѭ������ģʽ��������DMAҪ��ͣ�İ��ˣ�����͵��;*/ 
	dma_init_structure.DMA_Mode = DMA_Mode_Circular;
	 /*�趨DMAѡ����ͨ���������ȼ�;*/
	dma_init_structure.DMA_Priority = DMA_Priority_High;
	dma_init_structure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA1_Channel1, &dma_init_structure);

	DMA_Cmd( DMA1_Channel1, ENABLE);

	/* ADC1 configuration */
	/*����ADC�����ڶ���ģʽ;*/
	adc_init_structure.ADC_Mode = ADC_Mode_Independent;
	/*�涨ADת��������ɨ��ģʽ�����Զ��ͨ������;*/ 
	//ADC_InitStructure.ADC_ScanConvMode = ENABLE

	/*�涨ADת�������ڵ���ģʽ������һ��ͨ������;*/
	adc_init_structure.ADC_ScanConvMode = ENABLE;
	/*�趨ADת��������ģʽ;*/
	adc_init_structure.ADC_ContinuousConvMode = ENABLE;
	/*��ʹ���ⲿ�ٷ�ת��;*/
	adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/*�ɼ��������ڼĴ��������Ҷ���ķ�ʽ���;*/
	adc_init_structure.ADC_DataAlign = ADC_DataAlign_Right;
	/*�趨Ҫת����ADͨ����Ŀ;*/
	adc_init_structure.ADC_NbrOfChannel = 2;
	//adc_init_structure.ADC_NbrOfChannel = 2;

	ADC_Init( ADC1, &adc_init_structure);
	/*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9MHz;*/ 
	RCC_ADCCLKConfig( RCC_PCLK2_Div8);
	/*����ADC1��ͨ��11Ϊ55.5����������; */
	////ADC1,ADCͨ��x,�������˳��ֵΪy,����ʱ��Ϊ55����;
	ADC_RegularChannelConfig( ADC1,ADC_Channel_10,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig( ADC1,ADC_Channel_16,2,ADC_SampleTime_55Cycles5);
	//ADC_RegularChannelConfig( ADC1,ADC_Channel_11,1,ADC_SampleTime_55Cycles5);

	ADC_TempSensorVrefintCmd( ENABLE);


	ADC_DMACmd( ADC1, ENABLE);
	ADC_Cmd( ADC1, ENABLE);

	ADC_ResetCalibration( ADC1);

	while( ADC_GetResetCalibrationStatus( ADC1) );

	ADC_StartCalibration( ADC1);
	while( ADC_GetCalibrationStatus( ADC1));
	/* ����û�в����ⲿ����������ʹ����������ADCת��; */ 
	ADC_SoftwareStartConvCmd( ADC1, ENABLE);


}

void adc1_init(void)
{
	adc1_gpio_config();

#if TEMPAD_EN
	adc1_modeN_config();
#else
	adc1_mode_config();
#endif

	

}

float adc1_value( void )
{
	static float adc_conver;
	adc_conver = (float) s_adc1_converted_value[0] / 4096 *3.3;
	return adc_conver;
}

u16 tempadc(void)
{
	s_adc_temp_value = s_adc1_converted_value[1];
	s_current_temp = ( V25 - s_adc_temp_value ) / avg_slop +25;
	return s_current_temp;
}

/*





*/