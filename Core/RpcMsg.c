#include "RpcMsg.h"
#include "Stream.h"
// #include "Array.h"
#include "Debug.h"

// StreamCurrPoint() 更安全，但是速度慢不少。本文件参数判断很多，这里直接取指针就好。
#define StCurPoint(st)  (&st->MemStart[st->Position])


/// <summary>NewLife SRMP 数据包获取长度</summary>
/// <param name="data">缓冲区</param>
/// <param name="len">缓冲区长度</param>
/// <returns>返回数据包长度；小于0，读取失败</returns>
int SrmpTryGetLen(byte* data, int len)
{
	int remain = len;
	if (remain < sizeof(SrmpHead_t))return -1;

	// 标准包
	SrmpHead_t* head = (SrmpHead_t*)data;
	int pklen = 0;

	if (head->Length != 0xffff)
	{
		pklen = head->Length + sizeof(SrmpHead_t);
	}
	else
	{
		// 扩展 Head.LengthExt
		if (remain < sizeof(SrmpHead_t) + 4)return -1;

		uint lenExt = 0;
		memcpy(&lenExt, ((byte*)head) + 4, 4);

		pklen = lenExt + sizeof(SrmpHead_t) + 4;
	}

	if (remain < pklen)return -1;
	return pklen;
}

/// <summary>NewLife SRMP 数据包获取长度</summary>
/// <param name="st">流</param>
/// <returns></returns>
int SrmpTryGetLen_Stream(Stream_t* st)
{
	return SrmpTryGetLen(StCurPoint(st), StreamRemian(st));
}

/// <summary>NewLife SRMP 数据包获取长度</summary>
/// <param name="queue">环形队列</param>
/// <returns>返回数据包长度；小于0，读取失败</returns>
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





// Head.Flag 低 6bit 指定值。
#define HEAD_FLAG_0_5  0x00

// 超大数据包，大于 65535 字节的数据包。启用 Head.LengthExt
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

	// action.length（7位压缩编码）+ action
	StreamWriteCompressionUint(st, act_len);

	rpc->Action = (char*)StCurPoint(st);
	rpc->ActionLen = act_len;
	if (StreamWriteBytes(st, (byte*)action, act_len) != act_len)
	{
		DebugPrintf("ApiPacket %s datalen less\r\n", action);
		return -2;
	}

	// 错误码处理
	if ((errCode != 0) && replay)
	{
		rpc->Head->Flag |= 0x40;
		StreamWriteUint(st, (uint)errCode);
	}

	// 检查一波长度，避免后续问题。
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

/// <summary>封装 RPC 通讯方式</summary>
/// <param name="data">内存缓冲</param>
/// <param name="datalen">内存大小</param>
/// <param name="action">RPC 名称</param>
/// <param name="rpc">RPC 对象</param>
/// <returns>返回负载允许的最大长度</returns>
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

/// <summary>封装 RPC 返回消息构建</summary>
/// <param name="data">内存缓冲</param>
/// <param name="datalen">内存大小</param>
/// <param name="action">RPC 名称</param>
/// <param name="errCode">错误码，0无错</param>
/// <param name="rpc">RPC 对象</param>
/// <returns>返回负载允许的最大长度</returns>
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

/// <summary>创建回复消息</summary>
/// <param name="data">缓冲区</param>
/// <param name="datalen">缓冲区大小</param>
/// <param name="errCode">错误码，0无错</param>
/// <param name="src">原有消息</param>
/// <param name="dst">创建的新消息</param>
/// <returns>负载最大尺寸</returns>
int RpcCreatReply(byte* data, int datalen, uint errCode, RpcInfo_t* src, RpcInfo_t* dst)
{
	if (data == NULL)return -1;
	if (src == NULL)return -1;
	if (src->Head == NULL)return -1;
	if (src->Action == NULL)return -1;
#if BIG_PKT
	if (datalen < 9 + 4)return -1;
	// （actionlen=0）最小数据包 9 + 4 字节。
	if (src->ActionLen + 9 + 4 > datalen)return -1;
#else
	if (datalen < 9)return -1;
	// （actionlen=0）最小数据包 9字节。
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

/// <summary>修正/完善 RPC 数据包结构</summary>
/// <param name="rpc">RPC 对象</param>
/// <returns>返回整条消息长度</returns>
int RpcFix(RpcInfo_t* rpc)
{
	if (rpc == NULL)return -1;
	if (rpc->Head == NULL)return -1;
	if (rpc->Payload == NULL)return -1;

	// 填充【负载长度】,在负载前面4字节
	Stream_t pkst;
	Stream_t* st = &pkst;
	StreamInit(st, rpc->Payload - 4, 4);
	StreamWriteUint(st, rpc->PayloadLen);

	// （内存尾-内存头） - 数据包头部长度
	SrmpHead_t* head = rpc->Head;
	uint hLen = (uint)rpc->Payload + rpc->PayloadLen - (uint)head - sizeof(SrmpHead_t);

	// 正常数据包
	if (hLen < 65535)
	{
		head->Length = hLen;
		return hLen + sizeof(SrmpHead_t);
	}

#if BIG_PKT
	// 超长数据包。 前面空余的4字节，派上用场。
	// 判断 第一次调用本函数
	if ((uint)rpc->Action - (uint)rpc->Head - sizeof(SrmpHead_t) < 4)
	{
		byte* p = (byte*)rpc->Head;
		SrmpHead_t* head = (SrmpHead_t*)(p - 4);
		// 从后面往前 copy 不担心覆盖问题，直接上。
		memcpy(head, rpc->Head, sizeof(SrmpHead_t));
		// 移形换位。
		rpc->Head = head;
	}

	// 长度处理
	head->Length = 0xffff;
	StreamInit(st, (byte*)rpc->Head, 8);
	StreamSeek(st, sizeof(SrmpHead_t), SeekCurrent);
	StreamWriteUint(st, hLen);

	return hLen + sizeof(SrmpHead_t) + 4;
#else
	// 不支持超大数据包
	return 0;
#endif
}

/// <summary>修正/完善 RPC 数据包结构</summary>
/// <param name="rpc">RPC 对象</param>
/// <param name="seq">消息SEQ编号</param>
/// <returns>返回整条消息长度</returns>
int RpcFixWithSeq(RpcInfo_t* rpc, byte seq)
{
	if (rpc == NULL)return 0;
	if (rpc->Head == NULL)return 0;
	rpc->Head->Seq = seq;

	return RpcFix(rpc);
}

/// <summary>设置数据Seq值</summary>
/// <param name="data">数据包</param>
/// <param name="seq">Seq</param>
void RpcPktSetSeq(byte* data, byte seq)
{
	if (data == NULL)return;

	SrmpHead_t* head = (SrmpHead_t*)data;
	head->Seq = seq;
}

/// <summary>解包RPC数据</summary>
/// <param name="data">数据</param>
/// <param name="len">数据大小</param>
/// <param name="rpc">rpc解出位置</param>
/// <returns>小于0表示出错</returns>
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

	// FLAG 标志位
	if ((head->Flag & 0x80) != 0)rpc->Reply = true;
	bool err = (head->Flag & 0x40) != 0;

	Stream_t pkst;
	Stream_t* st = &pkst;
	StreamInit(st, data, len);
	StreamSeek(st, sizeof(SrmpHead_t), SeekBegin);
	// 大于 64K 的包。长度在 Head.LengthExt 。
	if (head->Length == 0xffff)
	{
		uint realLen;
		if (StreamReadUint(st, &realLen) != 4)return -2;
		if (realLen + sizeof(SrmpHead_t) > len)return -2;
	}

	rpc->Head = head;

	// action.length（7位压缩编码）+ action
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

/// <summary>打印RPC数据</summary>
void RpcShow(RpcInfo_t* rpc)
{
	if (rpc == NULL)return;
	
	DebugPrintf("F %02X S %02X [%d]",
		rpc->Head->Flag,rpc->Head->Seq,rpc->Head->Length);

	// 备份字符结尾，修改为 \0 , 还原。 
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

