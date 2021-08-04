#pragma once

#include "Type.h"
#include "HardwareInfo.h"

/*
��Ҫ���ж���  HardwareInfo.h

�ڲ�ֻ��һ�����ݡ�
��·�����ڵ�define
��ʽ���� yyyy-MM-dd ��ʽд��

���磺

#pragma once

// Ӳ�����ʱ�䣬������һ���б��
// ��ʽ���� yyyy-MM-dd ��ʽд��
#define HARDWARETIME "2021-05-22"

*/

// ���Ը��ⲿʹ�ã���ӡ������ͼʱ�䡣
extern const char* HardwareTime;

// �汾��ʹ�õ�·��ʱ���ȥ 2000-1-1 00:00:00 �õ��ķ�������
uint GetHardwareVersion(void);

/// <summary>��ȡӲ���汾</summary>
/// <param name="addr">�̼���ʼ��ַ</param>
/// <param name="len">�̼���С</param>
/// <returns></returns>
uint GetHardwareVersion2(uint addr, int len);
