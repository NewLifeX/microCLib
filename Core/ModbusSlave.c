
#include "ModbusSlave.h"
#include "Debug.h"

#include "ModbusRtuCodec.h"
#include "ModbusTcpCodec.h"

void ModbusRtuMsgProcess(const ModbusSlave_t* mrs, byte* p, int pklen, void* sendparam)
{
	if (p == NULL)return;
	if (pklen < 8)return;

	ShowArray("485 ", p, pklen, 16);
	if (mrs == NULL)return;
	if (mrs->SlaveAddr == NULL)return;

	// 发送用的缓冲区
	byte tx[264];
	byte* txpy = tx + 8;

	// 解数据包
	byte addr = p[0];
	byte cmd = p[1];

	if ((addr == (*mrs->SlaveAddr)) || (addr == 0xff))
	{
		switch (cmd)
		{
		case 1:	// 读输出线圈
		case 2: // 读输入
		{
			// 处理一下大端
			ushort	regaddr = (p[2] << 8) + p[3];
			ushort	bitcnt = (p[4] << 8) + p[5];

			// 清空缓冲区
			memset(txpy, 0x00, bitcnt / 8 + 1);
			// 读 bit 空间
			if (mrs->BitAreas != NULL) AreaRead2(mrs->BitAreas, regaddr, txpy, bitcnt, true);
			int txlen = MrcResult01a02(addr, cmd, txpy, bitcnt, tx, sizeof(tx));

			ShowArray("-> ", tx, txlen, 16);
			if (mrs->Send != NULL) mrs->Send(tx, txlen, sendparam);
		}break;
		// 写输出线圈
		case 5:
		{
			ushort regaddr = (p[2] << 8) + p[3];
			ushort regdata = (p[4] << 8) + p[5];
			byte value = regdata > 0 ? 0x01 : 0x00;

			if (mrs->BitAreas != NULL) AreaWrite2(mrs->BitAreas, regaddr, &value, 1, true);

			ShowArray("-> ", p, pklen, 16);
			if (mrs->Send != NULL) mrs->Send(p, pklen, sendparam);
		}break;
		// 写输出线圈S
		case 0x0f:
		{
			ushort regaddr = (p[2] << 8) + p[3];
			ushort bitcnt = (p[4] << 8) + p[5];

			if (mrs->BitAreas != NULL) AreaWrite2(mrs->BitAreas, regaddr, &p[7], bitcnt, true);
			int txlen = MrcResult0f(addr, regaddr, bitcnt, tx, sizeof(tx));

			ShowArray("-> ", tx, txlen, 16);
			if (mrs->Send != NULL) mrs->Send(tx, txlen, sendparam);
		}break;
		case 3:	// 读多个保持寄存器
		case 4:	// 读输入寄存器
		{
			// 处理一下大端
			ushort	regaddr = (p[2] << 8) + p[3];
			ushort	regcnt = (p[4] << 8) + p[5];

			// 清空缓冲区
			memset(txpy, 0x00, regcnt * 2);
			// 读 byte 空间
			// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
			if (mrs->ByteAreas != NULL) AreaRead2(mrs->ByteAreas, regaddr * 2, txpy, regcnt * 2, false);
			int txlen = MrcResult03a04(addr, cmd, txpy, regcnt, tx, sizeof(tx));

			ShowArray("-> ", tx, txlen, 16);
			if (mrs->Send != NULL) mrs->Send(tx, txlen, sendparam);
		}
		break;
		case 6:	// 写单个寄存器
		{
			ushort regaddr = (p[2] << 8) + p[3];
			ushort regdata = (p[4] << 8) + p[5];

			// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
			if (mrs->ByteAreas != NULL) AreaWrite2(mrs->ByteAreas, regaddr * 2, (byte*)&regdata, 2, false);

			ShowArray("-> ", p, pklen, 16);
			if (mrs->Send != NULL) mrs->Send(p, pklen, sendparam);
		}break;
		case 0x10:
		{
			ushort regaddr = (p[2] << 8) + p[3];
			ushort regcnt = (p[4] << 8) + p[5];

			// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
			if (mrs->ByteAreas != NULL) AreaWrite2(mrs->ByteAreas, regaddr * 2, &p[7], regcnt * 2, false);
			int txlen = MrcResult10(addr, regaddr, regcnt, tx, sizeof(tx));

			ShowArray("-> ", tx, txlen, 16);
			if (mrs->Send != NULL) mrs->Send(tx, txlen, sendparam);
		}break;

		default:DebugPrintf("CMD 0x%02X\r\n", cmd); break;
		}
	}
}

void ModbusRtuTryProcess(CircularQueue_t* queue, const ModbusSlave_t* mrs, void* sendparam)
{
	if (queue == NULL)return;

	// 数据包长度
	int pklen = MrcSlaveGetLenCircularQueue(queue);
	// 不够长
	if (pklen == 0)return;
	// 有问题的包
	if (pklen < 0) { CircularQueueSeek(queue, 1); return; }

	// 读数据
	byte* p = (byte*)GlobleMalloc(pklen);
	CircularQueueReads(queue, p, pklen, false);

	// 处理数据
	ModbusRtuMsgProcess(mrs, p, pklen, sendparam);

	// free
	GlobleFree(p);
}

void ModbusTcpMsgProcess(const ModbusSlave_t* mrs, byte* pk, int pklen, void* sendparam)
{
	if (pk == NULL)return;
	if (pklen < 6 + 6)return;

	ShowArray("Rev ", pk, pklen, 16);
	if (mrs == NULL)return;

	byte* p = pk + 6;

	// TCP 不需要 byte addr = p[0];
	// 但是要 head，原样返回的 head
	MtcHead_t* head = (MtcHead_t*)pk;
	byte cmd = p[1];

	// 发送用的缓冲区
	byte tx[272];
	byte* txpy = tx + 16;
	// int txpylen = sizeof(tx) - 16;

	switch (cmd)
	{
		// 读输入
	case 2:
	{
		ushort	regaddr = (p[2] << 8) + p[3];
		ushort	bitcnt = (p[4] << 8) + p[5];

		// 清空缓冲区
		memset(txpy, 0x00, bitcnt / 8 + 1);
		// 读 bit 空间
		if (mrs->BitAreas != NULL)AreaRead2(mrs->BitAreas, regaddr, txpy, bitcnt, true);
		int txlen = MtcResult01a02(head, cmd, txpy, bitcnt, tx, sizeof(tx));

		ShowArray("-> ", tx, txlen, 16);
		if (mrs->Send != NULL)mrs->Send(tx, txlen, sendparam);
	}break;
	// 读输出线圈
	case 1:
	{
		ushort	regaddr = (p[2] << 8) + p[3];
		ushort	bitcnt = (p[4] << 8) + p[5];

		// 清空缓冲区
		memset(txpy, 0x00, bitcnt / 8 + 1);
		// 读 bit 空间
		if (mrs->BitAreas != NULL)AreaRead2(mrs->BitAreas, regaddr, txpy, bitcnt, true);
		int txlen = MtcResult01a02(head, cmd, txpy, bitcnt, tx, sizeof(tx));

		ShowArray("-> ", tx, txlen, 16);
		if (mrs->Send != NULL)mrs->Send(tx, txlen, sendparam);
	}break;
	// 写输出线圈
	case 5:
	{
		ushort regaddr = (p[2] << 8) + p[3];
		ushort regdata = (p[4] << 8) + p[5];

		byte value = regdata > 0 ? 0x01 : 0x00;
		if (mrs->BitAreas != NULL)AreaWrite2(mrs->BitAreas, regaddr, &value, 1, true);

		ShowArray("-> ", pk, pklen, 16);
		if (mrs->Send != NULL)mrs->Send(pk, pklen, sendparam);
	}break;
	// 写输出线圈S
	case 0x0f:
	{
		ushort regaddr = (p[2] << 8) + p[3];
		ushort bitcnt = (p[4] << 8) + p[5];

		if (mrs->BitAreas != NULL)AreaWrite2(mrs->BitAreas, regaddr, &p[7], bitcnt, true);
		int txlen = MtcResult0f(head, regaddr, bitcnt, tx, sizeof(tx));

		ShowArray("-> ", tx, txlen, 16);
		if (mrs->Send != NULL)mrs->Send(tx, txlen, sendparam);
	}break;


	case 3:	// 读多个保持寄存器
	case 4:	// 读输入寄存器
	{
		// 处理一下大端
		ushort	regaddr = (p[2] << 8) + p[3];
		ushort	regcnt = (p[4] << 8) + p[5];

		// 清空缓冲区
		memset(txpy, 0x00, regcnt * 2);
		// 读 byte 空间
		// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
		if (mrs->ByteAreas != NULL)AreaRead2(mrs->ByteAreas, regaddr * 2, txpy, regcnt * 2, false);
		int txlen = MtcResult03a04(head, cmd, txpy, regcnt, tx, sizeof(tx));

		ShowArray("-> ", tx, txlen, 16);
		if (mrs->Send != NULL)mrs->Send(tx, txlen, sendparam);
	}
	break;
	case 6:	// 写单个寄存器
	{
		ushort regaddr = (p[2] << 8) + p[3];
		ushort regdata = (p[4] << 8) + p[5];

		// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
		if (mrs->ByteAreas != NULL)AreaWrite2(mrs->ByteAreas, regaddr * 2, (byte*)&regdata, 2, false);

		ShowArray("-> ", pk, pklen, 16);
		if (mrs->Send != NULL)mrs->Send(pk, pklen, sendparam);
	}break;
	case 0x10:
	{
		ushort regaddr = (p[2] << 8) + p[3];
		ushort regcnt = (p[4] << 8) + p[5];

		// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
		if (mrs->ByteAreas != NULL)AreaWrite2(mrs->ByteAreas, regaddr * 2, &p[7], regcnt * 2, false);
		int txlen = MtcResult10(head, regaddr, regcnt, tx, sizeof(tx));

		ShowArray("-> ", tx, txlen, 16);
		if (mrs->Send != NULL)mrs->Send(tx, txlen, sendparam);
	}break;

	default:DebugPrintf("CMD 0x%02X\r\n", cmd); break;
	}
}

void ModbusTcpTryProcess(CircularQueue_t* queue, const ModbusSlave_t* mrs, void* sendparam)
{
	if (queue == NULL)return;

	int pklen = MtcGetLenCircularQueue(queue);
	// 不够长
	if (pklen == 0)return;
	// 有问题的包
	if (pklen < 0) { CircularQueueSeek(queue, 1); return; }

	// 读数据
	byte* pk = (byte*)GlobleMalloc(pklen);
	CircularQueueReads(queue, pk, pklen, false);

	// 处理数据
	ModbusTcpMsgProcess(mrs, pk, pklen, sendparam);

	// free
	GlobleFree(pk);
}

void ModbusUdpTryProcess(Stream_t* ms, const ModbusSlave_t* mrs, void* sendparam)
{
	// 因为 UDP 不存在数据包粘连问题，而且是一次性到达。
	// 没有使用 CircularQueue_t 的情况。
	// 使用 Stream_t  或者  Buffer 都可以。
	// UDP 数据包与 TCP 完全相同。
	if (ms == NULL)return;
	// if (StreamRemian(ms) < 12)return;
	int pklen = MtcGetLenStream(ms);
	// 不够长 或者 数据包有问题。
	if (pklen <= 0)return;

	// 处理数据
	ModbusTcpMsgProcess(mrs, ms->MemStart + ms->Position, pklen, sendparam);
}
