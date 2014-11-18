#include "mpu6050.h"
//#include "led.h"
#include "systick.h"


u16 i2c_rx_buf[14]; 

int16_t accel_gyro[7];


static void i2c_configuration( void )
{
	GPIO_InitTypeDef	gpio_init_structure;
	I2C_InitTypeDef		i2c_init_structure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE);

	/* PB6-I2C1_SCL、PB7-I2C1_SDA*/
	gpio_init_structure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_OD;  // 开漏输出;
	GPIO_Init( GPIOB, &gpio_init_structure);

	/*********** I2C periphral configuration **********/
	I2C_DeInit(I2C1);
	i2c_init_structure.I2C_Mode = I2C_Mode_I2C;
	i2c_init_structure.I2C_DutyCycle = I2C_DutyCycle_2;//快速模式时的占空比;
	i2c_init_structure.I2C_OwnAddress1 = I2C1_MPU6050;
	i2c_init_structure.I2C_Ack = I2C_Ack_Enable;
	i2c_init_structure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c_init_structure.I2C_ClockSpeed = I2C_SPEED;

	I2C_Cmd(I2C1, ENABLE);
	I2C_Init( I2C1, &i2c_init_structure);
	I2C_AcknowledgeConfig( I2C1, ENABLE);

	/************** I2C NVIC configuration *************************/  


}


static void dma_i2c1_configuration(void)
{
	NVIC_InitTypeDef	nvic_init_structure;
	DMA_InitTypeDef		dma_init_structure;

	DMA_DeInit(DMA1_Channel7);

	dma_init_structure.DMA_MemoryBaseAddr = (u32)I2C1_DR_ADDRESS;
	dma_init_structure.DMA_MemoryBaseAddr = (u32)i2c_rx_buf;
	dma_init_structure.DMA_M2M = DMA_M2M_Disable;
	dma_init_structure.DMA_Mode = DMA_Mode_Normal;
	dma_init_structure.DMA_Priority = DMA_Priority_VeryHigh;
	dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据来源于外设；
	dma_init_structure.DMA_BufferSize = 14;
	dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	
	DMA_Init(DMA1_Channel7, &dma_init_structure);
	////传输完成中断 开启DMA中断;
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

	nvic_init_structure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_structure.NVIC_IRQChannelSubPriority = 0;
	nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_structure);


}

void i2c_dma_read( u8 read_addr)
{
//	read_addr +=1;
	//disable dma channel
	DMA_Cmd( MPU6050_DMA_CHANNEL, DISABLE);
	//? set current data number again to 14
	DMA_SetCurrDataCounter(MPU6050_DMA_CHANNEL, 14);

	//while the bus is busy
	while( I2C_GetFlagStatus( MPU6050_I2C,I2C_FLAG_BUSY));
	//enable dma nack automatic generation.....
	I2C_DMALastTransferCmd( MPU6050_I2C, ENABLE);

	//send start condition 
	I2C_GenerateSTART( MPU6050_I2C, ENABLE);

	//test on ev5 and clear it 
	while( !I2C_CheckEvent( MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	//send mpu6050 address for write
	I2C_Send7bitAddress( MPU6050_I2C, I2C1_MPU6050, I2C_Direction_Transmitter );
	
	//test on ev6 and clear it 
	while( !I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//clear ev6 by setting again the PE bit
	I2C_Cmd( MPU6050_I2C,ENABLE);

	//send the mpu6050 internal address to write to
	I2C_SendData( MPU6050_I2C, read_addr);

	//test on ev8 and clear it 
	while(!I2C_CheckEvent( MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//send start condition a second time
	I2C_GenerateSTART(MPU6050_I2C, ENABLE);

	//test on ev5 and clear it 
	while( !I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	
	//send mpu6050 address for read
	I2C_Send7bitAddress(MPU6050_I2C,I2C1_MPU6050, I2C_Direction_Receiver);
	
	//test on ev6 and clear it 
	while( !I2C_CheckEvent( MPU6050_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	//start dma to receive data from i2c
	DMA_Cmd(MPU6050_DMA_CHANNEL, ENABLE);

	I2C_DMACmd(MPU6050_I2C, ENABLE);
	//when the data transmission is complete, it will automatically jump to dma interrupt routine
	//to finish the rest.
}

void mpu6050_i2c_byte_write(u8 write_data, u8 reg_addr)
{
	while(I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_BUSY));
	//send start condition
	I2C_GenerateSTART( MPU6050_I2C, ENABLE);
	//test on ev5 and clear it 
	while(!I2C_CheckEvent( MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	//send address for write 
	I2C_Send7bitAddress( MPU6050_I2C,I2C1_MPU6050, I2C_Direction_Transmitter);
	//test on ev6 and clear it 
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//send internal address to write to 
	I2C_SendData( MPU6050_I2C, reg_addr);
	//test on ev8
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//send the byte to be written
	I2C_SendData(MPU6050_I2C,write_data);
	//test on ev8
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	//send stop condition
	I2C_GenerateSTOP( MPU6050_I2C, ENABLE);
	//i2c_wait_stand();
}

//////////////////////////////////////////
uint8_t I2C_ReadByte(uint8_t REG_Address)
{
	uint8_t data_byte;
  //*((u8 *)0x4001080c) |=0x80; 
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008
    
    
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, I2C1_MPU6050, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, REG_Address);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2C1, I2C1_MPU6050, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  /* While there is data to be read */
       /* Disable Acknowledgement ;*/
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))       
      /* Read a byte from the EEPROM */
      data_byte = I2C_ReceiveData(I2C1);


  /* Enable Acknowledgement to be ready for another reception; */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
	//I2C_EE_WaitEepromStandbyState();
	return data_byte;
}


 void mpu6050_init(void)
{
	i2c_configuration();
	dma_i2c1_configuration();
	mpu6050_i2c_byte_write(1<<7, PWR_MGMT_1);
//	Delay(0x0f0000);
	delay_us(1000);

	mpu6050_i2c_byte_write(0x03, PWR_MGMT_1);
	//Delay(0x0f00);
	delay_us(10);

	mpu6050_i2c_byte_write(0x01, SMPLRT_DIV);
	//Delay(0x0f00);
	delay_us(10);

	mpu6050_i2c_byte_write(0x01, CONFIG);
	//Delay(0x0f00);
	delay_us(10);

	mpu6050_i2c_byte_write(0x03, GYRO_CONFIG);
	//Delay(0x0f00);
	delay_us(10);

	mpu6050_i2c_byte_write(0x03, ACCEL_CONFIG);
	//Delay(0xf0000);
	delay_us(1000);

}

void i2c_wait_standby(void)
{
	vu16 sr1_temp = 0;

	do 
	{
		I2C_GenerateSTART(I2C1, ENABLE);

		sr1_temp = I2C_ReadRegister( I2C1, I2C_Register_SR1);

		I2C_Send7bitAddress(I2C1, I2C1_MPU6050, I2C_Direction_Transmitter);
	} while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1)&0x0002));

	I2C_ClearFlag(I2C1, I2C_FLAG_AF);

	I2C_GenerateSTOP(I2C1, ENABLE);


}



