#ifndef __TADC_H
#define __TADC_H

#define ADC1_DR_ADDRESS ( (u32)0x40012400+0x4c)
/*��ַд���ˣ�
��Ϊ���ADת������ļĴ����ĵ�ַ����0x4001244c;
ADC1�ǹ���APB2�����ϵġ� ;
APB2��ַ��0x40010000 ;
ADC1��APB2�ϵ�ƫ�ƣ�0x2400 ;
����ADC1�ĵ�ַ���ǣ�0x40012400 ;
*/



void ADC1Init(void);
void AdcValue(void);
void AdcValue(void);
#endif

