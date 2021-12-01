#pragma once

#include "Type.h"

// 热重启不丢失的数据。
// 不存到FLASH。
typedef struct
{
	/// <summary>UTC时间基础值，相对于CurrTime</summary>
	uint64 UtcTimeBase;
	/// <summary>系统当前时间</summary>
	Time_t CurrTime;
	/// <summary>热重启次数</summary>
	byte HotRestCnt;
}HotData_t;

