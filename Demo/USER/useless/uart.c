#include "uart.h"
#include <stdarg.h>	


u8	usart1_buf[ USART_REC_LEN ];
u8	usart1_rx = 0;
u8	usart1_tx=0;
u8  usart1_len = 0;
u8	usart1_sta = 0;


//���ڣ������������ֻ��115.2k����ô��2M��GPIO�������ٶȾ͹��ˣ���ʡ��Ҳ����С��;
//I2C�ӿڣ�����ʹ��400k�����ʣ��������������Щ����ô��2M��GPIO�������ٶȻ�����������ʱ����ѡ��10M��GPIO�����ٶ�;
//SPI�ӿڣ�����ʹ��18M��9M�����ʣ���10M��GPIO�������ٶ���Ȼ�����ˣ���Ҫѡ��50M��GPIO�������ٶ�;

void uart_init( u32 bound )
{

	GPIO_InitTypeDef	gpio_init_structure;
	USART_InitTypeDef	usart_init_structure;
	NVIC_InitTypeDef	nvic_init_structure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE );
	USART_DeInit( USART1 );

	//pA.9 ����������� USART1_TX;
	gpio_init_structure.GPIO_Pin = GPIO_Pin_9;
	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOA, &gpio_init_structure );

	//PA.10 �������� usart1_RX;
	gpio_init_structure.GPIO_Pin = GPIO_Pin_10;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &gpio_init_structure );

	//usart ��ʼ������;
	usart_init_structure.USART_BaudRate = bound;
	usart_init_structure.USART_WordLength = USART_WordLength_8b;
	usart_init_structure.USART_StopBits = USART_StopBits_1;
	usart_init_structure.USART_Parity = USART_Parity_No;
	usart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART1, &usart_init_structure);

#if EN_USART1_RX
	//���ʹ���˽��գ����ý����ж�;
	nvic_init_structure.NVIC_IRQChannel = USART1_IRQn;
	nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_structure.NVIC_IRQChannelSubPriority = 1;
	nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &nvic_init_structure);
	//�����ж�;
	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE);
#endif
	USART_Cmd( USART1, ENABLE);

}

void USART1_SendByte(unsigned char temp)
{//TC�жϵ���һ֡���ݣ���TXE�ǵ�����д��DR�ͱ���λ��;
	USART_SendData(USART1, temp);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void usart1_sendback( void )
{
	u8 i;
	if ( usart1_sta != 0 )
	{
		for( i=0; i<usart1_len; i++,usart1_tx++)
			USART1_SendByte( usart1_buf[ usart1_tx]);
		usart1_tx = 0;
		usart1_rx = 0;
		usart1_sta = 0;
	}
}


/*
 * ��������fputc;
 * ����  ���ض���c�⺯��printf��USART1;
 * ����  ����;
 * ���  ����;
 * ����  ����printf����;
 */
int fputc(int ch, FILE *f)
{
	/* ��Printf���ݷ�������; */
	USART_SendData(USART1, (unsigned char) ch);
	while (!(USART1->SR & USART_FLAG_TXE));
	
	return (ch);
}

/*
 * ��������itoa;
 * ����  ������������ת�����ַ���;
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0;
 *         -value Ҫת����������;
 *         -buf ת������ַ���;
 *         -radix = 10;
 * ���  ����;
 * ����  ����;
 * ����  ����USART1_printf()����;
 */
static char *itoa(int value, char *string, int radix)
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;
	
	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
	    *ptr = 0;
	    return string;
	}
	
	if (!value)
	{
	    *ptr++ = 0x30;
	    *ptr = 0;
	    return string;
	}
	
	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
	    *ptr++ = '-';
	
	    /* Make the value positive. */
	    value *= -1;
	}
	
	for (i = 10000; i > 0; i /= 10)
	{
	    d = value / i;
	
	    if (d || flag)
	    {
	        *ptr++ = (char)(d + 0x30);
	        value -= (d * i);
	        flag = 1;
	    }
	}
	
	/* Null terminate the string. */
	*ptr = 0;
	
	return string;

} /* NCL_Itoa */

/*
 * ��������USART1_printf;
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��;
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���1����USART1;
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��;
 *			   -...    ��������;
 * ���  ����;
 * ����  ���� ;
 * ����  ���ⲿ����;
 *         ����Ӧ��USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *            		 USART1_printf( USART1, "\r\n %d \r\n", i );
 *            		 USART1_printf( USART1, "\r\n %s \r\n", j );
 */
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
	int d;   
	char buf[16];
	
	va_list ap;
	va_start(ap, Data);
	
	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������;
	{				                          
		if ( *Data == 0x5c )  //'\'
	{									  
	switch ( *++Data )
	{
		case 'r':							          //�س���;
			USART_SendData(USARTx, 0x0d);
			Data ++;
		break;
		
		case 'n':							          //���з�;
			USART_SendData(USARTx, 0x0a);	
			Data ++;
		break;
		
		default:
			Data ++;
		break;
	}			 
	}
	else if ( *Data == '%')
	{									  //
	switch ( *++Data )
	{				
		case 's':										  //�ַ���;
			s = va_arg(ap, const char *);
	for ( ; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
		Data++;
		break;
	
	case 'd':										//ʮ����;
	d = va_arg(ap, int);
	itoa(d, buf, 10);
	for (s = buf; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
	Data++;
	break;
		 default:
				Data++;
		    break;
	}		 
	} /* end of else if */
	else USART_SendData(USARTx, *Data++);
	while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}

/*

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	;  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ��� ;;                
struct __FILE 
{ 
int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    ;
_sys_exit(int x) 
{ 
x = x; 
} 
//�ض���fputc���� ;
int fputc(int ch, FILE *f)
{      
while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
USART_SendData(USART1,(uint8_t)ch);   
return ch;
}
#endif 

*/