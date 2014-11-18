#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC1_DR_ADDRESS ( (u32)0x40012400+0x4c)
/*地址写错了；
因为存放AD转换结果的寄存器的地址就是0x4001244c;
ADC1是挂在APB2总线上的。 ;
APB2地址：0x40010000 ;
ADC1在APB2上的偏移：0x2400 ;
这样ADC1的地址就是：0x40012400 ;
*/
static __IO u16 s_adc1_converted_value[2];
/*定义数组变量ADC_ConvertedValue[2],分别放AD1通道10和11转换的数据;*/ 

// 局部变量，用于存从flash读到的电压值;
static __IO u16 s_adc_temp_value;
////存放计算后的温度值;
static __IO u16 s_current_temp;
////温度为25摄氏度时的电压值;
static __IO u16 V25 = 0x6ee;

static __IO u16 avg_slop = 0x05;

extern void adc1_init( void);
 
extern float adc1_value( void );
extern u16 tempadc(void);



#endif


