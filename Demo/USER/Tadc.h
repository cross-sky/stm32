#ifndef __TADC_H
#define __TADC_H

#define ADC1_DR_ADDRESS ( (u32)0x40012400+0x4c)
/*地址写错了；
因为存放AD转换结果的寄存器的地址就是0x4001244c;
ADC1是挂在APB2总线上的。 ;
APB2地址：0x40010000 ;
ADC1在APB2上的偏移：0x2400 ;
这样ADC1的地址就是：0x40012400 ;
*/



void ADC1Init(void);
void AdcValue(void);
void AdcValue(void);
#endif


