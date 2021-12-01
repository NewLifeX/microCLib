#pragma once

#include "Type.h"
#include "Gpio.h"

typedef struct
{
	byte DelayUs;

	PinName Cp;
	PinName Pl;
	PinName Ce;
	PinName Data;
}HC165_t;

/// <summary>初始化gpio</summary>
/// <param name="port"></param>
void HC165Init(HC165_t* port);

/// <summary>采样数据到165芯片内部</summary>
/// <param name="port"></param>
void HC165Take(HC165_t* port);

/// <summary>读取数据</summary>
/// <param name="port"></param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <param name="bitBigEndian">大端bit</param>
void HC165Read(HC165_t* port, byte* data, int len, bool bitBigEndian);

