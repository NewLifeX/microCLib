#pragma once

#include "Type.h"

/*
* ӳ��������ַ�ռ�����ݲ�����
* ͨѶ�и�����ַ��д����������ҵ�����С�
* ���� modbus ϵ�е�ͨѶ��
*/

/// <summary>�ռ�Ƭ�ζ���</summary>
typedef struct VirtualArea
{
	// ��ʼ�����ַ
	uint Addr;
	// �ռ��С
	uint Size;

	// ʵ�ʴ洢
	byte* Buff;
	// ʹ�ÿռ�֮ǰ�Ĵ���,�����Լ�
	void(*Init)(struct VirtualArea* thiss);
	// д�����ݺ�ˢ�£������Լ�
	void(*Fresh)(struct VirtualArea* thiss);
}Area_t;

// �� src ������д�� dst �ڡ�
int AreaCopy(Area_t* dst, Area_t* src);

// �� src �� bit ����д�� dst �ڡ�
int AreaBitCopy(Area_t* dst, Area_t* src);

/// <summary>������ռ����ݣ��ϲ������</summary>
/// <param name="dst">Ŀ��ռ�</param>
/// <param name="srcs">�ռ����飬���һ��� SIZE С�� 1</param>
/// <param name="isBit">�Ƿ���bit�ռ�</param>
void AreaRead(Area_t* dst, Area_t* srcs, bool isBit);

/// <summary>д����ռ����ݣ��Ӵ�ռ��С�ռ䣬����� fresh һ��</summary>
/// <param name="dsts">Ŀ��ռ����飬���һ��� SIZE С�� 1</param>
/// <param name="src">Դ</param>
/// <param name="isBit">�Ƿ���bit�ռ�</param>
void AreaWrite(Area_t* dsts, Area_t* src, bool isBit);


/*
modbus �ο�ʹ�á�


void OutInit(Area_t* thiss)
{
	// �����״̬
	static byte local[1];
	thiss->Buff = local;
}

void OutFresh(Area_t* thiss)
{
	// ˢ�� IO ���
}

void InInit(Area_t* thiss)
{
	// ������״̬
	static byte local[1];
	thiss->Buff = local;
}

Area_t BitAreas[] =
{
	// ��Ȧ
	{.Addr = 0x0000,.Size = 6,.Init = OutInit,.Fresh = OutFresh },
	// ��ɢ����
	{.Addr = 0x2020,.Size = 6,.Init = InInit,.Fresh = NULL },
	{.Size = 0 }
};

void InfoInit(Area_t* thiss)
{
	// ��Ʒ��Ϣ
}

// ע�⣬����ĵ�ַ�� �ֽڵ�ַ������ modbus ��Ҫ X2 ����
Area_t ByteAreas[] =
{
	// �Ĵ���
	{.Addr = 0x8000,.Size = 16,.Init = InfoInit,.Fresh = NULL },
	{.Size = 0 }
};

int ReadBit(ushort addr, byte* data, int len)
{
	Area_t ar;
	ar.Addr = addr;
	ar.Size = len;
	ar.Buff = data;

	AreaRead(&ar, BitAreas, true);
}

int WriteBit(ushort addr, byte* data, int len)
{
	Area_t ar;
	ar.Addr = addr;
	ar.Size = len;
	ar.Buff = data;

	AreaWrite(BitAreas, &ar, true);
}

int ReadByte(ushort addr, byte* data, int len)
{
	Area_t ar;
	ar.Addr = addr;
	ar.Size = len;
	ar.Buff = data;

	AreaRead(&ar, ByteAreas, false);
}
*/




