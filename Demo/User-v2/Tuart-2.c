#include "cominc.h"
//#include <stdarg.h>	

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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|USART2_Tx_GPIO_CLK,ENABLE);
	RCC_APB1PeriphClockCmd(USART2_Tx_CLK,ENABLE);
	//RCC_APB2PeriphClockCmd(USART1_Rx_CLK|USART1_Rx_GPIO,ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	/* Configure USART_Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = USART2_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART2_Rx_GPIO,&GPIO_InitStructure);

	 /* Configure USART_Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = USART2_TxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART2_Tx_GPIO,&GPIO_InitStructure);

	DMA_DeInit(DMA1_Ch_Usart2_Tx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART2_Tx_DR_Base;
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

	DMA_Init(DMA1_Ch_Usart2_Tx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart2_Tx,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Ch_Usart2_Tx,DMA_IT_TE,ENABLE);
	USART_DMACmd(USART2_Tx, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart2_Tx,DISABLE);

	DMA_DeInit(DMA1_Ch_Usart2_Rx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART2_Rx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer2;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = FIFO_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	DMA_Init(DMA1_Ch_Usart2_Rx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart2_Rx,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Ch_Usart2_Rx,DMA_IT_TE,ENABLE);
	USART_DMACmd(USART2_Rx, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart2_Rx,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	  //Enable DMA Channel4 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart2_Tx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*Enable DMA Channel5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart2_Rx_IRQn;
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
	USART_Init(USART2_Tx, &USART_InitStructure);

	//�����ж�
	USART_ITConfig(USART2_Tx, USART_IT_IDLE , ENABLE);
	USART_Cmd(USART2_Tx, ENABLE);
	USART_ClearFlag(USART2_Tx, USART_FLAG_TC);     //* �巢����Ǳ�־��

}

void Usart2_Handle(void)
{
	u16 DATA_LEN;
	u16 i;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		DMA_Cmd(DMA1_Channel6, DISABLE);//�ر�DMA,��ֹ�������������
		//USART_RX_STA = USART1->SR;//�ȶ�SR��Ȼ���DR�������
		//USART_RX_STA = USART1->DR;
		DATA_LEN=BufferSize-DMA_GetCurrDataCounter(DMA1_Ch_Usart2_Rx); 
		if(DATA_LEN > 0)
		{			
			while(USART1_TX_Finish==0)
			{
				;
			}
			//��������DMA�洢��ַ
			for(i=0;i<DATA_LEN;i++)
			{
				TxBuffer1[i]=RxBuffer2[i];
			}
			//USART��DMA���������ѯ��ʽ���ͣ��˷��������ȼ��ж϶�������֡����
			DMA_Cmd(DMA1_Channel7, DISABLE); //�ı�datasizeǰ��Ҫ��ֹͨ������
			DMA1_Channel7->CNDTR=DATA_LEN; //DMA1,����������
			USART1_TX_Finish=0;//DMA���俪ʼ��־��
			DMA_Cmd(DMA1_Channel7, ENABLE);			
		}
		//DMA_Cmd(DMA1_Ch_Usart1_Rx, DISABLE);//�ر�DMA,��ֹ�������������
		DMA_ClearFlag(DMA1_FLAG_GL6 | DMA1_FLAG_TC6 | DMA1_FLAG_TE6 | DMA1_FLAG_HT6);//���־
		DMA1_Ch_Usart2_Rx->CNDTR = BufferSize;//��װ��
		DMA_Cmd(DMA1_Ch_Usart2_Rx, ENABLE);//������,�ؿ�DMA
		//��SR���DR���Idle
		i = USART2->SR;
		i = USART2->DR;
	}
	if(USART_GetITStatus(USART2, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//����
	{
		USART_ClearITPendingBit(USART2, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_ClearITPendingBit(USART2, USART_IT_IDLE);
}

void Dma1_Channel6Handle(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC6);
	DMA_ClearITPendingBit(DMA1_IT_TE6);
	DMA_Cmd(DMA1_Channel6, DISABLE);//�ر�DMA,��ֹ�������������
	DMA1_Channel6->CNDTR = FIFO_SIZE;//��װ��
	DMA_Cmd(DMA1_Channel6, ENABLE);//������,�ؿ�DMA
}

void Dma1_Channel7Handle(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC7);
//	DMA_ClearFlag(DMA1_FLAG_TC4);
	DMA_ClearITPendingBit(DMA1_IT_TE7);
	DMA_Cmd(DMA1_Channel7, DISABLE);//�ر�DMA
	DMA1_Channel7->CMAR = (uint32_t)TxBuffer1;
	USART1_TX_Finish=1;//��DMA�������
}

void UsartDmaTest(void)
{
	static u8 i=1;
	
	#if TestEx
		G_TestExcut=1;
#endif
	
		DMA_SetCurrDataCounter(DMA1_Channel7,i);
		DMA1_Channel7->CMAR = (uint32_t)TxBuffer2;
		DMA_Cmd(DMA1_Channel7,ENABLE);
		i++;
		if (i>5)
		{
			i=1;
		}
	
#if TestEx
		G_TestExcut=0;
#endif
}
