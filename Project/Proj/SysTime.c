
#include "SysTime.h"
// #include "Cpu.h"

#include "Debug.h"
#include "Cpu.h"
#include "SysInfo.h"

/// <summary>当前系统时间</summary>
static volatile Time_t TimeNow = 0;

/// <summary>系统滴答定时器  中断函数</summary>
void SysTimeIRQ(void)
{
	TimeNow++;
	Bt_ClearIntFlag(SysTimeHw, BtUevIrq);

	if (TimeNow > SysInfo.ActivityTime + 20000)
	{
		DebugPrintf("SysInfo.ActivityTime 20sec\r\n");
		TraceShow();
		SysReset();
	}

	if (TimeNow > SysInfo.RestartTime)
	{
		DebugPrintf("SysInfo.RestartTime\r\n");
		TraceShow();
		SysReset();
	}
}

/// <summary>系统时间初始化</summary>
void SysTimeInit(void)
{
	// 配置 1ms 循环中断
}

/// <summary>获取系统当前时间</summary>
/// <returns>系统当前时间</returns>
Time_t GetCurrentTimeMs(void)
{
	return (Time_t)TimeNow;
}

/// <summary>while(1)等待</summary>
/// <param name="ms">毫秒</param>
void DelayMs(int ms)
{
	Time_t end = GetCurrentTimeMs() + ms;

	while (end > GetCurrentTimeMs());
}

