
#include "InOutPoint.h"
#include "HC165.h"
#include "HC595.h"

#include "BitHelper.h"
#include "Debug.h"

InOutInfo_t InOutInfo;

/// <summary>初始化输入输出口</summary>
void InOutPointInit(void)
{
	InOutInfo.HC595.Delayus = 2;
	InOutInfo.HC595.Enable = PC7;
	InOutInfo.HC595.Clk = PC9;
	InOutInfo.HC595.Data = PC6;
	InOutInfo.HC595.Reset = P0;
	InOutInfo.HC595.PutOut = PC8;

	// 初始化引脚
	HC595Init(&InOutInfo.HC595);

	memset(InOutInfo.Out, 0x00, sizeof(InOutInfo.Out));
	OutPointFlash();

	// 使能
	HC595Enable(&InOutInfo.HC595, true);

	InOutInfo.HC165.DelayUs = 2;
	InOutInfo.HC165.Cp = PB6;
	InOutInfo.HC165.Pl = PB5;
	InOutInfo.HC165.Ce = PB4;
	InOutInfo.HC165.Data = PB3;

	// 初始化引脚
	HC165Init(&InOutInfo.HC165);
	// 缓冲一次，可有可无。
	HC165Take(&InOutInfo.HC165);
}

void SetOutPoint(OutPointName_e out, bool value, bool flashOut)
{
	int idx = (int)out;
	if (idx < 0)return;
	if (idx > 7)return;

	SetBit(InOutInfo.Out, idx + 8, value);

	if (flashOut)OutPointFlash();
}

bool GetOutPoint(OutPointName_e out)
{
	int idx = (int)out;
	if (idx < 0)return false;
	if (idx > 7)return false;

	return GetBit(InOutInfo.Out, idx + 8);
}

void OutPointClear(bool flashOut)
{
	InOutInfo.Out[1] = 0x00;
	if (flashOut)OutPointFlash();
}

void OutPointFlash(void)
{
	TraceThis();
	HC595Write(&InOutInfo.HC595, InOutInfo.Out, sizeof(InOutInfo.Out), true, true);
	UnTrace();
}

bool ReadInPoint(InPointName_e in)
{
	int idx = (int)in;
	if (idx < 0)return false;
	if (idx > 7)return false;

	return GetBit(InOutInfo.In, idx);
}

void InPointFlash(void)
{
	TraceThis();

	HC165Take(&InOutInfo.HC165);
	HC165Read(&InOutInfo.HC165, InOutInfo.In, sizeof(InOutInfo.In), true);

	UnTrace();
}


/**************************************************/

void OutLedsStat(byte stat, bool flashOut)
{
	InOutInfo.Out[0] = stat;

	if (flashOut)OutPointFlash();
}



/*
#include "SysTime.h"

void InOutTest(void)
{
	static int idx = 0;

	static Time_t Last = 0;
	Time_t now = GetCurrentTimeMs();
	if (now - Last > 500)
	{
		memset(InOutInfo.Out, 0x00, sizeof(InOutInfo.Out));
		SetBit(InOutInfo.Out, idx, true);

		idx++;
		if (idx >= 16)idx = 0;

		Last = now;
	}
}

void InOutTest2(void)
{
	static int idx = 0;

	static Time_t Last = 0;
	Time_t now = GetCurrentTimeMs();
	if (now - Last > 50)
	{
		InPointFlash();
		InOutInfo.Out[1] = InOutInfo.In[0];
		OutPointFlash();

		Last = now;
	}
}

*/

