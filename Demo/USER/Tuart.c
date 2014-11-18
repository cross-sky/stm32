#include "ComInclude.h"
#include <stdarg.h>	


u8	usart1_buf[ USART_REC_LEN ];
u8	usart1_rx = 0;
u8	usart1_tx=0;
u8  usart1_len = 0;
u8	usart1_sta = 0;
u8	usart1_recdata[ USART_REC_LEN ];

//串口，假如最大波特率只需115.2k，那么用2M的GPIO的引脚速度就够了，既省电也噪声小。;
//I2C接口，假如使用400k波特率，若想把余量留大些，那么用2M的GPIO的引脚速度或许不够，这时可以选用10M的GPIO引脚速度;
//SPI接口，假如使用18M或9M波特率，用10M的GPIO的引脚速度显然不够了，需要选用50M的GPIO的引脚速度;

void uart_init( u32 bound )
{

	GPIO_InitTypeDef	gpio_init_structure;
	USART_InitTypeDef	usart_init_structure;
	NVIC_InitTypeDef	nvic_init_structure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE );
	USART_DeInit( USART1 );

	//pA.9 复用推挽输出 USART1_TX;
	gpio_init_structure.GPIO_Pin = GPIO_Pin_9;
	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOA, &gpio_init_structure );

	//PA.10 浮空输入 usart1_RX;
	gpio_init_structure.GPIO_Pin = GPIO_Pin_10;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &gpio_init_structure );

	//usart 初始化设置;
	usart_init_structure.USART_BaudRate = bound;
	usart_init_structure.USART_WordLength = USART_WordLength_8b;
	usart_init_structure.USART_StopBits = USART_StopBits_1;
	usart_init_structure.USART_Parity = USART_Parity_No;
	usart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART1, &usart_init_structure);

#if EN_USART1_RX
	//如果使能了接收，配置接收中断;
	nvic_init_structure.NVIC_IRQChannel = USART1_IRQn;
	nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_structure.NVIC_IRQChannelSubPriority = 1;
	nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &nvic_init_structure);
	//开启中断;
	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE);
#endif
	USART_Cmd( USART1, ENABLE);

}

void USART1_SendByte(unsigned char temp)
{//TC判断的是一帧数据，而TXE是当数据写到DR就被置位了;
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
 * 函数名：fputc;
 * 描述  ：重定向c库函数printf到USART1;
 * 输入  ：无;
 * 输出  ：无;
 * 调用  ：由printf调用;
 */
int fputc(int ch, FILE *f)
{
	/* 将Printf内容发往串口; */
	USART_SendData(USART1, (unsigned char) ch);
	while (!(USART1->SR & USART_FLAG_TXE));
	
	return (ch);
}

/*
 * 函数名：itoa;
 * 描述  ：将整形数据转换成字符串;
 * 输入  ：-radix =10 表示10进制，其他结果为0;
 *         -value 要转换的整形数;
 *         -buf 转换后的字符串;
 *         -radix = 10;
 * 输出  ：无;
 * 返回  ：无;
 * 调用  ：被USART1_printf()调用;
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
 * 函数名：USART1_printf;
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库;
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1;
 *		     -Data   要发送到串口的内容的指针;
 *			   -...    其他参数;
 * 输出  ：无;
 * 返回  ：无 ;
 * 调用  ：外部调用;
 *         典型应用USART1_printf( USART1, "\r\n this is a demo \r\n" );
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
	
	while ( *Data != 0)     // 判断是否到达字符串结束符;
	{				                          
		if ( *Data == 0x5c )  //'\'
	{									  
	switch ( *++Data )
	{
		case 'r':							          //回车符;
			USART_SendData(USARTx, 0x0d);
			Data ++;
		break;
		
		case 'n':							          //换行符;
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
		case 's':										  //字符串;
			s = va_arg(ap, const char *);
	for ( ; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
		Data++;
		break;
	
	case 'd':										//十进制;
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
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	;  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数 ;;                
struct __FILE 
{ 
int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    ;
_sys_exit(int x) 
{ 
x = x; 
} 
//重定义fputc函数 ;
int fputc(int ch, FILE *f)
{      
while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
USART_SendData(USART1,(uint8_t)ch);   
return ch;
}
#endif 

*/

void UART1Receive(void)
{
	static u8 i=0;
	int8_t k;
	if ( USART_GetITStatus( USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit( USART1, USART_IT_RXNE);
		usart1_buf[i&0x0f] = USART_ReceiveData(USART1);

		if (usart1_buf[(i-10)&0x0f]==0xaa
			&&usart1_buf[(i-9)&0x0f]==0x55
			&&usart1_buf[i&0x0f]==0xa5
			&&usart1_sta==0)
		{
			for (k=7; k>=0; k--)
			{
				usart1_recdata[k] = usart1_buf[(--i)&0x0f];
			}

			usart1_sta = 1;
			i += 8;
		}
		i++;
	}

	 if ( USART_GetFlagStatus( USART1,USART_FLAG_ORE) == SET ) //溢出;
	 {
	 	USART_ClearFlag( USART1,USART_FLAG_ORE);  //读SR;
	 	USART_ReceiveData( USART1);  //读DR 
	 }
}

void Usart1SendBackTest(void)
{
		u8 i;
#if TestEx
	TestExcuteTime(1);
#endif

	if (usart1_sta == 1)
	{
		for (i=0; i<8; i++)
		{
			printf("%d",usart1_recdata[i]);
		}
		usart1_sta = 0;
	}

#if TestEx
	TestExcuteTime(0);
#endif

}
