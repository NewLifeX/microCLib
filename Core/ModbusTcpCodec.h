#pragma once

#include "Type.h"


#include "CircularQueue.h"
#include "Stream.h"

// ��ȡ modbus tcp/udp ���ݰ����ȣ������������Ӧ��
int MtcGetLength(byte* p, int len);
int MtcGetLenCircularQueue(CircularQueue_t* queue);
int MtcGetLenStream(Stream_t* st);


// modbus tcp/udp ����� rtu ��˵��ͷ�������� 6�ֽڣ�β����2�ֽڡ�
// ���ಿ��һ�¡��ο� modbus-rtu ���ɡ�
// rtu				     addr cmd ndata 2crc
// tcp   2seq 2pro 2len  addr cmd ndata

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	// ���񣬻ظ���������������ֱ�Ӹ�0
	ushort Seq;
	// Modbus �� 0
	ushort Protocol;
	// ���ȣ����
	ushort Length;
	// �� modbus rtu �� addr һ��������û��ʲô���壬һ����ip�����֡�
	byte Addr;
}MtcHead_t;
#pragma pack(pop)

// 01 02 ����ָ� len = 12
int Mtc01a02(MtcHead_t* head, byte cmd, ushort regaddr, ushort bitlen, byte* data, int len);
// 01 02 ָ��ظ��� len > (bitlen+7)/8 + 9;
// bits δ����С�˴���
int MtcResult01a02(MtcHead_t* head, byte cmd, byte* bits, ushort bitlen, byte* data, int len);

// 03 04 ����ָ� len = 12
int Mtc03a04(MtcHead_t* head, byte cmd, ushort regaddr, ushort regcnt, byte* data, int len);
// 03 ָ��ظ��� len > regcnt * 2 + 9
// reg δ����С�˴���
int MtcResult03a04(MtcHead_t* head, byte cmd, byte* reg, ushort regcnt, byte* data, int len);

// 05 06 ����ָ��  len = 12
// 05 ָ����Ч���ݣ� ff00 ��  0000 ��
// 05�� 06 ԭ���ظ����ݡ����÷�װ��
int Mtc05a06(MtcHead_t* head, byte cmd, ushort regaddr, ushort reg, byte* data, int len);

// 10 ����ָ� len > regcnt*2 + 13
// regdata δ����С�˴���
int Mtc10(MtcHead_t* head, ushort regaddr, byte* regdata, ushort regcnt, byte* data, int len);
// 10 ָ��ظ�  len = 12
int MtcResult10(MtcHead_t* head, ushort regaddr, ushort regcnt, byte* data, int len);

