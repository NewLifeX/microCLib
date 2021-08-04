#pragma once

#include "Type.h"
#include "HardwareInfo.h"

/*
需要自行定义  HardwareInfo.h

内部只有一项内容。
电路板日期的define
格式按照 yyyy-MM-dd 格式写。

例如：

#pragma once

// 硬件设计时间，锁板上一定有标记
// 格式按照 yyyy-MM-dd 格式写。
#define HARDWARETIME "2021-05-22"

*/

// 可以给外部使用，打印出来画图时间。
extern const char* HardwareTime;

// 版本号使用电路板时间减去 2000-1-1 00:00:00 得到的分钟数。
uint GetHardwareVersion(void);

/// <summary>获取硬件版本</summary>
/// <param name="addr">固件起始地址</param>
/// <param name="len">固件大小</param>
/// <returns></returns>
uint GetHardwareVersion2(uint addr, int len);
