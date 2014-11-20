#ifndef __TUART_H
#define	__TUART_H

#include <stdio.h>

#define USART_REC_LEN  			0x0f  	//定义最大接收字节数 200;
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收;
#define EN_USART1_DMATx			1
#define USART1_DR_ADDR			(u32)(&USART1->DR)
#define USART1_DR_Base  0x40013804

extern u8	usart1_buf[ USART_REC_LEN ];
extern u8	usart1_recdata[ USART_REC_LEN ];
extern u8	usart1_rx ;
extern u8	usart1_tx ;
extern u8	usart1_len ;
extern u8	usart1_sta ;
extern u8	Flag_uart_send;

void uart_init( u32 bound );
void usart1_sendback( void );
void Usart1SendBackTest(void);
void UART1Receive(void);
void Usart1DmaSendTest(void);

#endif

