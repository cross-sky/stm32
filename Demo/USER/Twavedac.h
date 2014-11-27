
/*---------------------------------------------------------
		  		ʾ������������ģ���ӳ���

 ˵��: ���������ʾ����������Ĺ���,�������������,ʱ����
 	   ����,���ε���ʾ,��ȡ���ε�Ƶ������.�����źŷ�������
	   ����.

	   ������3��ʮ����Ҫ�Ľṹ��,�ֱ���

	   1) WaveMode�ṹ����Ҫ���ʾ������ʾ���ε�һЩ���ò���
	   2) WaveType�ṹ����Ҫ��ɲ��ε������ʱ������Ĵ���,ע�����η�volatile
	   3) FG_WaveType�ṹ����Ҫ����źŷ�����������Ϣ�Ĵ���
	   4) FunctionGeneratorTypeö������Ϊ�˷�ֹ����ĸ�ֵ

---------------------------------------------------------*/

#ifndef __TWAVEDAC_H
#define __TWAVEDAC_H

/*--------------------------------------------------------
			�����������ݽṹ,�洢�˲������ò���
--------------------------------------------------------*/
typedef struct
{
//     unsigned char SavedSD;
// 	unsigned char ReadSD;
	unsigned char UpdatTrue;
    unsigned char Stop;
    unsigned char TriggerMode;
	unsigned char Gain;
	unsigned int  TimeBase;	
    float AmpDiv;
    float TimDiv;
}WaveMode;
/*--------------------------------------------------------
			�������ݽṹ,�洢�˲��εĳ��ò���
--------------------------------------------------------*/
typedef struct
{
    volatile unsigned int HighWidth;
    volatile unsigned int LowWidth;
    volatile float Vmax;
    volatile float Vmin;
	volatile float Vpp;
	volatile float Vrms;
    volatile unsigned int Priod;
    volatile unsigned int DutyCycle;
    volatile unsigned int Frequency;
}WaveType;
/*--------------------------------------------------------
			�źŷ������������ݽṹ
--------------------------------------------------------*/
typedef struct
{
	volatile float Vpp;
    volatile unsigned char DutyCycle;
    volatile unsigned int Frequency;
}FG_WaveType;
/*--------------------------------------------------------
				  ����������ö������
--------------------------------------------------------*/
typedef enum
{ 
  Sine_Wave = 1,
  Triangle_Wave, 
  Rectangular_Wave,
  Square_Wave,
  Sawtooth_Wave,
  Noise_Wave
}FunctionGeneratorType;


#endif
