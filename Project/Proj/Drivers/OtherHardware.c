#include "OtherHardware.h"
#include "Gpio.h"


#define STATLED PD4

/// <summary>���������ʼ��</summary>
void OtherHardwareInit(void)
{
	GpioInit(STATLED, PIN_OUTPUT, PIN_OUTPUT_PP, PIN_NO_PULL, 0, 0);
}

/// <summary>��ǰָʾ��״̬</summary>
static bool LedStat = false;

/// <summary>״ָ̬ʾ������</summary>
/// <param name="value">��/����</param>
void LedStatSet(bool value)
{
	GpioWrite(STATLED, value);
	LedStat = value;
}

/// <summary>�ڰ����ϵ�LEDָʾ��״̬�ߵ�</summary>
void LedStatReversal(void)
{
	LedStat = !LedStat;
	GpioWrite(STATLED, LedStat);
}

