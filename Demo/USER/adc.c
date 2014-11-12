#include "adc.h"

#define TEMPAD_EN 1



///addd000
//add1
/////

 
//__IO u16 ADC_ConvertedValue[2]; 

/*
 * 函数名：ADC1_GPIO_Config;
 * 描述  ：使能ADC1和DMA1的时钟，初始化PC.01;
 * 输入  : 无;
 * 输出  ：无;
 * 调用  ：内部调用;
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

/* 函数名：ADC1_Mode_Config
 * 描述  ：配置ADC1的工作模式为MDA模式;
 * 输入  : 无;
 * 输出  ：无;
 * 调用  ：内部调用;
 */

static void adc1_mode_config( void)
{
	DMA_InitTypeDef dma_init_structure;
	ADC_InitTypeDef adc_init_structure;

	/* 复位 ADC1,将与DMA有关的寄存器设我初始值; */ 
	DMA_DeInit( DMA1_Channel1);
	/*定义DMA外设基地址, 这里的ADC1_DR_Address 是用户自己定义的，即为存放转换结果的寄存器; 
	，他的作用就是告诉DMA取数就到ADC1_DR_Address 这里来取。;*/
	dma_init_structure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
	/*定义内存基地址，即告诉DMA要将从AD中取来的数放到ADC_ConvertedValue中 ;*/
	dma_init_structure.DMA_MemoryBaseAddr = ( uint32_t)&s_adc1_converted_value;
	/*定义AD外设作为数据传输的来源，即告诉DMA是将AD中的数据取出放到内存中，不能反过来;*/
	dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC;

	/*指定DMA通道的DMA缓存的大小,即告诉DMA开辟几个内存空间，由于我们只取通道10的AD数据所以只需开辟一个内存空间;*/
	dma_init_structure.DMA_BufferSize = 1;

	//dma_init_structure.DMA_BufferSize = 2;
	/*设定寄存器地址固定，即告诉DMA，只从固定的一个地方取数;*/ 
	dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/*设定内存地址固定，即每次DMA，，只将数搬到固定的内存中;*/ 
	dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	/*设定内存地址递加，即每次DMA都是将该外设寄存器中的值传到两个内存空间中;*/ 
	//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;


	/*设定外设数据宽度，即告诉DMA要取的数的大小,半字(16位);*/
	dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	 /*设定内存的的宽度;*/
	dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	/*设定DMA工作再循环缓存模式，即告诉DMA要不停的搬运，不能偷懒;*/ 
	dma_init_structure.DMA_Mode = DMA_Mode_Circular;
	 /*设定DMA选定的通道软件优先级;*/
	dma_init_structure.DMA_Priority = DMA_Priority_High;
	dma_init_structure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA1_Channel1, &dma_init_structure);

	DMA_Cmd( DMA1_Channel1, ENABLE);

	/* ADC1 configuration */
	/*设置ADC工作在独立模式;*/
	adc_init_structure.ADC_Mode = ADC_Mode_Independent;
	/*规定AD转换工作在扫描模式，即对多个通道采样;*/ 
	//ADC_InitStructure.ADC_ScanConvMode = ENABLE

	/*规定AD转换工作在单次模式，即对一个通道采样;*/
	adc_init_structure.ADC_ScanConvMode = DISABLE;
	/*设定AD转化在连续模式;*/
	adc_init_structure.ADC_ContinuousConvMode = ENABLE;
	/*不使用外部促发转换;*/
	adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/*采集的数据在寄存器中以右对齐的方式存放;*/
	adc_init_structure.ADC_DataAlign = ADC_DataAlign_Right;
	/*设定要转换的AD通道数目;*/
	adc_init_structure.ADC_NbrOfChannel = 1;
	//adc_init_structure.ADC_NbrOfChannel = 2;

	ADC_Init( ADC1, &adc_init_structure);
	/*配置ADC时钟，为PCLK2的8分频，即9MHz;*/ 
	RCC_ADCCLKConfig( RCC_PCLK2_Div8);
	/*配置ADC1的通道11为55.5个采样周期; */
	ADC_RegularChannelConfig( ADC1,ADC_Channel_10,1,ADC_SampleTime_55Cycles5);
	//ADC_RegularChannelConfig( ADC1,ADC_Channel_11,1,ADC_SampleTime_55Cycles5);


	ADC_DMACmd( ADC1, ENABLE);
	ADC_Cmd( ADC1, ENABLE);

	ADC_ResetCalibration( ADC1);

	while( ADC_GetResetCalibrationStatus( ADC1) );

	ADC_StartCalibration( ADC1);
	while( ADC_GetCalibrationStatus( ADC1));
	/* 由于没有采用外部触发，所以使用软件触发ADC转换; */ 
	ADC_SoftwareStartConvCmd( ADC1, ENABLE);


}


/* 函数名：ADC1_ModeN_Config
 * 描述  ：配置ADC1,temp的工作模式为MDA模式;
 * 输入  : 无;
 * 输出  ：无;
 * 调用  ：内部调用;
 */

static void adc1_modeN_config( void)
{
	DMA_InitTypeDef dma_init_structure;
	ADC_InitTypeDef adc_init_structure;

	/* 复位 ADC1,将与DMA有关的寄存器设我初始值; */ 
	DMA_DeInit( DMA1_Channel1);
	/*定义DMA外设基地址, 这里的ADC1_DR_Address 是用户自己定义的，即为存放转换结果的寄存器; 
	，他的作用就是告诉DMA取数就到ADC1_DR_Address 这里来取。;*/
	dma_init_structure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
	/*定义内存基地址，即告诉DMA要将从AD中取来的数放到ADC_ConvertedValue中 ;*/
	dma_init_structure.DMA_MemoryBaseAddr = ( uint32_t)&s_adc1_converted_value;
	/*定义AD外设作为数据传输的来源，即告诉DMA是将AD中的数据取出放到内存中，不能反过来;*/
	dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC;

	/*指定DMA通道的DMA缓存的大小,即告诉DMA开辟几个内存空间，由于我们只取通道10的AD数据所以只需开辟一个内存空间;*/
	dma_init_structure.DMA_BufferSize = 2;

	//dma_init_structure.DMA_BufferSize = 2;
	/*设定寄存器地址固定，即告诉DMA，只从固定的一个地方取数;*/ 
	dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/*设定内存地址固定，即每次DMA，，只将数搬到固定的内存中;*/ 
	dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	/*设定内存地址递加，即每次DMA都是将该外设寄存器中的值传到两个内存空间中;*/ 
	//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;


	/*设定外设数据宽度，即告诉DMA要取的数的大小,半字(16位);*/
	dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	 /*设定内存的的宽度;*/
	dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	/*设定DMA工作再循环缓存模式，即告诉DMA要不停的搬运，不能偷懒;*/ 
	dma_init_structure.DMA_Mode = DMA_Mode_Circular;
	 /*设定DMA选定的通道软件优先级;*/
	dma_init_structure.DMA_Priority = DMA_Priority_High;
	dma_init_structure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA1_Channel1, &dma_init_structure);

	DMA_Cmd( DMA1_Channel1, ENABLE);

	/* ADC1 configuration */
	/*设置ADC工作在独立模式;*/
	adc_init_structure.ADC_Mode = ADC_Mode_Independent;
	/*规定AD转换工作在扫描模式，即对多个通道采样;*/ 
	//ADC_InitStructure.ADC_ScanConvMode = ENABLE

	/*规定AD转换工作在单次模式，即对一个通道采样;*/
	adc_init_structure.ADC_ScanConvMode = ENABLE;
	/*设定AD转化在连续模式;*/
	adc_init_structure.ADC_ContinuousConvMode = ENABLE;
	/*不使用外部促发转换;*/
	adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/*采集的数据在寄存器中以右对齐的方式存放;*/
	adc_init_structure.ADC_DataAlign = ADC_DataAlign_Right;
	/*设定要转换的AD通道数目;*/
	adc_init_structure.ADC_NbrOfChannel = 2;
	//adc_init_structure.ADC_NbrOfChannel = 2;

	ADC_Init( ADC1, &adc_init_structure);
	/*配置ADC时钟，为PCLK2的8分频，即9MHz;*/ 
	RCC_ADCCLKConfig( RCC_PCLK2_Div8);
	/*配置ADC1的通道11为55.5个采样周期; */
	////ADC1,ADC通道x,规则采样顺序值为y,采样时间为55周期;
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
	/* 由于没有采用外部触发，所以使用软件触发ADC转换; */ 
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
