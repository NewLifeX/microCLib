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

/// <summary>��ʼ��gpio</summary>
/// <param name="port"></param>
void HC165Init(HC165_t* port);

/// <summary>�������ݵ�165оƬ�ڲ�</summary>
/// <param name="port"></param>
void HC165Take(HC165_t* port);

/// <summary>��ȡ����</summary>
/// <param name="port"></param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <param name="bitBigEndian">���bit</param>
void HC165Read(HC165_t* port, byte* data, int len, bool bitBigEndian);

