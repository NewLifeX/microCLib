#pragma once

#include "Type.h"

// ����������ʧ�����ݡ�
// ���浽FLASH��
typedef struct
{
	/// <summary>UTCʱ�����ֵ�������CurrTime</summary>
	uint64 UtcTimeBase;
	/// <summary>ϵͳ��ǰʱ��</summary>
	Time_t CurrTime;
	/// <summary>����������</summary>
	byte HotRestCnt;
}HotData_t;

