#pragma once

#include "Type.h"
#include "SysTime.h"
#include "Config.h"
#include "HotData.h"


// 系统信息
typedef struct
{
	/// <summary>重启不能丢失的数据</summary>
	HotData_t HotData;

	// 根据应用需要的全局状态
	// XXXX



	/// <summary>重启时间，用于异步重启。比如 命令下发重启，需要回复完消息后才能重启。</summary>
	Time_t RestartTime;

	// Main 每循环一次，记录一次。如果超时未动作，表示已死机，需要重启。
	Time_t ActivityTime;

	// 协议版本
	ushort ProVersion;
	// 软件编译时间--（当版本来用）
	// 值等于 完全编译时间-2020.1.1 00：00：00 的分钟数
	uint SoftVersion;

	// 硬件版本,一般标记到PCB板上。
	uint HardwareVersion;

	// Config 保存在Flash的地址
	uint ConfigSaveAddr;
	// Config 是否有效，无效的时候自动使用各自模块的默认配置。
	bool ConfigEff;
	/// <summary>配置</summary>
	Config_t Config;
} SysInfo_t;

extern SysInfo_t SysInfo;

/// <summary>系统初始化，配置读取，系统状态初始化</summary>
void SysInit(void);

/// <summary>保存配置内容到Flash</summary>
/// <returns>返回是否成功</returns>
bool SaveConfig(void);

/// <summary>状态指示灯处理</summary>
/// <param name=""></param>
/// <remark>平时慢删，有数据包眨眼</remark>
void LedStatShow(void);

/// <summary>获取UTC时间</summary>
uint64 GetUtcMs(void);

/// <summary>设置UTC时间</summary>
/// <param name="utcMs">UTC时间 单位ms</param>
void SetUtcMs(uint64 utcMs);

