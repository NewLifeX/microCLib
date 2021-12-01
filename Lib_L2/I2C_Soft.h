
#ifndef __IIC_H__
#define __IIC_H__

#include <stdio.h>
// #include "stm32f10x.h"
// #include "stm32f10x_conf.h"
#include "Pin.h"
#include "Gpio.h"
#include "Type.h"
// #include "Delay.h"

// 软件IIC   使用普通IO口实现IIC时序

typedef struct
{
	uint8_t SCK;				// Pin
	uint8_t SDA;				// Pin
	uint16_t TimeDelayUs;		// 数据建立时间，看从站的速度。够快的话可以是0
}I2C_Info_t;

// extern I2C_Info_t I2C_Info;
// 初始化结构体
void Init_I2C_Pin_t(I2C_Info_t* info);
// 初始化IIC  （实质是打开引脚）
int Init_I2C(I2C_Info_t* info);

void I2C_Start(I2C_Info_t* info);
void I2C_Stop(I2C_Info_t* info);

// 失败返回-1
int I2C_WaiteAck(I2C_Info_t* info);
// ack为应答类型  1为正常  0 为失败
void I2C_SendAck(I2C_Info_t* info, uint8_t ack);

void I2C_WriteByte(I2C_Info_t* info, uint8_t dat);
uint8_t I2C_ReadByte(I2C_Info_t* info);

#endif
