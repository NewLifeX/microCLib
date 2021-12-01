
#include "HC165.h"

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
void HC165Init(HC165_t* port)
{
	GpioInit(port->Data, PIN_INPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);

	GpioInit(port->Ce, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
	GpioInit(port->Cp, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
	GpioInit(port->Pl, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
}

/// <summary>采样数据到165芯片内部</summary>
/// <param name="port"></param>
void HC165Take(HC165_t* port)
{
	GpioWrite(port->Ce, true);

	GpioWrite(port->Pl, false);
	DelayUs(port->DelayUs);
	GpioWrite(port->Pl, true);

	GpioWrite(port->Ce, false);
}

/// <summary>读取数据</summary>
/// <param name="port"></param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <param name="bitBigEndian">大端bit</param>
void HC165Read(HC165_t* port, byte* data, int len ,bool bitBigEndian)
{
	if (port == NULL)return;
	if (data == NULL)return;
	if (len < 1)return;

	for (int i = 0; i < len; i++)
	{
		byte dat = 0x00;
		for (int j = 0; j < 8; j++)
		{
			// 下降沿拿数据
			GpioWrite(port->Cp, false);
			DelayUs(port->DelayUs);

			if (GpioRead(port->Data))
			{
				byte mask = 0x01 << j;
				if (bitBigEndian) mask = 0x80 >> j;

				dat |= mask;
			}

			GpioWrite(port->Cp, true);
			// DelayUs(port->DelayUs);
		}

		data[i] = dat;
	}
}
