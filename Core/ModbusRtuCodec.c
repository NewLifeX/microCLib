
#include "ModbusRtuCodec.h"
#include "Crc.h"

#include "Debug.h"

// 特别注意，modbusrtu 数据域是大端，crc 是小端。


static bool Check(byte* data, int len)
{
	if (data == NULL)return false;

	ushort crc = CaclcCRC16_MODBUS(data, len - 2);
	ushort pktcrc = data[len - 2] * 256 + data[len - 1];
	if (crc == pktcrc)return true;
	ushort pktcrc2 = data[len - 1] * 256 + data[len - 2];
	if (crc == pktcrc2)return true;

	// DebugPrintf("%04X  %04X", crc, pktcrc);

	return false;
}

int MrcSlaveGetLength(byte* p, int len)
{
	if (p == NULL)return 0;
	if (len < 8)return 0;

	byte cmd = p[1];
	switch (cmd)
	{
		// 读指令长度都是 8 字节。
		// 写单个寄存器长度也是 8字节。
	case 1: // 读线圈 
	case 2:	// 读离散输入 
	case 3: // 读多个保持寄存器 
	case 4: // 读输入寄存器  
	case 5: // 写单个线圈 
	case 6: // 写单个寄存器 
	{
		if (Check(p, 8))return 8;
		return -1;
	}

	case 0x0f:	// 写多个线圈 
	case 0x10:	// 写多个寄存器
	{
		// addr,cmd,regaddr2+regcnt2+len+ dataXlen +crc
		ushort bytelen = p[6];
		// ushort regcnt = p[4] * 256 + p[5];
		// 0f 指令的 regcnt 是 bitcnt。不能做这个判断。
		// if (regcnt != bytelen / 2)return -2;
		if (bytelen > 122)return -2;
		if (len < bytelen)return 0;
		// 固定长度是 9 字节
		ushort pglen = bytelen + 9;
		if (Check(p, pglen))return pglen;
		return -1;
	}

	default: return -1;
	}
}

int MrcSlaveGetLenCircularQueue(CircularQueue_t* queue)
{
	if (queue == NULL)return 0;
	int remian = CircularQueueGetLength(queue);
	if (remian < 8)return 0;

	// 拿到 cmd
	byte cache[8];
	CircularQueueReads(queue, cache, sizeof(cache), true);
	byte cmd = cache[1];

	switch (cmd)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	{
		if (Check(cache, 8))return 8;
		return -1;
	}

	case 0x0f:
	case 0x10:
	{
		/*
		// 因为 CircularQueue_t 是读写分离的，所以防止被修改，需要copy出来。
		CircularQueue_t queue2;
		CircularQueue_t* q = &queue2;
		memcpy(q, queue, sizeof(CircularQueue_t));
		// 需要反复操作，保留 tail 指针位置。
		byte* tail = q->pTail;
		*/

		// addr,cmd,regaddr2+regcnt2+len+ dataXlen +crc
		ushort len = cache[6];
		// ushort regcnt = cache[4] * 256 + cache[5];
		// 0f 指令的 regcnt 是 bitcnt。不能做这个判断。
		// if (regcnt != len / 2)return -2;
		// if (len == 0)return -2;
		if (len > 122)return -2;
		if (remian < len)return 0;

		ushort pglen = len + 9;
		// 10 数据包不会大于 130 字节。
		byte pgcache[130];
		CircularQueueReads(queue, pgcache, pglen, true);

		if (Check(pgcache, pglen))return pglen;
		return -1;
	}

	default: return -1;
	}
}

int MrcSlaveGetLenStream(Stream_t* st)
{
	int remain = st->Size - st->Position;
	byte* p = &st->MemStart[st->Position];

	return MrcSlaveGetLength(p, remain);
}

/// <summary>通过包头计算数据长度，不校验</summary>
/// <param name="p">最少给3个字节的数组</param>
/// <returns>理论上包长。-1 数据包类型不对，0 p为null</returns>
static int _masterCalcRxLen(byte* p)
{
	if (p == NULL)return 0;

	byte cmd = p[1];
	switch (cmd)
	{
	case 1:
	case 2:
	{
		byte bitlen = p[2];
		byte bytelen = (bitlen + 7) / 8;
		// addr+cmd+bitcnt + 2crc * 5
		int pklen = 5 + bytelen;

		return pklen;
	}
	case 3:
	case 4:
	{
		byte bytelen = p[2];
		// addr+cmd+bytelen+2crc = 5
		int pklen = 5 + bytelen;

		return pklen;
	}

	case 5:
	case 6:
	case 0xf:
	case 0x10:
	{
		return 8;
	}

	default: return -1;
	}
}

int MrcMasterGetRxLength(byte* p, int len)
{
	if (p == NULL)return 0;
	if (len < 5)return 0;

	int pklen = _masterCalcRxLen(p);

	// 数据包类型不对
	if (pklen < 0)return -1;
	// 长度超出
	if (pklen > 130)return -2;
	// 长度不够
	if (pklen > len)return 0;

	if (Check(p, pklen))return pklen;
	// 校验失败
	return -1;
}

int MrcMasterGetRxLenCircularQueue(CircularQueue_t* queue)
{
	if (queue == NULL)return 0;
	int remian = CircularQueueGetLength(queue);
	if (remian < 6)return 0;

	// 缓冲
	byte cache[130];
	// 读3个字节判断长度
	CircularQueueReads(queue, cache, 3, true);
	int pklen = _masterCalcRxLen(cache);
	// 数据包类型不对
	if (pklen < 0)return -1;
	// 长度超出
	if (pklen > 130)return -2;
	// 数据包长度
	if (pklen > remian)return 0;
		
	// if (sizeof(cache) >= pklen)
	// {

	// 直接使用缓冲区读出来判断
	CircularQueueReads(queue, cache, pklen, true);	
	if (Check(cache, pklen))return pklen;
	// 校验失败
	return -1;

	// }
	// else
	// {
	// 	// 再弄一个缓冲区使用
	// 	byte* p = (byte*)GlobleMalloc(pklen);
	// 	if (p == NULL)return -1;
	// 
	// 	// 读出来校验一下
	// 	CircularQueueReads(queue, p, pklen, true);
	// 	bool crcok = Check(p, pklen);
	// 	GlobleFree(p);
	// 
	// 	if (crcok)return pklen;
	// 	// 校验失败
	// 	return -1;
	// }
}

int MrcMasterGetRxLenStream(Stream_t* st)
{
	int remain = st->Size - st->Position;
	byte* p = &st->MemStart[st->Position];

	return MrcMasterGetRxLength(p, remain);
}

/// <summary>获取应答消息内负载数据的偏移量</summary>
/// <param name="pkt">已经校验通过的数据包</param>
/// <returns>返回偏移量，-1 消息类型错误，0 无负载数据</returns>
int MrcMasterGetRxPyOffset(byte* pkt)
{
	if (pkt == NULL)return 0;

	byte cmd = pkt[1];
	switch (cmd)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	{
		return 3;
	}
	case 5:
	case 6:
	{
		return 4;
	}
	case 0x0f:
	case 0x10:
	{
		return 0;
	}
	default: return -1;
	}
}

// 01 02 请求指令。 len = 8
int Mrc01a02(byte addr, byte cmd, ushort regaddr, ushort bitlen, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < 8)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteByte(&st, addr);
	StreamWriteByte(&st, cmd);
	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(bitlen);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	temp = (crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 01 02 指令的回复。 len > (bitlen+7)/8 + 5;
int MrcResult01a02(byte addr, byte cmd, byte* bits, ushort bitlen, byte* data, int len)
{
	if (cmd < 1)return -1;
	if (cmd > 2)return -1;
	if (data == NULL)return -1;
	int bytelen = (bitlen + 7) / 8;
	if (len < bytelen + 5)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteByte(&st, addr);
	StreamWriteByte(&st, cmd);
	StreamWriteByte(&st, bitlen);

	StreamWriteBytes(&st, bits, bytelen);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	ushort temp = (crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 03 04 请求指令。 len = 8
int Mrc03a04(byte addr, byte cmd, ushort regaddr, ushort regcnt, byte* data, int len)
{
	return Mrc01a02(addr, cmd, regaddr, regcnt, data, len);
}

// 03 指令回复。 len > regcnt * 2 + 5
int MrcResult03a04(byte addr, byte cmd, byte* reg, ushort regcnt, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < regcnt * 2 + 5)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteByte(&st, addr);
	StreamWriteByte(&st, cmd);
	StreamWriteByte(&st, regcnt * 2);
	StreamWriteBytes(&st, reg, regcnt * 2);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	ushort temp = (crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 05 06 请求指令  len = 8
int Mrc05a06(byte addr, byte cmd, ushort regaddr, ushort reg, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < 8)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteByte(&st, addr);
	StreamWriteByte(&st, cmd);
	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(reg);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	temp = (crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

int Mrc0f(byte addr, ushort regaddr, byte* bitdata, ushort bitcnt, byte bytelen, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < bytelen + 9)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteByte(&st, addr);
	StreamWriteByte(&st, 0x10);
	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(bitcnt);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	StreamWriteByte(&st, bytelen);
	StreamWriteBytes(&st, bitdata, bytelen);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	temp = (crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 0f 指令回复
int MrcResult0f(byte addr, ushort regaddr, ushort bitcnt, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < 8)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteByte(&st, addr);
	StreamWriteByte(&st, 0x10);

	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(bitcnt);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	temp = (crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

int Mrc10(byte addr, ushort regaddr, byte* regdata, ushort regcnt, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < regcnt * 2 + 9)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteByte(&st, addr);
	StreamWriteByte(&st, 0x10);
	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(regcnt);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	StreamWriteByte(&st, regcnt * 2);
	StreamWriteBytes(&st, regdata, regcnt * 2);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	temp = (crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 10 指令回复
int MrcResult10(byte addr, ushort regaddr, ushort regcnt, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < 8)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteByte(&st, addr);
	StreamWriteByte(&st, 0x10);

	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(regcnt);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	temp = (crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}


