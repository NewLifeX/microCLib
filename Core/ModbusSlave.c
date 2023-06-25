
#include "ModbusSlave.h"
#include "Debug.h"

#include "ModbusRtuCodec.h"
#include "ModbusTcpCodec.h"

void ModbusRtuMsgProcess(const ModbusSlave_t* mrs, byte* p, int pklen, void* sendparam)
{
	if (p == NULL)return;
	if (pklen < 8)return;

	ShowArray("485 ", p, pklen, pklen);
	if (mrs == NULL)return;
	if (mrs->SlaveAddr == NULL)return;

	// 发送用的缓冲区
	byte tx[264];
	byte* txpy = tx + 8;

	// 解数据包
	byte addr = p[0];
	byte cmd = p[1];

	if (addr == (*mrs->SlaveAddr))
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
			Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
			ar.Addr = regaddr;
			ar.Size = bitcnt;
			ar.Buff = txpy;
			if (mrs->BitAreas != NULL) AreaRead(&ar, mrs->BitAreas, true);
			// 构建回复
			int txlen = MrcResult01a02(addr, cmd, txpy, bitcnt, tx, sizeof(tx));
			ShowArray("-> ", tx, txlen, txlen);
			// Rs485Send2(tx, txlen);
			if (mrs->Send != NULL) mrs->Send(tx, txlen, sendparam);
		}break;
		// 写输出线圈
		case 5:
		{
			ushort regaddr = (p[2] << 8) + p[3];
			ushort regdata = (p[4] << 8) + p[5];

			byte value = regdata > 0 ? 0x01 : 0x00;
			Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
			ar.Addr = regaddr;
			ar.Buff = &value;
			ar.Size = 1;
			if (mrs->BitAreas != NULL) AreaWrite(mrs->BitAreas, &ar, true);

			ShowArray("-> ", p, pklen, pklen);
			// Rs485Send2(p, pklen);
			if (mrs->Send != NULL) mrs->Send(p, pklen, sendparam);
		}break;
		// 写输出线圈S
		case 0x0f:
		{
			ushort regaddr = (p[2] << 8) + p[3];
			ushort bitcnt = (p[4] << 8) + p[5];

			Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
			ar.Addr = regaddr;
			ar.Buff = &p[7];
			ar.Size = bitcnt;
			if (mrs->BitAreas != NULL) AreaWrite(mrs->BitAreas, &ar, true);

			int txlen = MrcResult0f(addr, regaddr, bitcnt, tx, sizeof(tx));
			ShowArray("-> ", tx, txlen, txlen);
			// Rs485Send2(tx, txlen);
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
			Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
			// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
			ar.Addr = regaddr * 2;
			ar.Size = regcnt * 2;
			ar.Buff = txpy;
			if (mrs->ByteAreas != NULL) AreaRead(&ar, mrs->ByteAreas, false);

			// 构建回复
			int txlen = MrcResult03a04(addr, cmd, txpy, regcnt, tx, sizeof(tx));
			ShowArray("-> ", tx, txlen, txlen);
			// Rs485Send2(tx, txlen);
			if (mrs->Send != NULL) mrs->Send(tx, txlen, sendparam);
		}
		break;
		case 6:	// 写单个寄存器
		{
			ushort regaddr = (p[2] << 8) + p[3];
			ushort regdata = (p[4] << 8) + p[5];

			Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
			// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
			ar.Addr = regaddr * 2;
			ar.Size = 2;
			ar.Buff = (byte*)&regdata;
			if (mrs->ByteAreas != NULL) AreaWrite(mrs->ByteAreas, &ar, false);

			ShowArray("-> ", p, pklen, pklen);
			// Rs485Send2(p, pklen);
			if (mrs->Send != NULL) mrs->Send(p, pklen, sendparam);
		}break;
		case 0x10:
		{
			ushort regaddr = (p[2] << 8) + p[3];
			ushort regcnt = (p[4] << 8) + p[5];

			Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
			// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
			ar.Addr = regaddr * 2;
			ar.Size = regcnt * 2;
			ar.Buff = &p[7];
			if (mrs->ByteAreas != NULL) AreaWrite(mrs->ByteAreas, &ar, false);

			int txlen = MrcResult10(addr, regaddr, regcnt, tx, sizeof(tx));
			ShowArray("-> ", tx, txlen, txlen);
			// Rs485Send2(tx, txlen);
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
	if (pklen < 8 + 6)return;

	ShowArray("Rev ", pk, pklen, pklen);
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
		Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
		ar.Addr = regaddr;
		ar.Size = bitcnt;
		ar.Buff = txpy;
		if (mrs->BitAreas != NULL)AreaRead(&ar, mrs->BitAreas, true);

		int txlen = MtcResult01a02(head, cmd, txpy, bitcnt, tx, sizeof(tx));
		ShowArray("-> ", tx, txlen, txlen);
		// bigsend(mi->SocId, tx, txlen, 5);
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
		Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
		ar.Addr = regaddr;
		ar.Size = bitcnt;
		ar.Buff = txpy;
		if (mrs->BitAreas != NULL)AreaRead(&ar, mrs->BitAreas, true);

		int txlen = MtcResult01a02(head, cmd, txpy, bitcnt, tx, sizeof(tx));

		ShowArray("-> ", tx, txlen, txlen);
		// bigsend(mi->SocId, tx, txlen, 5);
		if (mrs->Send != NULL)mrs->Send(tx, txlen, sendparam);
	}break;
	// 写输出线圈
	case 5:
	{
		ushort regaddr = (p[2] << 8) + p[3];
		ushort regdata = (p[4] << 8) + p[5];

		byte value = regdata > 0 ? 0x01 : 0x00;
		Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
		ar.Addr = regaddr;
		ar.Buff = &value;
		ar.Size = 1;
		if (mrs->BitAreas != NULL)AreaWrite(mrs->BitAreas, &ar, true);

		ShowArray("-> ", pk, pklen, pklen);
		// bigsend(mi->SocId, pk, pklen, 5);
		if (mrs->Send != NULL)mrs->Send(pk, pklen, sendparam);
	}break;
	// 写输出线圈S
	case 0x0f:
	{
		ushort regaddr = (p[2] << 8) + p[3];
		ushort bitcnt = (p[4] << 8) + p[5];

		Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
		ar.Addr = regaddr;
		ar.Buff = &p[7];
		ar.Size = bitcnt;
		if (mrs->BitAreas != NULL)AreaWrite(mrs->BitAreas, &ar, true);

		int txlen = MtcResult0f(head, regaddr, bitcnt, tx, sizeof(tx));
		ShowArray("-> ", tx, txlen, txlen);
		// bigsend(mi->SocId, tx, txlen, 5);
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
		Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
		// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
		ar.Addr = regaddr * 2;
		ar.Size = regcnt * 2;
		ar.Buff = txpy;
		if (mrs->ByteAreas != NULL)AreaRead(&ar, mrs->ByteAreas, false);

		// 构建回复
		int txlen = MtcResult03a04(head, cmd, txpy, regcnt, tx, sizeof(tx));
		ShowArray("-> ", tx, txlen, txlen);
		// bigsend(mi->SocId, tx, txlen, 5);
		if (mrs->Send != NULL)mrs->Send(tx, txlen, sendparam);
	}
	break;
	case 6:	// 写单个寄存器
	{
		ushort regaddr = (p[2] << 8) + p[3];
		ushort regdata = (p[4] << 8) + p[5];

		Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
		// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
		ar.Addr = regaddr * 2;
		ar.Size = 2;
		ar.Buff = (byte*)&regdata;
		if (mrs->ByteAreas != NULL)AreaWrite(mrs->ByteAreas, &ar, false);

		ShowArray("-> ", pk, pklen, pklen);
		// bigsend(mi->SocId, pk, pklen, 5);
		if (mrs->Send != NULL)mrs->Send(pk, pklen, sendparam);
	}break;
	case 0x10:
	{
		ushort regaddr = (p[2] << 8) + p[3];
		ushort regcnt = (p[4] << 8) + p[5];

		Area_t ar;	ar.Init = NULL; ar.Fresh = NULL;
		// modbus 地址对应的是 2字节，地址需要 X2 对应字节空间。
		ar.Addr = regaddr * 2;
		ar.Size = regcnt * 2;
		ar.Buff = &p[7];
		if (mrs->ByteAreas != NULL)AreaWrite(mrs->ByteAreas, &ar, false);

		int txlen = MtcResult10(head, regaddr, regcnt, tx, sizeof(tx));
		ShowArray("-> ", tx, txlen, txlen);
		// bigsend(mi->SocId, tx, txlen, 5);
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
