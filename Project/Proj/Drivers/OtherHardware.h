#pragma once


#include "Type.h"

/// <summary>其他外设初始化</summary>
void OtherHardwareInit(void);

/// <summary>状态指示灯设置</summary>
/// <param name="value">亮/不亮</param>
void LedStatSet(bool value);

/// <summary>在板子上的LED指示灯状态颠倒</summary>
void LedStatReversal(void);

