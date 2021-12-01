#pragma once

#include "Type.h"

/// <summary>系统时间初始化</summary>
void SysTimeInit(void);

/// <summary>获取系统当前时间</summary>
/// <returns>系统当前时间</returns>
Time_t GetCurrentTimeMs(void);

/// <summary>while(1)等待</summary>
/// <param name="ms">毫秒</param>
void DelayMs(int ms);

/// <summary>收发时间戳</summary>
typedef struct
{
	Time_t Rx;
	Time_t Tx;
}TimeStamp_t;

