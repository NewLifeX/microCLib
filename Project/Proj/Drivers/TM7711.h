#pragma once

#include "Type.h"
#include "Gpio.h"
// #include "SysTime.h"

// ֻʹ��10HZ ���ֵ�� 
// �����������ݲ�Ҫ��
// Ҳ����˵��ֻʹ�� 25 ��������͡�

typedef struct
{
	PinName SCK;
	PinName DOUT;

	// push�����õ��±ꡣ
	ushort PushIdx;
	// ����ֵ����
	uint SrcData[12];

	// �ϴζ�ȡ��ֵ������������
	// ÿ�ζ�ȡ��ʱ�򶼻�Ա�һ�¡�
	uint LastValue;

	// �ϴζ�ȡʱ��
	Time_t LastStamp;

	// ���������
	byte ErrCnt;
}TM7711_t;

/// <summary>��ʼ��</summary>
/// <param name="thiss"></param>
/// <returns></returns>
bool TM7711Init(TM7711_t* thiss);

/// <summary>�����ݵ��˲�����</summary>
/// <param name="thiss"></param>
/// <param name="data"></param>
void TM7711PushData(TM7711_t* thiss, uint data);

/// <summary>��ȡ����,�˲���֮�������</summary>
/// <param name="thiss">�����Լ�</param>
/// <returns></returns>
uint TM7711GetDstData(TM7711_t* thiss);

/// <summary>25bit �����ݡ�����10HZ��֡�������λ17bit</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData25(TM7711_t* thiss);

/// <summary>26bit �����ݡ�����40HZ�¶�</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData26(TM7711_t* thiss);

/// <summary>27bit �����ݡ�����40HZ���</summary>
/// <param name="thiss"></param>
/// <returns></returns>
uint TM7711GetData27(TM7711_t* thiss);
