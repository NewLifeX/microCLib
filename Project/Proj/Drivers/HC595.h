#pragma once

#include "Type.h"
#include "Gpio.h"

typedef struct
{
	/// <summary>ʱ��֮�����ʱ</summary>
	byte Delayus;

	/// <summary>ʱ�� SHCP</summary>
	PinName Clk;
	/// <summary>���� IN</summary>
	PinName Data;
	/// <summary>������� STCP</summary>
	PinName PutOut;
	/// <summary>ʹ����� EN</summary>
	PinName Enable;
	/// <summary>������� MR</summary>
	PinName Reset;
}HC595_t;

/// <summary>��ʼ��gpio</summary>
/// <param name="port"></param>
void HC595Init(HC595_t* port);

/// <summary>д�ֽ�</summary>
/// <param name="port"></param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <param name="bitBigEndian">���bit</param>
/// <param name="out"></param>
void HC595Write(HC595_t* port, byte* data, int len, bool bitBigEndian, bool out);

/// <summary>ʹ�����ſ���</summary>
/// <param name="port"></param>
/// <param name="value"></param>
void HC595Enable(HC595_t* port, bool value);

/// <summary>ʹ��MR���ſ���</summary>
/// <param name="port"></param>
void H595Clear(HC595_t* port);

