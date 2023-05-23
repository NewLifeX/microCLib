
#include "Type.h"
#include "ModbusTcpCodec.h"
#include "LengthFieldCodec.h"


static const LengthFieldCodec_t Lfc =
{
	.offset = 4,
	.size = -2,
	.cachelen = 6,
};

int MtcGetLength(byte* p, int len)
{
	int res = LfcGetLength((LengthFieldCodec_t*)&Lfc, p, len);
	if (res > 0)return res + 6;
	return res;
}

int MtcGetLenCircularQueue(CircularQueue_t* queue)
{
	int res = LfcGetLenCircularQueue((LengthFieldCodec_t*)&Lfc, queue);
	if (res > 0)return res + 6;
	return res;
}

int MtcGetLenStream(Stream_t* st)
{
	int res = LfcGetLenStream((LengthFieldCodec_t*)&Lfc, st);
	if (res > 0)return res + 6;
	return res;
}

// 01 02 请求指令。 len = 12
int Mtc01a02(MtcHead_t* head, byte cmd, ushort regaddr, ushort bitlen, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < 12)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteBytes(&st, (byte*)head,sizeof(MtcHead_t));
	StreamWriteByte(&st, cmd);
	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(bitlen);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	LfcSetLength((LengthFieldCodec_t*) & Lfc, data, len, st.Position);

	return st.Position;
}

// 01 02 指令的回复。 len > (bitlen+7)/8 + 9;
int MtcResult01a02(MtcHead_t* head, byte cmd, byte* bits, ushort bitlen, byte* data, int len)
{
	if (cmd < 1)return -1;
	if (cmd > 2)return -1;
	if (data == NULL)return -1;
	int bytelen = (bitlen + 7) / 8;
	if (len < bytelen + 9)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteBytes(&st, (byte*)head, sizeof(MtcHead_t));

	StreamWriteByte(&st, cmd);
	StreamWriteByte(&st, bitlen);

	StreamWriteBytes(&st, bits, bytelen);

	LfcSetLength((LengthFieldCodec_t*)&Lfc, data, len, st.Position);

	return st.Position;
}

// 03 04 请求指令。 len = 12
int Mtc03a04(MtcHead_t* head, byte cmd, ushort regaddr, ushort regcnt, byte* data, int len)
{
	return Mtc01a02(head, cmd, regaddr, regcnt, data, len);
}

// 03 指令回复。 len > regcnt * 2 + 9
int MtcResult03a04(MtcHead_t* head, byte cmd, byte* reg, ushort regcnt, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < regcnt * 2 + 9)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteBytes(&st, (byte*)head, sizeof(MtcHead_t));
	StreamWriteByte(&st, cmd);
	StreamWriteByte(&st, regcnt * 2);
	StreamWriteBytes(&st, reg, regcnt * 2);

	LfcSetLength((LengthFieldCodec_t*)&Lfc, data, len, st.Position);

	return st.Position;
}

// 05 06 请求指令  len = 12
int Mtc05a06(MtcHead_t* head, byte cmd, ushort regaddr, ushort reg, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < 12)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteBytes(&st, (byte*)head, sizeof(MtcHead_t));
	StreamWriteByte(&st, cmd);
	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(reg);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	LfcSetLength((LengthFieldCodec_t*)&Lfc, data, len, st.Position);

	return st.Position;
}

int Mtc10(MtcHead_t* head, ushort regaddr, byte* regdata, ushort regcnt, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < regcnt * 2 + 13)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteBytes(&st, (byte*)head, sizeof(MtcHead_t));
	StreamWriteByte(&st, 0x10);
	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(regcnt);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	StreamWriteByte(&st, regcnt * 2);
	StreamWriteBytes(&st, regdata, regcnt * 2);

	LfcSetLength((LengthFieldCodec_t*)&Lfc, data, len, st.Position);

	return st.Position;
}

// 10 指令回复
int MtcResult10(MtcHead_t* head, ushort regaddr, ushort regcnt, byte* data, int len)
{
	if (data == NULL)return -1;
	if (len < 12)return -1;

	Stream_t st;
	StreamInit(&st, data, len);

	StreamWriteBytes(&st, (byte*)head, sizeof(MtcHead_t));
	StreamWriteByte(&st, 0x10);

	ushort temp = __REV16x(regaddr);
	StreamWriteBytes(&st, (byte*)&temp, 2);
	temp = __REV16x(regcnt);
	StreamWriteBytes(&st, (byte*)&temp, 2);

	LfcSetLength((LengthFieldCodec_t*)&Lfc, data, len, st.Position);

	return st.Position;
}

