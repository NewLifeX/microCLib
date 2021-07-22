
#include <stdio.h>
// #include "stm32f10x.h"
// #include "stm32f10x_conf.h"
#include "I2C_Soft.h"

// I2C_Pin_t I2C_Pin;

/*
SDA 和SCL 都是双向线路都通过一个电流源或上拉电阻连接到正的电源电压。
当总线空闲时这两条线路都是高电平。

SDA 线上的数据必须在时钟的高电平周期保持稳定。
数据线的高或低电平状态只有在SCL线的时钟信号是低电平时才能改变。
起始和停止例外，因此从机很容易区分起始和停止信号。
*/

static void DelayUs(ushort us)
{
	for (int i = 0; i < us * 17; i++);
}

void Init_I2C_Pin_t(I2C_Info_t* info)
{
	info->SCK = P0;		// Pin
	info->SDA = P0;		// Pin
	info->TimeDelayUs = 0;
}

static uint8_t ReadPin(uint8_t pin)
{
	assert_param(pin != P0);
	// return GPIO_ReadInputDataBit(IoGroup[pin>>4],GPIO_Pin_0 << (pin & 0x0f));
	return GpioRead(pin);
}

static void WritePin(uint8_t pin,uint8_t value)
{
	assert_param(pin != P0);
	// GPIO_WriteBit(IoGroup[pin >> 4], GPIO_Pin_0 << (pin & 0x0f), value ? Bit_SET : Bit_RESET);
	GpioWrite(pin, value);
}

int Init_I2C(I2C_Info_t* info)
{
	assert_param(info->SCK != P0);
	assert_param(info->SDA != P0);
	
	// GPIO_InitTypeDef GPIO_InitStructure;
	// GPIO_StructInit(&GPIO_InitStructure);
	// 
	// // 打开引脚时钟
	// RCC_APB2PeriphClockCmd(RccIoGroupParam[info->SCK>>4], ENABLE);
	// RCC_APB2PeriphClockCmd(RccIoGroupParam[info->SDA>>4], ENABLE);
	// 
	// GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	// GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_OD;
	// 
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 << (info->SCK & 0x0f);
	// GPIO_Init(IoGroup[info->SCK >> 4], &GPIO_InitStructure);
	// 
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 << (info->SDA & 0x0f);
	// GPIO_Init(IoGroup[info->SDA >> 4], &GPIO_InitStructure);
	
	GpioInit(info->SCK, PIN_OUTPUT, PIN_OUTPUT_OD, PIN_PULL_UP, 0, 1);
	GpioInit(info->SDA, PIN_OUTPUT, PIN_OUTPUT_OD, PIN_PULL_UP, 0, 1);

	WritePin(info->SCK,1);
	
	WritePin(info->SDA,1);
	return 1;
}

// SCL高电平期间  SDA产生下降沿
void I2C_Start(I2C_Info_t* info)
{
	WritePin(info->SCK,1);
	
	WritePin(info->SDA,1);
	DelayUs(info->TimeDelayUs);
	WritePin(info->SDA,0);
}

// SCL 高电平期间  SDA产生上升沿
void I2C_Stop(I2C_Info_t* info)
{
	WritePin(info->SDA,0);
	WritePin(info->SCK,1);
	
	DelayUs(info->TimeDelayUs);
	WritePin(info->SDA,1);
}

// SCL 高电平 等待SDA下降沿
int I2C_WaiteAck(I2C_Info_t* info)
{
	WritePin(info->SDA,1);
	WritePin(info->SCK,1);
	DelayUs(info->TimeDelayUs);
	
	int timeout = 10;
	while(ReadPin(info->SDA) && --timeout)
	{
		DelayUs(info->TimeDelayUs);
	}
	if(timeout == 0)return -1;
	
	DelayUs(info->TimeDelayUs);
	WritePin(info->SCK,0);
	DelayUs(info->TimeDelayUs);	
	return 1;
}

// 发送Ack
/*
在 I2C 总线传输数据过程中，每传输一个字节，都要跟一个应答状态位。接收器接收数
据的情况可以通过应答位来告知发送器。应答位的时钟脉冲仍由主机产生，而应答位的数据
状态则遵循“谁接收谁产生”的原则，即总是由接收器产生应答位。主机向从机发送数据时，
应答位由从机产生；主机从从机接收数据时，应答位由主机产生。I2C 总线标准规定：应答
位为0 表示接收器应答（ACK），常常简记为A；为1 则表示非应答（NACK），常常简记为
A。发送器发送完LSB 之后，应当释放SDA 线（拉高SDA，输出晶体管截止），以等待接
收器产生应答位。
*/

// ack为应答类型  1为正常  0 为失败
void I2C_SendAck(I2C_Info_t* info, uint8_t ack)
{
	WritePin(info->SCK,0);
	
	WritePin(info->SDA,!ack);
	DelayUs(info->TimeDelayUs);	
	
	WritePin(info->SCK,1);
	DelayUs(info->TimeDelayUs);	
	WritePin(info->SCK,0);
	
	WritePin(info->SDA,1);
	DelayUs(info->TimeDelayUs);	
}

void I2C_WriteByte(I2C_Info_t* info, uint8_t dat)
{
	// SDA 线上的数据必须在时钟的高电平周期保持稳定
	WritePin(info->SCK,0);
	for(uint8_t mask=0x80; mask>0; mask>>=1)
    {
		WritePin(info->SDA, dat & mask);
		DelayUs(info->TimeDelayUs);	

		// 置时钟线为高，通知被控器开始接收数据位
		WritePin(info->SCK,1);
		
		DelayUs(info->TimeDelayUs);	
		WritePin(info->SCK,0);
		
		DelayUs(info->TimeDelayUs);	
    }
}

uint8_t I2C_ReadByte(I2C_Info_t* info)
{
	// SDA 线上的数据必须在时钟的高电平周期保持稳定
	WritePin(info->SDA,1);
	uint8_t rs = 0;
	for(uint8_t mask=0x80; mask>0; mask>>=1)
	{
		WritePin(info->SCK,1);		// 置时钟线为高使数据线上数据有效
		//Delay(2);
		// 等SCL变高
		uint32_t timeout = 10;
		
		while(!ReadPin(info->SCK) && --timeout)
		{
			DelayUs(info->TimeDelayUs);
		}
		if(timeout == 0)while(1);
		
		if(ReadPin(info->SDA)) rs |= mask;	// 读数据位
		WritePin(info->SCK,0);				// 置时钟线为低，准备接收数据位
		
		DelayUs(info->TimeDelayUs);;
	}

	return rs;
}

