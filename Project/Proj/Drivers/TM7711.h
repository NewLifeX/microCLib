#pragma once

#include "Type.h"
#include "Gpio.h"
// #include "SysTime.h"

// 只使用10HZ 差分值。 
// 其他两种数据不要。
// 也就是说，只使用 25 脉冲的类型。

typedef struct
{
	PinName SCK;
	PinName DOUT;

	// push数据用的下标。
	ushort PushIdx;
	// 经验值，别动
	uint SrcData[12];

	// 上次读取的值。用于消抖。
	// 每次读取的时候都会对比一下。
	uint LastValue;

	// 上次读取时间
	Time_t LastStamp;

	// 错误计数。
	byte ErrCnt;
}TM7711_t;

/// <summary>初始化</summary>
/// <param name="thiss"></param>
/// <returns></returns>
bool TM7711Init(TM7711_t* thiss);

/// <summary>丢数据到滤波器内</summary>
/// <param name="thiss"></param>
/// <param name="data"></param>
void TM7711PushData(TM7711_t* thiss, uint data);

/// <summary>获取数据,滤波器之后的数据</summary>
/// <param name="thiss">对象自己</param>
/// <returns></returns>
uint TM7711GetDstData(TM7711_t* thiss);

/// <summary>25bit 读数据。设置10HZ差分。无噪声位17bit</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData25(TM7711_t* thiss);

/// <summary>26bit 读数据。设置40HZ温度</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData26(TM7711_t* thiss);

/// <summary>27bit 读数据。设置40HZ差分</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData27(TM7711_t* thiss);
