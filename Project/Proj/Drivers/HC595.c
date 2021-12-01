
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

/// <summary>��ʼ��gpio</summary>
/// <param name="port"></param>
void HC595Init(HC595_t* port)
{
	// ʹ���������ߣ���ʹ��
	GpioInit(port->Enable, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 1);
	GpioInit(port->Reset , PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 1);
	H595Clear(port);

	GpioInit(port->Clk, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
	GpioInit(port->Data, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
	GpioInit(port->PutOut, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);

	HC595Enable(port, false);
}

/// <summary>д�ֽ�</summary>
/// <param name="port"></param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <param name="bitBigEndian">���bit</param>
/// <param name="out"></param>
void HC595Write(HC595_t* port, byte* data, int len, bool bitBigEndian, bool out)
{
	if (port == NULL)return;
	if (data == NULL)return;
	if (len < 1)return;

	// 595 �������������ݡ�
	for (int i = 0; i < len; i++)
	{
		byte dat = data[i];
		for (int  j = 0; j < 8; j++)
		{
			// ���͵�ƽ��ȷ�������صĴ��ڡ�
			GpioWrite(port->Clk, false);

			byte mask = 0x01 << j;
			if (bitBigEndian)mask = 0x80 >> j;

			GpioWrite(port->Data, (dat & mask) != 0);
			// ��ʱ֮��������أ�ȷ�������ȶ���
			DelayUs(port->Delayus);
			GpioWrite(port->Clk, true);
		}
	}

	if (out)
	{
		// ���������
		GpioWrite(port->PutOut, true);
		DelayUs(port->Delayus);
		GpioWrite(port->PutOut, false);
	}
}

/// <summary>ʹ�����ſ���</summary>
/// <param name="port"></param>
/// <param name="value"></param>
void HC595Enable(HC595_t* port, bool value)
{
	// Enable �͵�ƽ��Ч
	GpioWrite(port->Enable, !value);
}

/// <summary>ʹ��MR���ſ���</summary>
/// <param name="port"></param>
void H595Clear(HC595_t* port)
{
	GpioWrite(port->Reset, false);
	DelayUs(port->Delayus);
	GpioWrite(port->Reset, true);
}
