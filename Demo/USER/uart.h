#ifndef __UART_H
#define	__UART_H

#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>

#define USART_REC_LEN  			0xff  	//�����������ֽ��� 200;
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����;


extern u8	usart1_buf[ USART_REC_LEN ];
extern u8	usart1_rx ;
extern u8	usart1_tx ;
extern u8	usart1_len ;
extern u8	usart1_sta ;

extern void uart_init( u32 bound );

extern void usart1_sendback( void );
#endif
