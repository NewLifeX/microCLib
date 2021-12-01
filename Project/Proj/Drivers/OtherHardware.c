#include "OtherHardware.h"
#include "Gpio.h"


#define STATLED PD4

/// <summary>其他外设初始化</summary>
void OtherHardwareInit(void)
{
	GpioInit(STATLED, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
}

/// <summary>当前指示灯状态</summary>
static bool LedStat = false;

/// <summary>状态指示灯设置</summary>
/// <param name="value">亮/不亮</param>
void LedStatSet(bool value)
{
	GpioWrite(STATLED, value);
	LedStat = value;
}

/// <summary>在板子上的LED指示灯状态颠倒</summary>
void LedStatReversal(void)
{
	LedStat = !LedStat;
	GpioWrite(STATLED, LedStat);
}

