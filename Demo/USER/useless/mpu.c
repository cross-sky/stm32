#include "mpu.h"
//#include "led.h"
#include  <math.h> 
#include "stm32f10x.h"

// ����MPU6050�ڲ���ַ
//****************************************
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)

#define INT_PIN_CFG     0x37    //������·��Ч ��ֵ��0x42 AUX_DA�ĸ���I2C
#define USER_CTRL       0x6A    //�û����üĴ��� ��ֵ��0x40  AUX_DA�ĸ���I2C

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

#define	PWR_MGMT_1		0x6B	//��Դ����������ֵ��0x00(��������)
//#define	WHO_AM_I		0x75	//IIC��ַ�Ĵ���(Ĭ�϶�����ֵ0x68��ֻ��)

//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ
#define	MPU6050_Addr   0xD0	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
//#define	MPU6050_Addr   0xD0	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
//************************************************************************************************/
unsigned char TX_DATA[4];  	 //��ʾ�ݻ�����
int16_t BUF[10];       //�������ݻ�����
char  test=0; 				 //IIC�õ�
int16_t T_X,T_Y,T_Z,T_T;		 //X,Y,Z�ᣬ�¶�
int16_t RwAcc[3],Gyro[3];

int16_t ax,ay,az,gx,gy,gz;
#define	uchar  u8
//#define unsigned char uchar
uchar ge,shi,bai,qian,wan,shiwan;           //��ʾ����

//************************************++++++++++++++++++++++++++++++++
/*ģ��IIC�˿�������붨��*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7


/* �������� -----------------------------------------------*/
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
void DATA_printf(uchar *s,short temp_data);   //�¼�


/* �������� ----------------------------------------------*/


//***************************************************
void conversion(long temp_data)  
{     
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //ȡ������ 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //ȡ������
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //ȡ������
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //ȡ������
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //ȡ������
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
  //I/O�ܽŵĸ��ù���
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  //��ʾ������Ӧ�ܽ�ΪGPIO��©���(��˫��IO) 
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*��©���_OUT_OD ����IO���0��GND��IO���1�����գ���Ҫ����������裬����ʵ������ߵ�ƽ�������Ϊ1ʱ��IO�ڵ�״̬�������������ߵ�ƽ��
  �������ǿ�©���ģʽ������IO��Ҳ�Ϳ������ⲿ��·�ı�Ϊ�͵�ƽ�򲻱䡣���Զ�IO�����ƽ�仯��ʵ��C51��IO˫����
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
		
   u8 i=30; //��������Ż��ٶ�	����������͵�5����д��
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
	if(!SDA_read)return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
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
bool I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
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
void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
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
unsigned char I2C_RadeByte(void)  //���ݴӸ�λ����λ//
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
//���ֽ�д��*******************************************

bool Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	if(!I2C_Start())return FALSE;
    I2C_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
    I2C_SendByte(REG_Address );   //���õ���ʼ��ַ      
    I2C_WaitAck();	
    I2C_SendByte(REG_data);
    I2C_WaitAck();   
    I2C_Stop(); 
    delay5ms();
    return TRUE;
}

//���ֽڶ�ȡ*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
	if(!I2C_Start())return FALSE;
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop();test=1; return FALSE;}
    I2C_SendByte((u8) REG_Address);   //���õ���ʼ��ַ      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress+1);     //����+1
    I2C_WaitAck();

	REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return TRUE;
	return REG_data;

}					
/*
********************************************************************************
** �������� �� Delay(vu32 nCount)
** �������� �� ��ʱ����
** ��    ��	�� ��
** ��    ��	�� ��
** ��    ��	�� ��
********************************************************************************

 void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}
*/

/*
********************************************************************************
** �������� �� void Delayms(vu32 m)
** �������� �� ����ʱ����	 m=1,��ʱ1ms
** ��    ��	�� ��
** ��    ��	�� ��
** ��    ��	�� ��
********************************************************************************
*/
 void Delayms(vu32 m)
{
  u32 i;
  
  for(; m != 0; m--)	
       for (i=50000; i>0; i--);
}

//��ʼ��MPU6050��������Ҫ��ο�pdf�����޸�************************
void Init_MPU6050(void)
{

   	Single_Write(MPU6050_Addr,PWR_MGMT_1, 0x00);	//�������״̬
	Single_Write(MPU6050_Addr,SMPLRT_DIV, 0x07);    //�����ǲ�����
	Single_Write(MPU6050_Addr,CONFIG, 0x06);        //5Hz 
	
//	Single_Write(MPU6050_Addr,INT_PIN_CFG, 0x42);   //ʹ����·I2C
//	Single_Write(MPU6050_Addr,USER_CTRL, 0x40);     //ʹ����·I2C
	
	Single_Write(MPU6050_Addr,GYRO_CONFIG, 0x18);   
	Single_Write(MPU6050_Addr,ACCEL_CONFIG, 0x01);
}
	
//******��ȡMPU6050����****************************************

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
//   T_X/=16.4; 						   //��ȡ����X������
   Gyro[0]=Gyro_C(T_X);
	 
   BUF[2]=Single_Read(MPU6050_Addr,GYRO_YOUT_L);
   BUF[3]=Single_Read(MPU6050_Addr,GYRO_YOUT_H);
   T_Y=	(BUF[3]<<8)|BUF[2];
   Gyro[1]=Gyro_C(T_Y);
	//T_Y/=16.4; 						   //��ȡ����Y������
   BUF[4]=Single_Read(MPU6050_Addr,GYRO_ZOUT_L);
   BUF[5]=Single_Read(MPU6050_Addr,GYRO_ZOUT_H);
   T_Z=	(BUF[5]<<8)|BUF[4];
   Gyro[2]=Gyro_C(T_Z);
	//T_Z/=16.4; 					       //��ȡ����Z������

	
  // BUF[6]=Single_Read(MPU6050_Addr,TEMP_OUT_L); 
  // BUF[7]=Single_Read(MPU6050_Addr,TEMP_OUT_H); 
  // T_T=(BUF[7]<<8)|BUF[6];
  // T_T = 35+ ((double) (T_T + 13200)) / 280;// ��ȡ������¶�
}


void Read_6050_Gyro(void)
{
   BUF[0]=Single_Read(MPU6050_Addr,GYRO_XOUT_L); 
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_XOUT_H);
   gx=	(BUF[1]<<8)|BUF[0];       //��ȡ����X������
	 
   BUF[2]=Single_Read(MPU6050_Addr,GYRO_YOUT_L);
   BUF[3]=Single_Read(MPU6050_Addr,GYRO_YOUT_H);
   gy=	(BUF[3]<<8)|BUF[2];       //��ȡ����Y������
	   
   BUF[4]=Single_Read(MPU6050_Addr,GYRO_ZOUT_L);
   BUF[5]=Single_Read(MPU6050_Addr,GYRO_ZOUT_H);
   gz=	(BUF[5]<<8)|BUF[4];     //��ȡ����Z������     
				       
}


void Read_6050_Accel(void)
{
	BUF[0]=Single_Read(MPU6050_Addr,ACCEL_XOUT_L); 
	BUF[1]=Single_Read(MPU6050_Addr,ACCEL_XOUT_H);
	ax=	(BUF[1]<<8)|BUF[0];    //��ȡ����X������              
	
	BUF[2]=Single_Read(MPU6050_Addr,ACCEL_YOUT_L);
	BUF[3]=Single_Read(MPU6050_Addr,ACCEL_YOUT_H);
	ay=	(BUF[3]<<8)|BUF[2];             //��ȡ����Y������
	
	BUF[4]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_L);
	BUF[5]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_H);
	az=	(BUF[5]<<8)|BUF[4];                    //��ȡ����Z������

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
//	T_X=(float)((float)T_X/16384)*100; 		//����100��	       //��ȡ����X������
  RwAcc[0]=Acce_C(T_X);
//	T_X=(T_X/256)*100/64;
	
	BUF[2]=Single_Read(MPU6050_Addr,ACCEL_YOUT_L);
	BUF[3]=Single_Read(MPU6050_Addr,ACCEL_YOUT_H);
	T_Y=	(BUF[3]<<8)|BUF[2];
	//T_Y=(float)((float)T_Y/16384)*100; 						   //��ȡ����Y������
   RwAcc[1]=Acce_C(T_Y);
//	T_Y=(T_Y/256)*100/64;
	
	BUF[4]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_L);
	BUF[5]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_H);
	T_Z=	(BUF[5]<<8)|BUF[4];
	//T_Z=(float)((float)T_Z/16384)*100; 					       //��ȡ����Z������
   RwAcc[2]=Acce_C(T_Z);
//	T_Z=(T_Z/256)*100/64;
	
/*
BUF[6]=Single_Read(MPU6050_Addr,TEMP_OUT_L); 
	BUF[7]=Single_Read(MPU6050_Addr,TEMP_OUT_H); 
	T_T=(BUF[7]<<8)|BUF[6];
	T_T = 35+ ((double) (T_T + 13200)) / 280;// ��ȡ������¶�
//	T_T = (((double) T_T )/340+36.53)*10 ;//+36.53;  // ��ȡ������¶�
*/
}

unsigned char who_am(void)
{
	unsigned char readd;
	readd=Single_Read(MPU6050_Addr,0x75);
	
	return  readd;
}
