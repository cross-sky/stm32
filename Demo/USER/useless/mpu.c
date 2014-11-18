#include "mpu.h"
//#include "led.h"
#include  <math.h> 
#include "stm32f10x.h"

// 定义MPU6050内部地址
//****************************************
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

#define INT_PIN_CFG     0x37    //设置旁路有效 打开值：0x42 AUX_DA的辅助I2C
#define USER_CTRL       0x6A    //用户配置寄存器 打开值：0x40  AUX_DA的辅助I2C

#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
//#define	WHO_AM_I		0x75	//IIC地址寄存器(默认读出数值0x68，只读)

//IIC写入时的地址字节数据，+1为读取
#define	MPU6050_Addr   0xD0	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
//#define	MPU6050_Addr   0xD0	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
//************************************************************************************************/
unsigned char TX_DATA[4];  	 //显示据缓存区
int16_t BUF[10];       //接收数据缓存区
char  test=0; 				 //IIC用到
int16_t T_X,T_Y,T_Z,T_T;		 //X,Y,Z轴，温度
int16_t RwAcc[3],Gyro[3];

int16_t ax,ay,az,gx,gy,gz;
#define	uchar  u8
//#define unsigned char uchar
uchar ge,shi,bai,qian,wan,shiwan;           //显示变量

//************************************++++++++++++++++++++++++++++++++
/*模拟IIC端口输出输入定义*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7


/* 函数申明 -----------------------------------------------*/
// void RCC_Configuration(void);
// void GPIO_Configuration(void);
// void NVIC_Configuration(void);
// void USART1_Configuration(void);
// void USART2_Configuration(void);
// void USART3_Configuration(void);
// void WWDG_Configuration(void);
// void Delay(u32 nTime);
   void Delayms(vu32 m);  
// void conversion(long temp_data);

void Init_MPU6050(void);
void Send_data(uchar axis);
void READ_MPU6050(void);
void READ_MPU6050_Accel(void);
void DATA_printf(uchar *s,short temp_data);   //新加


/* 变量定义 ----------------------------------------------*/


//***************************************************
void conversion(long temp_data)  
{     
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //取余运算 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //取余运算
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //取余运算
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //取余运算
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //取余运算
    ge=temp_data+0x30; 	
}

/*******************************************************************************
* Function Name  : I2C_GPIO_Config
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);
  //I/O管脚的复用功能
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  //表示配置相应管脚为GPIO开漏输出(真双向IO) 
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*开漏输出_OUT_OD ——IO输出0接GND，IO输出1，悬空，需要外接上拉电阻，才能实现输出高电平。当输出为1时，IO口的状态由上拉电阻拉高电平，
  但由于是开漏输出模式，这样IO口也就可以由外部电路改变为低电平或不变。可以读IO输入电平变化，实现C51的IO双向功能
	*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{
		
   u8 i=30; //这里可以优化速度	，经测试最低到5还能写入
   while(i) 
   { 
     i--; 
   }  
}

void delay5ms(void)
{
		
   u16 i=5000;  
   while(i) 
   { 
     i--; 
   }  
}

/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
bool I2C_Start(void)
{
	SDA_H;
	SCL_H;//    
	I2C_delay();
	if(!SDA_read)return FALSE;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	//SDA线为高电平则总线出错,退出
	SDA_L;
	I2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
bool I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
	  I2C_delay();
      return FALSE;
	}
	SCL_L;
	I2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(u8 SendByte) //数据从高位到低位//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
unsigned char I2C_RadeByte(void)  //数据从高位到低位//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}

//ZRX          
//单字节写入*******************************************

bool Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	if(!I2C_Start())return FALSE;
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
    I2C_SendByte(REG_Address );   //设置低起始地址      
    I2C_WaitAck();	
    I2C_SendByte(REG_data);
    I2C_WaitAck();   
    I2C_Stop(); 
    delay5ms();
    return TRUE;
}

//单字节读取*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
	if(!I2C_Start())return FALSE;
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop();test=1; return FALSE;}
    I2C_SendByte((u8) REG_Address);   //设置低起始地址      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress+1);     //读数+1
    I2C_WaitAck();

	REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return TRUE;
	return REG_data;

}					
/*
********************************************************************************
** 函数名称 ： Delay(vu32 nCount)
** 函数功能 ： 延时函数
** 输    入	： 无
** 输    出	： 无
** 返    回	： 无
********************************************************************************

 void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}
*/

/*
********************************************************************************
** 函数名称 ： void Delayms(vu32 m)
** 函数功能 ： 长延时函数	 m=1,延时1ms
** 输    入	： 无
** 输    出	： 无
** 返    回	： 无
********************************************************************************
*/
 void Delayms(vu32 m)
{
  u32 i;
  
  for(; m != 0; m--)	
       for (i=50000; i>0; i--);
}

//初始化MPU6050，根据需要请参考pdf进行修改************************
void Init_MPU6050(void)
{

   	Single_Write(MPU6050_Addr,PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_Write(MPU6050_Addr,SMPLRT_DIV, 0x07);    //陀螺仪采样率
	Single_Write(MPU6050_Addr,CONFIG, 0x06);        //5Hz 
	
//	Single_Write(MPU6050_Addr,INT_PIN_CFG, 0x42);   //使能旁路I2C
//	Single_Write(MPU6050_Addr,USER_CTRL, 0x40);     //使能旁路I2C
	
	Single_Write(MPU6050_Addr,GYRO_CONFIG, 0x18);   
	Single_Write(MPU6050_Addr,ACCEL_CONFIG, 0x01);
}
	
//******读取MPU6050数据****************************************

int16_t Gyro_C(int16_t tt)
{
	tt=(int32_t)((int32_t)tt*10)/164;
  return tt;
}

void READ_MPU6050(void)
{
   BUF[0]=Single_Read(MPU6050_Addr,GYRO_XOUT_L); 
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_XOUT_H);
   T_X=	(BUF[1]<<8)|BUF[0];
//   T_X/=16.4; 						   //读取计算X轴数据
   Gyro[0]=Gyro_C(T_X);
	 
   BUF[2]=Single_Read(MPU6050_Addr,GYRO_YOUT_L);
   BUF[3]=Single_Read(MPU6050_Addr,GYRO_YOUT_H);
   T_Y=	(BUF[3]<<8)|BUF[2];
   Gyro[1]=Gyro_C(T_Y);
	//T_Y/=16.4; 						   //读取计算Y轴数据
   BUF[4]=Single_Read(MPU6050_Addr,GYRO_ZOUT_L);
   BUF[5]=Single_Read(MPU6050_Addr,GYRO_ZOUT_H);
   T_Z=	(BUF[5]<<8)|BUF[4];
   Gyro[2]=Gyro_C(T_Z);
	//T_Z/=16.4; 					       //读取计算Z轴数据

	
  // BUF[6]=Single_Read(MPU6050_Addr,TEMP_OUT_L); 
  // BUF[7]=Single_Read(MPU6050_Addr,TEMP_OUT_H); 
  // T_T=(BUF[7]<<8)|BUF[6];
  // T_T = 35+ ((double) (T_T + 13200)) / 280;// 读取计算出温度
}


void Read_6050_Gyro(void)
{
   BUF[0]=Single_Read(MPU6050_Addr,GYRO_XOUT_L); 
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_XOUT_H);
   gx=	(BUF[1]<<8)|BUF[0];       //读取计算X轴数据
	 
   BUF[2]=Single_Read(MPU6050_Addr,GYRO_YOUT_L);
   BUF[3]=Single_Read(MPU6050_Addr,GYRO_YOUT_H);
   gy=	(BUF[3]<<8)|BUF[2];       //读取计算Y轴数据
	   
   BUF[4]=Single_Read(MPU6050_Addr,GYRO_ZOUT_L);
   BUF[5]=Single_Read(MPU6050_Addr,GYRO_ZOUT_H);
   gz=	(BUF[5]<<8)|BUF[4];     //读取计算Z轴数据     
				       
}


void Read_6050_Accel(void)
{
	BUF[0]=Single_Read(MPU6050_Addr,ACCEL_XOUT_L); 
	BUF[1]=Single_Read(MPU6050_Addr,ACCEL_XOUT_H);
	ax=	(BUF[1]<<8)|BUF[0];    //读取计算X轴数据              
	
	BUF[2]=Single_Read(MPU6050_Addr,ACCEL_YOUT_L);
	BUF[3]=Single_Read(MPU6050_Addr,ACCEL_YOUT_H);
	ay=	(BUF[3]<<8)|BUF[2];             //读取计算Y轴数据
	
	BUF[4]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_L);
	BUF[5]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_H);
	az=	(BUF[5]<<8)|BUF[4];                    //读取计算Z轴数据

}

int16_t Acce_C(int16_t tt)
{
	tt=(int32_t)((int32_t)tt*100)/16384;
	return tt;
}



void READ_MPU6050_Accel(void)
{
	BUF[0]=Single_Read(MPU6050_Addr,ACCEL_XOUT_L); 
	BUF[1]=Single_Read(MPU6050_Addr,ACCEL_XOUT_H);
	T_X=	(BUF[1]<<8)|BUF[0];
//	T_X=(float)((float)T_X/16384)*100; 		//扩大100倍	       //读取计算X轴数据
  RwAcc[0]=Acce_C(T_X);
//	T_X=(T_X/256)*100/64;
	
	BUF[2]=Single_Read(MPU6050_Addr,ACCEL_YOUT_L);
	BUF[3]=Single_Read(MPU6050_Addr,ACCEL_YOUT_H);
	T_Y=	(BUF[3]<<8)|BUF[2];
	//T_Y=(float)((float)T_Y/16384)*100; 						   //读取计算Y轴数据
   RwAcc[1]=Acce_C(T_Y);
//	T_Y=(T_Y/256)*100/64;
	
	BUF[4]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_L);
	BUF[5]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_H);
	T_Z=	(BUF[5]<<8)|BUF[4];
	//T_Z=(float)((float)T_Z/16384)*100; 					       //读取计算Z轴数据
   RwAcc[2]=Acce_C(T_Z);
//	T_Z=(T_Z/256)*100/64;
	
/*
BUF[6]=Single_Read(MPU6050_Addr,TEMP_OUT_L); 
	BUF[7]=Single_Read(MPU6050_Addr,TEMP_OUT_H); 
	T_T=(BUF[7]<<8)|BUF[6];
	T_T = 35+ ((double) (T_T + 13200)) / 280;// 读取计算出温度
//	T_T = (((double) T_T )/340+36.53)*10 ;//+36.53;  // 读取计算出温度
*/
}

unsigned char who_am(void)
{
	unsigned char readd;
	readd=Single_Read(MPU6050_Addr,0x75);
	
	return  readd;
}

