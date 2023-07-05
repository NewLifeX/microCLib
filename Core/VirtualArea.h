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
int AreaByteCopy(Area_t* dst, Area_t* src);

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


/// <summary>从多个空间读取数据</summary>
/// <param name="srcs">被读空间s</param>
/// <param name="addr">读地址</param>
/// <param name="data">读缓冲</param>
/// <param name="len">读长度</param>
/// <param name="isBit">是否是bit空间</param>
void AreaRead2(Area_t* srcs, int addr, byte* data, int len, bool isBit);

/// <summary>向多个空间写数据</summary>
/// <param name="dsts">被写入</param>
/// <param name="addr">写入地址</param>
/// <param name="data">写入数据</param>
/// <param name="len">写入长度</param>
/// <param name="isBit">是否是bit空间</param>
void AreaWrite2(Area_t* dsts, int addr, byte* data, int len, bool isBit);


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

// bit 线圈 in/out 操作
AreaRead2(BitAreas, addr, data, len ,true);
AreaWrite2(BitAreas, addr, data, len ,true);

// 寄存器操作，因为寄存器是 short 位宽，地址需要 X 2 
AreaRead2(BitAreas, regaddr * 2, data, regcnt * 2 ,true);
AreaWrite2(BitAreas, regaddr * 2, data, regcnt * 2 ,true);

*/




