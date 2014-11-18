#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC1_DR_ADDRESS ( (u32)0x40012400+0x4c)
/*��ַд���ˣ�
��Ϊ���ADת������ļĴ����ĵ�ַ����0x4001244c;
ADC1�ǹ���APB2�����ϵġ� ;
APB2��ַ��0x40010000 ;
ADC1��APB2�ϵ�ƫ�ƣ�0x2400 ;
����ADC1�ĵ�ַ���ǣ�0x40012400 ;
*/
static __IO u16 s_adc1_converted_value[2];
/*�����������ADC_ConvertedValue[2],�ֱ��AD1ͨ��10��11ת��������;*/ 

// �ֲ����������ڴ��flash�����ĵ�ѹֵ;
static __IO u16 s_adc_temp_value;
////��ż������¶�ֵ;
static __IO u16 s_current_temp;
////�¶�Ϊ25���϶�ʱ�ĵ�ѹֵ;
static __IO u16 V25 = 0x6ee;

static __IO u16 avg_slop = 0x05;

extern void adc1_init( void);
 
extern float adc1_value( void );
extern u16 tempadc(void);



#endif

