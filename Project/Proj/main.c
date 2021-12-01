
#include "Type.h"
#include "Debug.h"
#include "SysTime.h"
#include "SysInfo.h"
#include "HotDataBase.h"

#include "Cpu.h"
#include "Uart.h"
#include "InOutPoint.h"
#include "OtherHardware.h"


#ifdef DEBUG

void* DebugMalloc(size_t size)
{
	void* p = malloc(size);
	DebugPrintf("malloc %d -> 0x%08X\r\n", size, p);

	return p;
}

void DebugFree(void* p)
{
	DebugPrintf("free 0x%08X\r\n", p);
	free(p);
}

extern int __heap_base;
extern int __heap_limit;

/// <summary>日志口初始化</summary>
void DebugInit(void)
{
	ComName com = COM1;

	UsartCfg_t* cfg = UsartCfgCreateOrGet(com);
	cfg->Baudrate = 128000;
	UartInit(com);

	SetDebugPort(com);
	SetDebugLevel(DebugNormal);

	GlobleMalloc = DebugMalloc;
	GlobleFree = DebugFree;

	DebugPrintf("__heap_base  0x%08X\r\n", &__heap_base);
	DebugPrintf("__heap_limit 0x%08X\r\n", &__heap_limit);
}
#endif

int main(void)
{
	// 硬件时钟配置
	SYSTint();
	// 系统时钟配置
	SysTimeInit();
	
	// 紧要的输入输出点初始化。
	// InOutPointInit();

#ifdef DEBUG
	DebugInit();
#endif

	// 系统变量初始化。
	SysInit();
	// 其他硬件初始化。独立心跳LED、
	OtherHardwareInit();

	// 热重启SysInfo.HotData数据判断和使用。
	// XXXX()


	// 配置无效的时候必须进入 的功能
	// 或者进入首页。
	// SysInfo.ConfigEff = false;
	if (!SysInfo.ConfigEff)
	{
		
	}

	while (true)
	{
		
		Time_t now = GetCurrentTimeMs();
		// 活动时间。  如果长时间不更新，系统时钟中断会打印调用栈+重启CPU。
		SysInfo.ActivityTime = now;
		// 数据热保存时刻
		SysInfo.HotData.CurrTime = now;
		// 保存热数据，避免重启一些关键数据丢失
		SaveHotData(&SysInfo.HotData);

		// 输入输出口周期扫描。
		OutPointFlash();
		InPointFlash();
		// 指示灯闪烁
		LedStatShow();

		// TraceCall 存调用栈。 进入 hardware fault 之后可以打印出调用栈信息。方便bug定位。
		// TraceCall(abc());
	}
}


