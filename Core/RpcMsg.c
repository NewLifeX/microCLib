#include "RpcMsg.h"
#include "Stream.h"
// #include "Array.h"
#include "Debug.h"

// StreamCurrPoint() ����ȫ�������ٶ������١����ļ������жϺܶ࣬����ֱ��ȡָ��ͺá�
#define StCurPoint(st)  (&st->MemStart[st->Position])


/// <summary>NewLife SRMP ���ݰ���ȡ����</summary>
/// <param name="data">������</param>
/// <param name="len">����������</param>
/// <returns>�������ݰ����ȣ�С��0����ȡʧ��</returns>
int SrmpTryGetLen(byte* data, int len)
{
	int remain = len;
	if (remain < sizeof(SrmpHead_t))return -1;

	// ��׼��
	SrmpHead_t* head = (SrmpHead_t*)data;
	int pklen = 0;

	if (head->Length != 0xffff)
	{
		pklen = head->Length + sizeof(SrmpHead_t);
	}
	else
	{
		// ��չ Head.LengthExt
		if (remain < sizeof(SrmpHead_t) + 4)return -1;

		uint lenExt = 0;
		memcpy(&lenExt, ((byte*)head) + 4, 4);

		pklen = lenExt + sizeof(SrmpHead_t) + 4;
	}

	if (remain < pklen)return -1;
	return pklen;
}

/// <summary>NewLife SRMP ���ݰ���ȡ����</summary>
/// <param name="st">��</param>
/// <returns></returns>
int SrmpTryGetLen_Stream(Stream_t* st)
{
	return SrmpTryGetLen(StCurPoint(st), StreamRemian(st));
}

/// <summary>NewLife SRMP ���ݰ���ȡ����</summary>
/// <param name="queue">���ζ���</param>
/// <returns>�������ݰ����ȣ�С��0����ȡʧ��</returns>
int SrmpTryGetLen_CircularQueue(CircularQueue_t* queue)
{
	byte cache[8];
	SrmpHead_t* head = (SrmpHead_t*)cache;
	int read = CircularQueueReads(queue, cache, sizeof(SrmpHead_t) + 4, true);
	if (read < sizeof(SrmpHead_t))return -1;

	int pklen = 0;
	if (head->Length != 0xffff)
	{
		pklen = head->Length + sizeof(SrmpHead_t);
	}
	else
	{
		if (read < sizeof(SrmpHead_t) + 4)return -1;

		uint lenExt = 0;
		memcpy(&lenExt, ((byte*)head) + 4, 4);

		pklen = lenExt + sizeof(SrmpHead_t) + 4;
	}

	if (CircularQueueGetLength(queue) < pklen)return -1;
	return pklen;
}





// Head.Flag �� 6bit ָ��ֵ��
#define HEAD_FLAG_0_5  0x00

// �������ݰ������� 65535 �ֽڵ����ݰ������� Head.LengthExt
#define BIG_PKT 0


static int _RpcInit(byte* data, int datalen,
	char* action, int act_len,
	bool replay, int errCode,
	RpcInfo_t* rpc)
{
	if (datalen < 9)return -1;
	memset(data, 0x00, sizeof(SrmpHead_t));

	rpc->Head = (SrmpHead_t*)data;
	rpc->Reply = replay;
	rpc->ErrCode = errCode;
	rpc->Head->Flag = HEAD_FLAG_0_5;
	if (replay) rpc->Head->Flag = 0x80 | HEAD_FLAG_0_5;
	// rpc->Head->Seq = 0;

	Stream_t pkst;
	Stream_t* st = &pkst;
	StreamInit(st, data, datalen);
	StreamSeek(st, sizeof(SrmpHead_t), SeekBegin);

	// action.length��7λѹ�����룩+ action
	StreamWriteCompressionUint(st, act_len);

	rpc->Action = (char*)StCurPoint(st);
	rpc->ActionLen = act_len;
	if (StreamWriteBytes(st, (byte*)action, act_len) != act_len)
	{
		DebugPrintf("ApiPacket %s datalen less\r\n", action);
		return -2;
	}

	// �����봦��
	if ((errCode != 0) && replay)
	{
		rpc->Head->Flag |= 0x40;
		StreamWriteUint(st, (uint)errCode);
	}

	// ���һ�����ȣ�����������⡣
	if (StreamRemian(st) < 4)
	{
		rpc->Payload = NULL;
		rpc->PayloadLen = 0;

		return -3;
	}

	// payload.length(int) + payload
	StreamSeek(st, 4, SeekCurrent);

	rpc->Payload = StCurPoint(st);
	rpc->PayloadLen = 0;

	return StreamRemian(st);
}

/// <summary>��װ RPC ͨѶ��ʽ</summary>
/// <param name="data">�ڴ滺��</param>
/// <param name="datalen">�ڴ��С</param>
/// <param name="action">RPC ����</param>
/// <param name="rpc">RPC ����</param>
/// <returns>���ظ����������󳤶�</returns>
int RpcInit(byte* data, int datalen, char* action, RpcInfo_t* rpc)
{
	if (data == NULL)return -1;
	if (action == NULL)return -1;

	int slen = strlen(action);
#if BIG_PKT
	return _RpcInit(data + 4, datalen - 4, action, slen, false, 0, rpc);
#else
	return _RpcInit(data, datalen, action, slen, false, 0, rpc);
#endif
}

/// <summary>��װ RPC ������Ϣ����</summary>
/// <param name="data">�ڴ滺��</param>
/// <param name="datalen">�ڴ��С</param>
/// <param name="action">RPC ����</param>
/// <param name="errCode">�����룬0�޴�</param>
/// <param name="rpc">RPC ����</param>
/// <returns>���ظ����������󳤶�</returns>
int RpcReplyInit(byte* data, int datalen, char* action, uint errCode, RpcInfo_t* rpc)
{
	if (data == NULL)return -1;
	if (action == NULL)return -1;

	int slen = strlen(action);
#if BIG_PKT
	return _RpcInit(data + 4, datalen - 4, action, slen, true, errCode, rpc);
#else
	return _RpcInit(data, datalen, action, slen, true, errCode, rpc);
#endif
}

/// <summary>�����ظ���Ϣ</summary>
/// <param name="data">������</param>
/// <param name="datalen">��������С</param>
/// <param name="errCode">�����룬0�޴�</param>
/// <param name="src">ԭ����Ϣ</param>
/// <param name="dst">����������Ϣ</param>
/// <returns>�������ߴ�</returns>
int RpcCreatReply(byte* data, int datalen, uint errCode, RpcInfo_t* src, RpcInfo_t* dst)
{
	if (data == NULL)return -1;
	if (src == NULL)return -1;
	if (src->Head == NULL)return -1;
	if (src->Action == NULL)return -1;
#if BIG_PKT
	if (datalen < 9 + 4)return -1;
	// ��actionlen=0����С���ݰ� 9 + 4 �ֽڡ�
	if (src->ActionLen + 9 + 4 > datalen)return -1;
#else
	if (datalen < 9)return -1;
	// ��actionlen=0����С���ݰ� 9�ֽڡ�
	if (src->ActionLen + 9 > datalen)return -1;
#endif

	byte Flag = src->Head->Flag & 0x3f;
	byte Seq = src->Head->Seq;

	int pyMax = _RpcInit(data, datalen, src->Action, src->ActionLen, true, errCode, dst);
	dst->Head->Flag &= 0xc0;
	dst->Head->Flag |= Flag;
	dst->Head->Seq = Seq;

	return pyMax;
}

/// <summary>����/���� RPC ���ݰ��ṹ</summary>
/// <param name="rpc">RPC ����</param>
/// <returns>����������Ϣ����</returns>
int RpcFix(RpcInfo_t* rpc)
{
	if (rpc == NULL)return -1;
	if (rpc->Head == NULL)return -1;
	if (rpc->Payload == NULL)return -1;

	// ��䡾���س��ȡ�,�ڸ���ǰ��4�ֽ�
	Stream_t pkst;
	Stream_t* st = &pkst;
	StreamInit(st, rpc->Payload - 4, 4);
	StreamWriteUint(st, rpc->PayloadLen);

	// ���ڴ�β-�ڴ�ͷ�� - ���ݰ�ͷ������
	SrmpHead_t* head = rpc->Head;
	uint hLen = (uint)rpc->Payload + rpc->PayloadLen - (uint)head - sizeof(SrmpHead_t);

	// �������ݰ�
	if (hLen < 65535)
	{
		head->Length = hLen;
		return hLen + sizeof(SrmpHead_t);
	}

#if BIG_PKT
	// �������ݰ��� ǰ������4�ֽڣ������ó���
	// �ж� ��һ�ε��ñ�����
	if ((uint)rpc->Action - (uint)rpc->Head - sizeof(SrmpHead_t) < 4)
	{
		byte* p = (byte*)rpc->Head;
		SrmpHead_t* head = (SrmpHead_t*)(p - 4);
		// �Ӻ�����ǰ copy �����ĸ������⣬ֱ���ϡ�
		memcpy(head, rpc->Head, sizeof(SrmpHead_t));
		// ���λ�λ��
		rpc->Head = head;
	}

	// ���ȴ���
	head->Length = 0xffff;
	StreamInit(st, (byte*)rpc->Head, 8);
	StreamSeek(st, sizeof(SrmpHead_t), SeekCurrent);
	StreamWriteUint(st, hLen);

	return hLen + sizeof(SrmpHead_t) + 4;
#else
	// ��֧�ֳ������ݰ�
	return 0;
#endif
}

/// <summary>����/���� RPC ���ݰ��ṹ</summary>
/// <param name="rpc">RPC ����</param>
/// <param name="seq">��ϢSEQ���</param>
/// <returns>����������Ϣ����</returns>
int RpcFixWithSeq(RpcInfo_t* rpc, byte seq)
{
	if (rpc == NULL)return 0;
	if (rpc->Head == NULL)return 0;
	rpc->Head->Seq = seq;

	return RpcFix(rpc);
}

/// <summary>��������Seqֵ</summary>
/// <param name="data">���ݰ�</param>
/// <param name="seq">Seq</param>
void RpcPktSetSeq(byte* data, byte seq)
{
	if (data == NULL)return;

	SrmpHead_t* head = (SrmpHead_t*)data;
	head->Seq = seq;
}

/// <summary>���RPC����</summary>
/// <param name="data">����</param>
/// <param name="len">���ݴ�С</param>
/// <param name="rpc">rpc���λ��</param>
/// <returns>С��0��ʾ����</returns>
int RpcPktParse(byte* data, int len, RpcInfo_t* rpc)
{
	if (data == NULL)return -1;
	if (len < 9)return -1;
	if (rpc == NULL)return -1;
	memset(rpc, 0x00, sizeof(RpcInfo_t));

	// HEAD
	SrmpHead_t* head = (SrmpHead_t*)data;
	if (head->Length < 65535)
	{
		if (head->Length + sizeof(SrmpHead_t) > len)return -2;
	}

	// FLAG ��־λ
	if ((head->Flag & 0x80) != 0)rpc->Reply = true;
	bool err = (head->Flag & 0x40) != 0;

	Stream_t pkst;
	Stream_t* st = &pkst;
	StreamInit(st, data, len);
	StreamSeek(st, sizeof(SrmpHead_t), SeekBegin);
	// ���� 64K �İ��������� Head.LengthExt ��
	if (head->Length == 0xffff)
	{
		uint realLen;
		if (StreamReadUint(st, &realLen) != 4)return -2;
		if (realLen + sizeof(SrmpHead_t) > len)return -2;
	}

	rpc->Head = head;

	// action.length��7λѹ�����룩+ action
	uint slen = 0;
	StreamReadCompressionUint(st, &slen);
	rpc->Action = (char*)StCurPoint(st);
	rpc->ActionLen = slen;
	StreamSeek(st, slen, SeekCurrent);
	if (StreamRemian(st) == 0)return 0;

	// errCode
	if (err)
	{
		StreamReadUint(st, (uint*)&rpc->ErrCode);
		if (StreamRemian(st) == 0)return 0;
	}

	// payload.length(int) + payload
	StreamReadUint(st, (uint*)&rpc->PayloadLen);
	rpc->Payload = StCurPoint(st);
	// if (StreamRemian(st) < rpc->PayloadLen)return -5;

	return 0;
}

/// <summary>��ӡRPC����</summary>
void RpcShow(RpcInfo_t* rpc)
{
	if (rpc == NULL)return;
	
	DebugPrintf("F %02X S %02X [%d]",
		rpc->Head->Flag,rpc->Head->Seq,rpc->Head->Length);

	// �����ַ���β���޸�Ϊ \0 , ��ԭ�� 
	byte act_e = rpc->Action[rpc->ActionLen];
	
	DebugPrintf(" ACT ");
	if (rpc->Action != NULL)
	{
		rpc->Action[rpc->ActionLen] = '\0';
		DebugPrintf("%s\r\n", rpc->Action);
		rpc->Action[rpc->ActionLen] = act_e;
	}
	else
	{
		DebugPrintf(" --\r\n");
	}

	if (rpc->Payload != NULL)
	{
		byte pay_e = rpc->Payload[rpc->PayloadLen];
		rpc->Payload[rpc->PayloadLen] = 0x00;
		if (rpc->ErrCode != 0)
			DebugPrintf(" ERR %d : %s\r\n", rpc->ErrCode, rpc->Payload);
		else
			DebugPrintf(" %s\r\n", rpc->Payload);
		rpc->Payload[rpc->PayloadLen] = pay_e;
	}
	else
	{
		DebugPrintf(" --\r\n");
	}
}

