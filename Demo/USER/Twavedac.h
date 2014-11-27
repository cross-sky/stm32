
/*---------------------------------------------------------
		  		示波器基本功能模块子程序

 说明: 本程序完成示波器最基本的功能,包括增益的设置,时基的
 	   设置,波形的显示,获取波形的频率周期.还有信号发生器的
	   设置.

	   定义了3个十分重要的结构体,分别是

	   1) WaveMode结构体主要完成示波器显示波形的一些配置参数
	   2) WaveType结构体主要完成波形电参量和时间参量的储存,注意修饰符volatile
	   3) FG_WaveType结构体主要完成信号发生器参量信息的储存
	   4) FunctionGeneratorType枚举类型为了防止错误的赋值

---------------------------------------------------------*/

#ifndef __TWAVEDAC_H
#define __TWAVEDAC_H

/*--------------------------------------------------------
			波形配置数据结构,存储了波形配置参数
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
			波形数据结构,存储了波形的常用参数
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
			信号发生器波形数据结构
--------------------------------------------------------*/
typedef struct
{
	volatile float Vpp;
    volatile unsigned char DutyCycle;
    volatile unsigned int Frequency;
}FG_WaveType;
/*--------------------------------------------------------
				  函数发生器枚举类型
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

