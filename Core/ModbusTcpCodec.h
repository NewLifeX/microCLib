#pragma once

#include "Type.h"


#include "CircularQueue.h"
#include "Stream.h"

// 获取 modbus tcp/udp 数据包长度，不区分请求和应答
int MtcGetLength(byte* p, int len);
int MtcGetLenCircularQueue(CircularQueue_t* queue);
int MtcGetLenStream(Stream_t* st);


// modbus tcp/udp 相对于 rtu 来说，头部增加了 6字节，尾部少2字节。
// 其余部分一致。参考 modbus-rtu 即可。
// rtu				     addr cmd ndata 2crc
// tcp   2seq 2pro 2len  addr cmd ndata

/*
1:  读线圈
2:	读离散输入
3:  读多个保持寄存器
4:  读输入寄存器
5:  写单个线圈
6:  写单个寄存器
15  写多个线圈 
16	写多个寄存器
*/

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	// 事务，回复抄请求包，请求包直接给0
	ushort Seq;
	// Modbus 填 0
	ushort Protocol;
	// 长度，大端
	ushort Length;
	// 跟 modbus rtu 的 addr 一样，不过没有什么意义，一般用ip来区分。
	byte Addr;
}MtcHead_t;
#pragma pack(pop)

// 01 02 请求指令。 len = 12
int Mtc01a02(MtcHead_t* head, byte cmd, ushort regaddr, ushort bitlen, byte* data, int len);
// 01 02 指令回复。 len > (bitlen+7)/8 + 9;
// bits 未做大小端处理
int MtcResult01a02(MtcHead_t* head, byte cmd, byte* bits, ushort bitlen, byte* data, int len);

// 03 04 请求指令。 len = 12
int Mtc03a04(MtcHead_t* head, byte cmd, ushort regaddr, ushort regcnt, byte* data, int len);
// 03 指令回复。 len > regcnt * 2 + 9
// reg 未做大小端处理
int MtcResult03a04(MtcHead_t* head, byte cmd, byte* reg, ushort regcnt, byte* data, int len);

// 05 06 请求指令  len = 12
// 05 指令有效数据： ff00 合  0000 分
// 05、 06 原样回复数据。不用封装。
int Mtc05a06(MtcHead_t* head, byte cmd, ushort regaddr, ushort reg, byte* data, int len);

// 0f 请求指令， len > bytelen + 9
// bitdata 未做大小端处理
int Mtc0f(MtcHead_t* head, ushort regaddr, byte* bitdata, ushort bitcnt, byte bytelen, byte* data, int len);
// 0f 指令回复  len = 8
int MtcResult0f(MtcHead_t* head, ushort regaddr, ushort bitcnt, byte* data, int len);

// 10 请求指令， len > regcnt*2 + 13
// regdata 未做大小端处理
int Mtc10(MtcHead_t* head, ushort regaddr, byte* regdata, ushort regcnt, byte* data, int len);
// 10 指令回复  len = 12
int MtcResult10(MtcHead_t* head, ushort regaddr, ushort regcnt, byte* data, int len);

