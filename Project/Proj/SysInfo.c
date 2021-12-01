
#include "SysInfo.h"
#include "Type.h"

#include "Flash.h"
#include "ConfigBase.h"

#include "Debug.h"

#include "HotDataBase.h"
#include "Cpu.h"
#include "Version.h"
#include "HardwareVersion.h"

SysInfo_t SysInfo;

/// <summary>系统初始化</summary>
void SysInit(void)
{
	SysInfo.ConfigSaveAddr = GetChipStartAddr(0) + GetFlashSize() - GetFlashBlockSize(0);

	byte cpuid[12];
	GetCpuid(cpuid, sizeof(cpuid));
	ShowArray("CPU ID ", cpuid, sizeof(cpuid), sizeof(cpuid));

	SysInfo.ProVersion = 0;
	SysInfo.HardwareVersion = GetHardwareVersion();
	SysInfo.SoftVersion = GetVersion();
	SysInfo.RestartTime = UINT64_MAX - 60000;

	DebugPrintf("HardwareVersion\t0x%08X\r\n", SysInfo.HardwareVersion);
	DebugPrintf("\t%s\r\n", HardwareTime);
	DebugPrintf("SoftVersion\t\t0x%08X\r\n", SysInfo.SoftVersion);
	DebugPrintf("\t%s\r\n", FirmwareBuildTime);

	SysInfo.ConfigEff = true;
	if (!GetConfig(&SysInfo.Config, SysInfo.ConfigSaveAddr))
	{
		DebugPrintf("GetConfig Fail, Load default\r\n");
		SysInfo.ConfigEff = false;
		// 获取失败，加载默认配置
		Config_t* cfg = &SysInfo.Config;
		memset(cfg, 0x00, sizeof(Config_t));

	}
	else
	{
		DebugPrintf("GetConfig Success\r\n");
	}

	// 后备内存初始化
	BackupRegInit();
	DebugPrintf("HotDataSize %d + 8\r\n",sizeof(SysInfo.HotData));

	// 后备数据
	if (!GetHotData(&SysInfo.HotData))
	{
		HotData_t* hot = &SysInfo.HotData;
		memset(hot, 0x00, sizeof(HotData_t));
	}
	else
	{
		SysInfo.HotData.HotRestCnt++;

		SysInfo.HotData.UtcTimeBase += SysInfo.HotData.CurrTime;
		// SysInfo.HotData.CurrTime = GetCurrentTimeMs();
	}

	DebugPrintf("\tHotResetCnt\t%d\r\n", SysInfo.HotData.HotRestCnt);
}

/// <summary>保存配置内容到Flash</summary>
/// <returns>返回是否成功</returns>
bool SaveConfig(void)
{
	bool rs = SetConfig(&SysInfo.Config, SysInfo.ConfigSaveAddr);
	DebugPrintf("SaveConfig(cfg,%08X) %s\r\n", SysInfo.ConfigSaveAddr, rs ? "True" : "False");
	return rs;
}

#include "OtherHardware.h"
#include "SysTime.h"

/// <summary>状态指示灯处理</summary>
/// <param name=""></param>
/// <remark>平时慢删，有数据包眨眼</remark>
void LedStatShow(void)
{
	uint now = GetCurrentTimeMs();
	static bool start = false;
	// 定时翻转一下led，表示还活着。
	static uint ledrevertime = 0;

	// if (now - SysInfo.LastRxMsgTime < 80)
	if (false)
	{
		static uint time = 0;
		if (start)
		{
			LedStatSet(false);
			start = false;
			time = now;
		}
		else
		{
			if ((now - time) / 32)
			{
				LedStatReversal();
				time = now;
				ledrevertime = now;
			}
		}
	}
	else
	{
		start = true;

		if (now - ledrevertime > 1000)
		{
			LedStatReversal();
			ledrevertime = GetCurrentTimeMs();
		}
	}
}

/// <summary>相对于 GetCurrentTimeMs() = 0 的时候的UTC时间。单位ms</summary>
// static uint64 UtcTimeBase = 0;  移到 HotData_t
uint64 GetUtcMs(void)
{
	return GetCurrentTimeMs() + SysInfo.HotData.UtcTimeBase;
}

void SetUtcMs(uint64 utcMs)
{
	SysInfo.HotData.UtcTimeBase = utcMs - GetCurrentTimeMs();
}
