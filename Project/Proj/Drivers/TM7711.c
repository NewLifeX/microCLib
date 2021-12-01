#include "TM7711.h"
#include "Array.h"
#include "SysTime.h"
#include "Debug.h"

#define DisableIrq() __disable_irq()
#define EnableIrq() __enable_irq()

static void DelayUs(int us)
{
	for (int i = 0; i < us; i++)
	{
		for (volatile int j = 0; j < 15; j++);
	}
}

/// <summary>初始化</summary>
/// <param name="thiss"></param>
/// <returns></returns>
bool TM7711Init(TM7711_t* thiss)
{
	if (thiss == NULL)return false;
	if (thiss->SCK == P0)return false;
	if (thiss->DOUT == P0)return false;

	memset(thiss->SrcData, 0x00, sizeof(thiss->SrcData));
	thiss->PushIdx = 0;

	GpioInit(thiss->SCK, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
	GpioInit(thiss->DOUT, PIN_INPUT, PIN_OUTPUT_PP, PIN_PULL_UP, 0, 0);

	thiss->LastStamp = GetCurrentTimeMs();
	thiss->ErrCnt = 0;

	return true;
}

/// <summary>获取数据</summary>
/// <param name="thiss">对象自己</param>
/// <returns></returns>
uint TM7711GetDstData(TM7711_t* thiss)
{
	// 去掉最大值，去掉最小值。 取一半做平均数。
	uint temp[ArrayLength(thiss->SrcData)];

	memcpy(temp, thiss->SrcData, sizeof(temp));
	// 排序
	ArraySortUint(temp, ArrayLength(temp));

	int sun = 0;
	int len = ArrayLength(temp) - 4;
	for (int i = 2; i < len+2; i++)
	{
		sun += temp[i];
	}

	int avevalue = sun / len;

	if (abs(thiss->LastValue - avevalue) < 16)
	{
		return thiss->LastValue & (~16);
	}
	
	thiss->LastValue = avevalue;
	return avevalue & (~16);

	// return temp[ArrayLength(temp) / 2];
}

/// <summary>丢数据到滤波器内</summary>
/// <param name="thiss"></param>
/// <param name="data"></param>
void TM7711PushData(TM7711_t* thiss, uint data)
{
	// 24bit数据。判断一下越界。
	if ((data & 0xfe000000) != 0)return;
	if (data < 0x10000)return;

	if (thiss->PushIdx >= ArrayLength(thiss->SrcData)) thiss->PushIdx = 0;

	thiss->SrcData[thiss->PushIdx] = data;
	thiss->PushIdx++;
}

static uint _TM7711GetData25(TM7711_t* thiss)
{
	// 上升沿 -- 设备输出电平
	// 输出一直延伸到，下一个上升沿
	uint data = 0;
	for (int i = 0; i < 24; i++)
	{
		GpioWrite(thiss->SCK, true);
		DelayUs(1);
		GpioWrite(thiss->SCK, false);

		DelayUs(1);
		data <<= 1;
		if (GpioRead(thiss->DOUT))
		{
			data |= 0x01;
		}
	}

	GpioWrite(thiss->SCK, true);
	DelayUs(1);
	GpioWrite(thiss->SCK, false);

	// 负数
	if ((data & 0x00800000) != 0)
	{
		data &= 0x7fffff;
		// data |= 0x80000000;
	}
	else
	{
		data |= 0x800000;
	}

	return data;
}

static void Reset(TM7711_t* thiss)
{
	Time_t now = GetCurrentTimeMs();

	GpioWrite(thiss->SCK, true);
	DebugPrintf("TM Reset\r\n");
	DelayMs(5);
	GpioWrite(thiss->SCK, false);

	DisableIrq();
	_TM7711GetData25(thiss);
	EnableIrq();

	thiss->LastStamp = now;
	thiss->ErrCnt = 0;
}

/// <summary>25bit 读数据。设置10HZ差分。无噪声位17bit</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData25(TM7711_t* thiss)
{
	Time_t now = GetCurrentTimeMs();
	if (now - thiss->LastStamp > 1000)
	{
		Reset(thiss);
		return 0xffffffff;
	}

	if (now - thiss->LastStamp < 90)return 0xffffffff;
	// GpioWrite(thiss->SCK, false);

	// DOUT 高电平，表示数据还未准备好。
	if (GpioRead(thiss->DOUT))return 0xffffffff;
	// 读取时间。
	thiss->LastStamp = now;

	DisableIrq();
	uint data = _TM7711GetData25(thiss);
	EnableIrq();

	bool err = (((data & 0x7ffff0) == 0) || ((data & 0x7ffff0) == 0x7ffff0));
	if (err)thiss->ErrCnt++;
	else thiss->ErrCnt = 0;

	// 进入休眠
	if (thiss->ErrCnt > 8)
	{
		Reset(thiss);
		return 0xffffffff;
	}

	return data;
}

/// <summary>26bit 读数据。设置40HZ温度</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData26(TM7711_t* thiss)
{
	Time_t now = GetCurrentTimeMs();	
	if (now - thiss->LastStamp > 1000)
	{
		Reset(thiss);
		return 0xffffffff;
	}

	if (now - thiss->LastStamp < 15)return 0xffffffff;
	// GpioWrite(thiss->SCK, false);
	// DOUT 高电平，表示数据还未准备好。
	if (GpioRead(thiss->DOUT))return 0xffffffff;
	// 读取时间。
	thiss->LastStamp = now;

	DisableIrq();
	uint data = _TM7711GetData25(thiss);

	DelayUs(1);
	GpioWrite(thiss->SCK, true);
	DelayUs(1);
	GpioWrite(thiss->SCK, false);

	EnableIrq();

	bool err = (((data & 0x7ffff0) == 0) || ((data & 0x7ffff0) == 0x7ffff0));
	if (err)thiss->ErrCnt++;
	else thiss->ErrCnt = 0;

	// 进入休眠
	if (thiss->ErrCnt > 8)
	{
		Reset(thiss);
		return 0xffffffff;
	}

	return data;
}

/// <summary>27bit 读数据。设置40HZ差分</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData27(TM7711_t* thiss)
{
	Time_t now = GetCurrentTimeMs();	
	if (now - thiss->LastStamp > 1000)
	{
		Reset(thiss);
		return 0xffffffff;
	}

	if (now - thiss->LastStamp < 15)return 0xffffffff;
	// GpioWrite(thiss->SCK, false);
	// DOUT 高电平，表示数据还未准备好。
	if (GpioRead(thiss->DOUT))return 0xffffffff;
	// 读取时间。
	thiss->LastStamp = now;

	DisableIrq();
	uint data = _TM7711GetData25(thiss);

	DelayUs(1);
	GpioWrite(thiss->SCK, true);
	DelayUs(1);
	GpioWrite(thiss->SCK, false);

	DelayUs(1);
	GpioWrite(thiss->SCK, true);
	DelayUs(1);
	GpioWrite(thiss->SCK, false);

	EnableIrq();

	bool err = (((data & 0x7ffff0) == 0) || ((data & 0x7ffff0) == 0x7ffff0));
	if (err)thiss->ErrCnt++;
	else thiss->ErrCnt = 0;

	// 进入休眠
	if (thiss->ErrCnt > 8)
	{
		Reset(thiss);
		return 0xffffffff;
	}

	return data;
}

/*

#include "Debug.h"
#include "SysTime.h"

static TM7711_t TmInfo[4] =
{
	{ .SCK = PB10, .DOUT = PB11,},
	{ .SCK = PB1,  .DOUT = PB2, },
	{ .SCK = PD3 , .DOUT = PB7, },
	{ .SCK = PB9,  .DOUT = PB8, },
};

void TM7711TestInit(void)
{
	TM7711Init(&TmInfo[0]);
	TM7711Init(&TmInfo[1]);
	TM7711Init(&TmInfo[2]);
	TM7711Init(&TmInfo[3]);
}

void TM7711Test(void)
{
	Time_t now = GetCurrentTimeMs();
	static Time_t TmLast = 0;
	if (now - TmLast >= 105)
	{
		TM7711PushData(&TmInfo[0], TM7711GetData25(&TmInfo[0]));
		TM7711PushData(&TmInfo[1], TM7711GetData25(&TmInfo[1]));
		TM7711PushData(&TmInfo[2], TM7711GetData25(&TmInfo[2]));
		TM7711PushData(&TmInfo[3], TM7711GetData25(&TmInfo[3]));

		static byte cnt = 0;
		cnt++;
		if (cnt >= 4)
		{
			int dst1 = TM7711GetDstData(&TmInfo[0]);
			int dst2 = TM7711GetDstData(&TmInfo[1]);
			int dst3 = TM7711GetDstData(&TmInfo[2]);
			int dst4 = TM7711GetDstData(&TmInfo[3]);
			DebugPrintf("%08X\t%08X\t%08X\t%08X\r\n", dst1, dst2, dst3, dst4);
			// DebugPrintf("%10d\t%10d\t%10d\t%10d\r\n", dst1, dst2, dst3, dst4);
			cnt = 0;
		}

		TmLast = now;
	}
}

*/
