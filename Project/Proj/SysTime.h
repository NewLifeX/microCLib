#pragma once

#include "Type.h"

/// <summary>ϵͳʱ���ʼ��</summary>
void SysTimeInit(void);

/// <summary>��ȡϵͳ��ǰʱ��</summary>
/// <returns>ϵͳ��ǰʱ��</returns>
Time_t GetCurrentTimeMs(void);

/// <summary>while(1)�ȴ�</summary>
/// <param name="ms">����</param>
void DelayMs(int ms);

/// <summary>�շ�ʱ���</summary>
typedef struct
{
	Time_t Rx;
	Time_t Tx;
}TimeStamp_t;

