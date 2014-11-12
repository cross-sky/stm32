
#ifndef __MPU_H
#define	__MPU_H
#include "stm32f10x.h"

void I2C_GPIO_Config(void);
void Init_MPU6050(void);
void READ_MPU6050(void);
void READ_MPU6050_Accel(void);
void Read_6050_Gyro(void);
void READ_MPU6050(void);


extern int16_t RwAcc[3],Gyro[3];
extern int16_t ax,ay,az,gx,gy,gz;
unsigned char who_am(void);

typedef enum {FALSE = 0, TRUE = !FALSE} bool;


#endif


