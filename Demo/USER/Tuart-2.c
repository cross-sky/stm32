#include "ComInclude.h"
#include <stdarg.h>	

uint8_t TxBuffer1[BufferSize]={'0','1','2','3','4','5','6','7'};
uint8_t TxBuffer2[BufferSize]={'0','1','2','3','4','5','6','7'};
uint8_t RxBuffer2[FIFO_SIZE]; /* Buffer emulating the FIFO */
int8_t RxBuffer2_SW[BufferSize]; /* Buffer used for final data store */
u8			USART1_TX_Finish=1;


void UART1_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	USART_InitTypeDef	USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(USART1_Tx_CLK|RCC_APB2Periph_AFIO|USART1_Tx_GPIO_CLK,ENABLE);
	//RCC_APB2PeriphClockCmd(USART1_Rx_CLK|USART1_Rx_GPIO,ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	/* Configure USART_Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = USART1_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1_Rx_GPIO,&GPIO_InitStructure);

	 /* Configure USART_Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = USART1_TxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART1_Tx_GPIO,&GPIO_InitStructure);

	DMA_DeInit(DMA1_Ch_Usart1_Tx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_Tx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = BufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Ch_Usart1_Tx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart1_Tx,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Ch_Usart1_Tx,DMA_IT_TE,ENABLE);
	USART_DMACmd(USART1_Tx, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart1_Tx,DISABLE);

	DMA_DeInit(DMA1_Ch_Usart1_Rx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_Rx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer2;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = FIFO_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	DMA_Init(DMA1_Ch_Usart1_Rx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart1_Rx,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Ch_Usart1_Rx,DMA_IT_TE,ENABLE);
	USART_DMACmd(USART1_Rx, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart1_Rx,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	  //Enable DMA Channel4 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart1_Tx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*Enable DMA Channel5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart1_Rx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1_Tx, &USART_InitStructure);

	//空闲中断
	USART_ITConfig(USART1_Tx, USART_IT_IDLE , ENABLE);
	USART_Cmd(USART1_Tx, ENABLE);
	USART_ClearFlag(USART1_Tx, USART_FLAG_TC);     //* 清发送外城标志，

}

void Usart1_Handle(void)
{
	u16 DATA_LEN;
	u16 i;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		DMA_Cmd(DMA1_Channel5, DISABLE);//关闭DMA,防止处理其间有数据
		//USART_RX_STA = USART1->SR;//先读SR，然后读DR才能清除
		//USART_RX_STA = USART1->DR;
		DATA_LEN=BufferSize-DMA_GetCurrDataCounter(DMA1_Channel5); 
		if(DATA_LEN > 0)
		{			
			while(USART1_TX_Finish==0)
			{
				;
			}
			//将数据送DMA存储地址
			for(i=0;i<DATA_LEN;i++)
			{
				TxBuffer1[i]=RxBuffer2[i];
			}
			//USART用DMA传输替代查询方式发送，克服被高优先级中断而产生丢帧现象。
			DMA_Cmd(DMA1_Channel4, DISABLE); //改变datasize前先要禁止通道工作
			DMA1_Channel4->CNDTR=DATA_LEN; //DMA1,传输数据量
			USART1_TX_Finish=0;//DMA传输开始标志量
			DMA_Cmd(DMA1_Channel4, ENABLE);			
		}
		//DMA_Cmd(DMA1_Channel5, DISABLE);//关闭DMA,防止处理其间有数据
		DMA_ClearFlag(DMA1_FLAG_GL5 | DMA1_FLAG_TC5 | DMA1_FLAG_TE5 | DMA1_FLAG_HT5);//清标志
		DMA1_Channel5->CNDTR = BufferSize;//重装填
		DMA_Cmd(DMA1_Channel5, ENABLE);//处理完,重开DMA
		//读SR后读DR清除Idle
		i = USART1->SR;
		i = USART1->DR;
	}
	if(USART_GetITStatus(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//出错
	{
		USART_ClearITPendingBit(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
	USART_ClearITPendingBit(USART1, USART_IT_TC);
	USART_ClearITPendingBit(USART1, USART_IT_IDLE);
}

void Dma1_Channel5Handle(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC5);
	DMA_ClearITPendingBit(DMA1_IT_TE5);
	DMA_Cmd(DMA1_Channel5, DISABLE);//关闭DMA,防止处理其间有数据
	DMA1_Channel5->CNDTR = FIFO_SIZE;//重装填
	DMA_Cmd(DMA1_Channel5, ENABLE);//处理完,重开DMA
}

void Dma1_Channel4Handle(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC4);
//	DMA_ClearFlag(DMA1_FLAG_TC4);
	DMA_ClearITPendingBit(DMA1_IT_TE4);
	DMA_Cmd(DMA1_Channel4, DISABLE);//关闭DMA
	DMA1_Channel4->CMAR = (uint32_t)TxBuffer1;
	USART1_TX_Finish=1;//置DMA传输完成
}

void teesst(void)
{
	static u8 i=1;
	
	#if TestEx
	TestExcuteTime(1);
		G_TestExcut=1;
#endif
	
		DMA_SetCurrDataCounter(DMA1_Channel4,i);
		DMA1_Channel4->CMAR = (uint32_t)TxBuffer2;
		DMA_Cmd(DMA1_Channel4,ENABLE);
		i++;
		if (i>5)
		{
			i=1;
		}
	
#if TestEx
	TestExcuteTime(0);
		G_TestExcut=0;
#endif
}

void DMASendAD(DSO_T *g_DSO)
{
	DMA_SetCurrDataCounter(DMA1_Channel4,10);
	DMA1_Channel4->CMAR = (uint32_t)&g_DSO->Ch1Buf;
	DMA_Cmd(DMA1_Channel4,ENABLE);

}
