#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"

extern u16 i2c_rx_buf[14]; 
extern int16_t accel_gyro[7];


#define I2C_SPEED		400000
#define I2C1_MPU6050	0xd0
#define MPU6050_DMA_CHANNEL DMA1_Channel7
#define MPU6050_I2C                  I2C1

#define MPU6050_ADDRESS_AD0_LOW     0x68 // address pin low (GND), default for InvenSense evaluation board
#define MPU6050_ADDRESS_AD0_HIGH    0x69 // address pin high (VCC)
#define MPU6050_DEFAULT_ADDRESS     (MPU6050_ADDRESS_AD0_LOW<<1)


#define I2C1_DR_ADDRESS ((u32)0x40005400 + 0x10)

/***********宏定义MPU6050内部寄存器地址****************/
#define	SMPLRT_DIV					0x19	//陀螺仪采样率，典型值：0x07(125Hz);
#define	CONFIG						0x1A	//低通滤波频率，典型值：0x06(5Hz);;
#define	GYRO_CONFIG					0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s);
#define	ACCEL_CONFIG				0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz);
#define	ACCEL_XOUT_H				0x3B
#define	ACCEL_XOUT_L				0x3C
#define	ACCEL_YOUT_H				0x3D
#define	ACCEL_YOUT_L				0x3E
#define	ACCEL_ZOUT_H				0x3F
#define	ACCEL_ZOUT_L				0x40
#define	TEMP_OUT_H					0x41
#define	TEMP_OUT_L					0x42
#define	GYRO_XOUT_H					0x43
#define	GYRO_XOUT_L					0x44	
#define	GYRO_YOUT_H					0x45
#define	GYRO_YOUT_L					0x46
#define	GYRO_ZOUT_H					0x47
#define	GYRO_ZOUT_L					0x48
#define	PWR_MGMT_1					0x6B	//电源管理，典型值：0x00(正常启用);
#define	WHO_AM_I					0x75	//IIC地址寄存器(默认数值0x68，只读);

//static void i2c_configuration(void);

extern void mpu6050_init(void);
extern void i2c_dma_read( u8 read_addr);
extern uint8_t I2C_ReadByte(uint8_t REG_Address);
#endif



