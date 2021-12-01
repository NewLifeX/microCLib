
#include "HC595.h"

static void DelayUs(uint us)
{
	for (int i = 0; i < us; i++)
	{
		for (volatile int j = 0; j < 10; j++)
		{
		}
	}
}

/// <summary>初始化gpio</summary>
/// <param name="port"></param>
void HC595Init(HC595_t* port)
{
	// 使能引脚拉高，不使能
	GpioInit(port->Enable, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 1);
	GpioInit(port->Reset , PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 1);
	H595Clear(port);

	GpioInit(port->Clk, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
	GpioInit(port->Data, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
	GpioInit(port->PutOut, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);

	HC595Enable(port, false);
}

/// <summary>写字节</summary>
/// <param name="port"></param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <param name="bitBigEndian">大端bit</param>
/// <param name="out"></param>
void HC595Write(HC595_t* port, byte* data, int len, bool bitBigEndian, bool out)
{
	if (port == NULL)return;
	if (data == NULL)return;
	if (len < 1)return;

	// 595 上升沿锁定数据。
	for (int i = 0; i < len; i++)
	{
		byte dat = data[i];
		for (int  j = 0; j < 8; j++)
		{
			// 给低电平，确保上升沿的存在。
			GpioWrite(port->Clk, false);

			byte mask = 0x01 << j;
			if (bitBigEndian)mask = 0x80 >> j;

			GpioWrite(port->Data, (dat & mask) != 0);
			// 延时之后给上升沿，确保数据稳定。
			DelayUs(port->Delayus);
			GpioWrite(port->Clk, true);
		}
	}

	if (out)
	{
		// 上升沿输出
		GpioWrite(port->PutOut, true);
		DelayUs(port->Delayus);
		GpioWrite(port->PutOut, false);
	}
}

/// <summary>使能引脚控制</summary>
/// <param name="port"></param>
/// <param name="value"></param>
void HC595Enable(HC595_t* port, bool value)
{
	// Enable 低电平有效
	GpioWrite(port->Enable, !value);
}

/// <summary>使用MR引脚控制</summary>
/// <param name="port"></param>
void H595Clear(HC595_t* port)
{
	GpioWrite(port->Reset, false);
	DelayUs(port->Delayus);
	GpioWrite(port->Reset, true);
}
