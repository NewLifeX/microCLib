#pragma once

#include "Type.h"

/*
* 映射的虚拟地址空间的数据操作。
* 通讯中给定地址读写数据以驱动业务运行。
* 比如 modbus 系列的通讯。
*/

/// <summary>空间片段定义</summary>
typedef struct VirtualArea
{
	// 起始虚拟地址
	uint Addr;
	// 空间大小
	uint Size;

	// 实际存储
	byte* Buff;
	// 使用空间之前的处理,传参自己
	void(*Init)(struct VirtualArea* thiss);
	// 写入数据后刷新，传参自己
	void(*Fresh)(struct VirtualArea* thiss);
}Area_t;

// 将 src 的数据写到 dst 内。
int AreaCopy(Area_t* dst, Area_t* src);

// 将 src 的 bit 数据写到 dst 内。
int AreaBitCopy(Area_t* dst, Area_t* src);

/// <summary>读多个空间数据，合并，半截</summary>
/// <param name="dst">目标空间</param>
/// <param name="srcs">空间数组，最后一项的 SIZE 小于 1</param>
/// <param name="isBit">是否是bit空间</param>
void AreaRead(Area_t* dst, Area_t* srcs, bool isBit);

/// <summary>写多个空间数据，从大空间拆到小空间，拆出来 fresh 一下</summary>
/// <param name="dsts">目标空间数组，最后一项的 SIZE 小于 1</param>
/// <param name="src">源</param>
/// <param name="isBit">是否是bit空间</param>
void AreaWrite(Area_t* dsts, Area_t* src, bool isBit);


/*
modbus 参考使用。


void OutInit(Area_t* thiss)
{
	// 读输出状态
	static byte local[1];
	thiss->Buff = local;
}

void OutFresh(Area_t* thiss)
{
	// 刷新 IO 输出
}

void InInit(Area_t* thiss)
{
	// 读输入状态
	static byte local[1];
	thiss->Buff = local;
}

Area_t BitAreas[] =
{
	// 线圈
	{.Addr = 0x0000,.Size = 6,.Init = OutInit,.Fresh = OutFresh },
	// 离散输入
	{.Addr = 0x2020,.Size = 6,.Init = InInit,.Fresh = NULL },
	{.Size = 0 }
};

void InfoInit(Area_t* thiss)
{
	// 产品信息
}

// 注意，这里的地址是 字节地址，碰到 modbus 需要 X2 处理
Area_t ByteAreas[] =
{
	// 寄存器
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




