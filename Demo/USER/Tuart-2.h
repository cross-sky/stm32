#ifndef __TUART2_H
#define	__TUART2_H

#include <stdio.h>
#include"Tdso.h"

#define USART1_Rx				USART1
#define USART1_Rx_GPIO			GPIOA
#define USART1_Rx_CLK			RCC_APB2Periph_USART1
#define USART1_Rx_GPIO_CLK		RCC_APB2Periph_GPIOA
#define USART1_RxPin			GPIO_Pin_10

#define USART1_Tx				USART1
#define USART1_Tx_GPIO			GPIOA
#define USART1_Tx_CLK			RCC_APB2Periph_USART1
#define USART1_Tx_GPIO_CLK		RCC_APB2Periph_GPIOA
#define USART1_TxPin			GPIO_Pin_9

#define USART1_Tx_DR_Base		0x40013804
#define DMA1_Ch_Usart1_Tx		DMA1_Channel4
#define DMA1_Ch_Usart1_Tx_IRQn	DMA1_Channel4_IRQn

#define USART1_Rx_DR_Base		0x40013804
#define DMA1_Ch_Usart1_Rx		DMA1_Channel5
#define DMA1_Ch_Usart1_Rx_IRQn	DMA1_Channel5_IRQn

#define BufferSize             250
#define FIFO_SIZE              200

extern uint8_t TxBuffer1[BufferSize];
extern uint8_t RxBuffer2[FIFO_SIZE]; /* Buffer emulating the FIFO */
extern int8_t RxBuffer2_SW[BufferSize]; /* Buffer used for final data store */
extern u8 USART1_TX_Finish;


void UART1_Init(void);
void Usart1_Handle(void);
void Dma1_Channel5Handle(void);
void Dma1_Channel4Handle(void);
void teesst(void);
void DMASendAD(DSO_T *g_DSO);
#endif

