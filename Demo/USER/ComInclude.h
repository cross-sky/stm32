#ifndef __COMINCLUDE_H
#define __COMINCLUDE_H

#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "Tsystick.h"
#include "task.h"
#include "Tled.h"
#include "Tlcd_drive.h"
#include "Tgui.h"
#include "TguiDemo.h"
#include "Tpwm.h"

//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� ;
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� ;

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� ;
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� ;

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� ;
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� ;

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� ;
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� ;

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� ;
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����;

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� ;
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����;

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� ;
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����;


#endif
