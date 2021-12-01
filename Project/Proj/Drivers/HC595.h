#pragma once

#include "Type.h"
#include "Gpio.h"

typedef struct
{
	/// <summary>时钟之间的延时</summary>
	byte Delayus;

	/// <summary>时钟 SHCP</summary>
	PinName Clk;
	/// <summary>数据 IN</summary>
	PinName Data;
	/// <summary>输出数据 STCP</summary>
	PinName PutOut;
	/// <summary>使能输出 EN</summary>
	PinName Enable;
	/// <summary>清空引脚 MR</summary>
	PinName Reset;
}HC595_t;

/// <summary>初始化gpio</summary>
/// <param name="port"></param>
void HC595Init(HC595_t* port);

/// <summary>写字节</summary>
/// <param name="port"></param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <param name="bitBigEndian">大端bit</param>
/// <param name="out"></param>
void HC595Write(HC595_t* port, byte* data, int len, bool bitBigEndian, bool out);

/// <summary>使能引脚控制</summary>
/// <param name="port"></param>
/// <param name="value"></param>
void HC595Enable(HC595_t* port, bool value);

/// <summary>使用MR引脚控制</summary>
/// <param name="port"></param>
void H595Clear(HC595_t* port);

