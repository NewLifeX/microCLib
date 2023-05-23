#pragma once

#include "Type.h"
#include "Stream.h"
#include "CircularQueue.h"

/*

// modbus 请求包格式

// 请求包
// 01   addr+cmd+2regaddr+2bitcnt+2crc
// 02   addr+cmd+2regaddr+2bitcnt+2crc
// 03   addr+cmd+2regaddr+2regcnt+2crc
// 04   addr+cmd+2regaddr+2regcnt+2crc
// 05   addr+cmd+2regaddr+2data+2crc
// 06   addr+cmd+2regaddr+2data+2crc
// 10	addr+cmd+2regaddr+2regcnt+bytelen+ (bytelen*data) +2crc

// 回复包
// 01	addr+cmd+bitcnt+ N * bits +2crc
// 02	addr+cmd+bitcnt+ N * bits +2crc
// 03	addr+cmd+bytelen+ (bytelen*data) +2crc
// 04	addr+cmd+bytelen+ (bytelen*data) +2crc
// 05   06   等于请求包
// 10	addr+cmd+2regaddr+2regcnt+2crc

解释：
// addr 站号
// cmd 功能码
// regaddr 寄存器地址
// regcnt  寄存器个数
// bitcnt  bit位数
// bytelen 字节数
// data    数据
// crc	   校验码

*/

// 获取有效的modbus rtu数据包长度（会校验数据包）
// 从机获取数据包长度
// 0 没有足够长度的数据。
// -1 数据包校验出错
// -2 数据长度不对，modbus rtu 数据包小于 130 字节。
int MrcSlaveGetLength(byte* data, int len);
int MrcSlaveGetLenCircularQueue(CircularQueue_t* queue);
int MrcSlaveGetLenStream(Stream_t* st);

// 01 02 请求指令。 len = 8
int Mrc01a02(byte addr, byte cmd, ushort regaddr, ushort bitlen, byte* data, int len);
// 01 02 指令回复。 len > (bitlen+7)/8 + 5;
// bits 未做大小端处理
int MrcResult01a02(byte addr,byte cmd, byte* bits, ushort bitlen, byte* data, int len);

// 03 04 请求指令。 len = 8
int Mrc03a04(byte addr, byte cmd, ushort regaddr, ushort regcnt, byte* data, int len);
// 03 指令回复。 len > regcnt * 2 + 5
// reg 未做大小端处理
int MrcResult03a04(byte addr,byte cmd, byte* reg, ushort regcnt, byte* data, int len);

// 05 06 请求指令  len = 8
// 05 指令有效数据： ff00 合  0000 分
// 05、 06 原样回复数据。不用封装。
int Mrc05a06(byte addr, byte cmd, ushort regaddr, ushort reg, byte* data, int len);

// 10 请求指令， len > regcnt*2 + 9
// regdata 未做大小端处理
int Mrc10(byte addr, ushort regaddr, byte* regdata, ushort regcnt, byte* data, int len);
// 10 指令回复  len = 8
int MrcResult10(byte addr, ushort regaddr, ushort regcnt, byte* data, int len);

