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

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 ;
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 ;

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 ;
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 ;

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 ;
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 ;

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 ;
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 ;

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 ;
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入;

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 ;
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入;

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 ;
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入;


#endif

