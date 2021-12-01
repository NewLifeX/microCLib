
#include "SysTime.h"
// #include "Cpu.h"

#include "Debug.h"
#include "Cpu.h"
#include "SysInfo.h"

/// <summary>��ǰϵͳʱ��</summary>
static volatile Time_t TimeNow = 0;

/// <summary>ϵͳ�δ�ʱ��  �жϺ���</summary>
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

/// <summary>ϵͳʱ���ʼ��</summary>
void SysTimeInit(void)
{
	// ���� 1ms ѭ���ж�
}

/// <summary>��ȡϵͳ��ǰʱ��</summary>
/// <returns>ϵͳ��ǰʱ��</returns>
Time_t GetCurrentTimeMs(void)
{
	return (Time_t)TimeNow;
}

/// <summary>while(1)�ȴ�</summary>
/// <param name="ms">����</param>
void DelayMs(int ms)
{
	Time_t end = GetCurrentTimeMs() + ms;

	while (end > GetCurrentTimeMs());
}

