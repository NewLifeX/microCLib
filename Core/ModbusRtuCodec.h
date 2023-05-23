#pragma once

#include "Type.h"
#include "Stream.h"
#include "CircularQueue.h"

/*

// modbus �������ʽ

// �����
// 01   addr+cmd+2regaddr+2bitcnt+2crc
// 02   addr+cmd+2regaddr+2bitcnt+2crc
// 03   addr+cmd+2regaddr+2regcnt+2crc
// 04   addr+cmd+2regaddr+2regcnt+2crc
// 05   addr+cmd+2regaddr+2data+2crc
// 06   addr+cmd+2regaddr+2data+2crc
// 10	addr+cmd+2regaddr+2regcnt+bytelen+ (bytelen*data) +2crc

// �ظ���
// 01	addr+cmd+bitcnt+ N * bits +2crc
// 02	addr+cmd+bitcnt+ N * bits +2crc
// 03	addr+cmd+bytelen+ (bytelen*data) +2crc
// 04	addr+cmd+bytelen+ (bytelen*data) +2crc
// 05   06   ���������
// 10	addr+cmd+2regaddr+2regcnt+2crc

���ͣ�
// addr վ��
// cmd ������
// regaddr �Ĵ�����ַ
// regcnt  �Ĵ�������
// bitcnt  bitλ��
// bytelen �ֽ���
// data    ����
// crc	   У����

*/

// ��ȡ��Ч��modbus rtu���ݰ����ȣ���У�����ݰ���
// �ӻ���ȡ���ݰ�����
// 0 û���㹻���ȵ����ݡ�
// -1 ���ݰ�У�����
// -2 ���ݳ��Ȳ��ԣ�modbus rtu ���ݰ�С�� 130 �ֽڡ�
int MrcSlaveGetLength(byte* data, int len);
int MrcSlaveGetLenCircularQueue(CircularQueue_t* queue);
int MrcSlaveGetLenStream(Stream_t* st);

// 01 02 ����ָ� len = 8
int Mrc01a02(byte addr, byte cmd, ushort regaddr, ushort bitlen, byte* data, int len);
// 01 02 ָ��ظ��� len > (bitlen+7)/8 + 5;
// bits δ����С�˴���
int MrcResult01a02(byte addr,byte cmd, byte* bits, ushort bitlen, byte* data, int len);

// 03 04 ����ָ� len = 8
int Mrc03a04(byte addr, byte cmd, ushort regaddr, ushort regcnt, byte* data, int len);
// 03 ָ��ظ��� len > regcnt * 2 + 5
// reg δ����С�˴���
int MrcResult03a04(byte addr,byte cmd, byte* reg, ushort regcnt, byte* data, int len);

// 05 06 ����ָ��  len = 8
// 05 ָ����Ч���ݣ� ff00 ��  0000 ��
// 05�� 06 ԭ���ظ����ݡ����÷�װ��
int Mrc05a06(byte addr, byte cmd, ushort regaddr, ushort reg, byte* data, int len);

// 10 ����ָ� len > regcnt*2 + 9
// regdata δ����С�˴���
int Mrc10(byte addr, ushort regaddr, byte* regdata, ushort regcnt, byte* data, int len);
// 10 ָ��ظ�  len = 8
int MrcResult10(byte addr, ushort regaddr, ushort regcnt, byte* data, int len);

