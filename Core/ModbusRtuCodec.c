
#include "ModbusRtuCodec.h"
#include "Crc.h"

static bool Check(byte* data, int len)
{
	if (data == NULL)return false;

	ushort crc = CaclcCRC16_MODBUS(data, len - 2);
	ushort pktcrc = data[len - 2] * 256 + data[len - 1];

	return crc == pktcrc;
}

int MrcSlaveGetLength(byte* p, int len)
{
	if (p == NULL)return 0;
	if (len < 8)return 0;

	byte cmd = p[1];
	switch (cmd)
	{
		// ��ָ��ȶ��� 8 �ֽڡ�
		// д�����Ĵ�������Ҳ�� 8�ֽڡ�
	case 1: // ����Ȧ   
	case 2:	// ����ɢ����  
	case 3: // ��������ּĴ��� 
	case 4: // ������Ĵ��� 
	case 5: // �������Ĵ��� 
	case 6: // д�Ĵ��� 
	{
		if (Check(p, 8))return 8;
		return -1;
	}

	case 0x10:
	{
		// addr,cmd,regaddr2+regcnt2+len+ dataXlen +crc
		ushort bytelen = p[6];
		ushort regcnt = p[4] * 256 + p[5];

		if (regcnt != bytelen / 2)return -2;
		if (len == 0)return -2;
		if (bytelen > 122)return -2;
		if (len < bytelen)return 0;
		// �̶������� 9 �ֽ�
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
	int remian = CircularQueueRemain(queue);
	if (remian < 8)return 0;

	// �õ� cmd
	byte cache[8];
	CircularQueueReads(queue, cache, sizeof(cache), true);
	byte cmd = cache[1];

	switch (cmd)
	{
		// ��ָ��ȶ��� 8 �ֽڡ�
		// д�����Ĵ�������Ҳ�� 8�ֽڡ�
	case 1: // ����Ȧ   1bit
	case 2:	// ����ɢ����  1bit
	case 3: // �����ּĴ��� 16bit
	case 4: // ������Ĵ��� 16bit
	case 6: // д�Ĵ���  16bit
	{
		if (Check(cache, 8))return 8;
		return -1;
	}

	case 0x10:
	{
		/*
		// ��Ϊ CircularQueue_t �Ƕ�д����ģ����Է�ֹ���޸ģ���Ҫcopy������
		CircularQueue_t queue2;
		CircularQueue_t* q = &queue2;
		memcpy(q, queue, sizeof(CircularQueue_t));
		// ��Ҫ�������������� tail ָ��λ�á�
		byte* tail = q->pTail;
		*/

		// addr,cmd,regaddr2+regcnt2+len+ dataXlen +crc
		ushort len = cache[6];
		ushort regcnt = cache[4] * 256 + cache[5];

		if (regcnt != len / 2)return -2;
		if (len == 0)return -2;
		if (len > 122)return -2;
		if (remian < len)return 0;

		ushort pglen = len + 9;
		// 10 ���ݰ�������� 130 �ֽڡ�
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

// 01 02 ����ָ� len = 8
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
	temp = __REV16x(crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 01 02 ָ��Ļظ��� len > (bitlen+7)/8 + 5;
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
	ushort temp = __REV16x(crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 03 04 ����ָ� len = 8
int Mrc03a04(byte addr, byte cmd, ushort regaddr, ushort regcnt, byte* data, int len)
{
	return Mrc01a02(addr, cmd, regaddr, regcnt, data, len);
}

// 03 ָ��ظ��� len > regcnt * 2 + 5
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
	ushort temp = __REV16x(crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 05 06 ����ָ��  len = 8
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
	temp = __REV16x(crc);
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
	StreamWriteByte(&st, regcnt*2);
	StreamWriteBytes(&st, regdata, regcnt * 2);

	ushort crc = CaclcCRC16_MODBUS(st.MemStart, st.Position);
	temp = __REV16x(crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}

// 10 ָ��ظ�
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
	temp = __REV16x(crc);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	return st.Position;
}
