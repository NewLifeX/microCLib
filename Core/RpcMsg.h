#pragma once

#include "Type.h"

/*
* 下位机不会超过 65535 的负载，所以不支持超长数据包。
*/

#pragma pack(push)
#pragma pack(1)

// NewLife SRMP 标准数据包头
typedef struct
{
	// Flag 为位标志
	// bit7  0x80  Reply
	// bit6  0x40  Error
	// bit 0-5	   数据包格式，默认使用0 
	// 其他未定义
	byte Flag;
	// 数据包 Seq
	byte Seq;
	// 数据长度，小端
	ushort Length;

	// 超长数据包，会让 Length=0XFFFF，然后 LengthExt=实际长度。
	// uint LengthExt;
} SrmpHead_t;

#pragma pack(pop)

// 也可以写成 NewLifeHead_t
#define NewLifeHead_t SrmpHead_t

#include "Stream.h"
#include "CircularQueue.h"

/// <summary>NewLife SRMP 数据包获取长度</summary>
/// <param name="data">缓冲区</param>
/// <param name="len">缓冲区长度</param>
/// <returns>返回数据包长度；小于0，读取失败</returns>
int SrmpTryGetLen(byte* data, int len);

/// <summary>NewLife SRMP 数据包获取长度</summary>
/// <param name="st">流</param>
/// <returns></returns>
int SrmpTryGetLen_Stream(Stream_t* st);

/// <summary>NewLife SRMP 数据包获取长度</summary>
/// <param name="queue">环形队列</param>
/// <returns>返回数据包长度；小于0，读取失败</returns>
int SrmpTryGetLen_CircularQueue(CircularQueue_t* queue);


typedef struct
{
	// 头部
	SrmpHead_t* Head;

	// 返回消息
	bool Reply;
	// 错误代码
	int ErrCode;

	// 动作名称
	char* Action;
	// 动作字符串长度
	uint ActionLen;

	// 负载
	byte* Payload;
	// 负载长度。 
	uint PayloadLen;
}RpcInfo_t;

/// <summary>封装 RPC 通讯方式</summary>
/// <param name="data">内存缓冲</param>
/// <param name="datalen">内存大小</param>
/// <param name="action">RPC 名称</param>
/// <param name="rpc">RPC 对象</param>
/// <returns>返回负载允许的最大长度</returns>
int RpcInit(byte* data, int datalen, char* action, RpcInfo_t* rpc);

/// <summary>封装 RPC 返回消息构建</summary>
/// <param name="data">内存缓冲</param>
/// <param name="datalen">内存大小</param>
/// <param name="action">RPC 名称</param>
/// <param name="errCode">错误码，0无错</param>
/// <param name="rpc">RPC 对象</param>
/// <returns>返回负载允许的最大长度</returns>
int RpcReplyInit(byte* data, int datalen, char* action, uint errCode, RpcInfo_t* rpc);

/// <summary>创建回复消息</summary>
/// <param name="data">缓冲区</param>
/// <param name="datalen">缓冲区大小</param>
/// <param name="errCode">错误码，0无错</param>
/// <param name="src">原有消息</param>
/// <param name="dst">创建的新消息</param>
/// <returns>负载最大尺寸</returns>
int RpcCreatReply(byte* data, int datalen, uint errCode, RpcInfo_t* src, RpcInfo_t* dst);

/// <summary>修正/完善 RPC 数据包结构</summary>
/// <param name="rpc">RPC 对象</param>
/// <returns>返回整条消息长度</returns>
int RpcFix(RpcInfo_t* rpc);

/// <summary>修正/完善 RPC 数据包结构</summary>
/// <param name="rpc">RPC 对象</param>
/// <param name="seq">消息SEQ编号</param>
/// <returns>返回整条消息长度</returns>
int RpcFixWithSeq(RpcInfo_t* rpc, byte seq);

/// <summary>设置数据Seq值</summary>
/// <param name="data">数据包</param>
/// <param name="seq">Seq</param>
void RpcPktSetSeq(byte* data, byte seq);

/// <summary>解包RPC数据</summary>
/// <param name="data">数据</param>
/// <param name="len">数据大小</param>
/// <param name="rpc">rpc解出位置</param>
/// <returns>小于0表示出错</returns>
int RpcPktParse(byte* data, int len, RpcInfo_t* rpc);

/// <summary>打印RPC数据</summary>
void RpcShow(RpcInfo_t* rpc);

/*

/// <summary>匹配条码/二维码</summary>
/// <param name="codestr">条码/二维码</param>
/// <param name="buff"></param>
/// <param name="bufflen"></param>
/// <returns></returns>
int BuildMatchMsg(char* codestr, int posi, byte* buff, int bufflen)
{
	char* qr = codestr;
	if (qr == NULL)qr = "null";
	int result = -1;

	RpcInfo_t rpc;
	int maxlen = RpcInit(buff, bufflen, "HdDev/Match", &rpc);

	cJSON* root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "Qr", cJSON_CreateString(qr));
	// cJSON_AddItemToObject(root, "Uid", cJSON_CreateString(SysInfo.Uid));
	cJSON_AddItemToObject(root, "Offset", cJSON_CreateNumber(posi));

	char* msgbuff = cJSON_PrintUnformatted(root);
	if (msgbuff == NULL) { cJSON_Delete(root); return -1; }

	int pylen = strlen(msgbuff);
	if (pylen == 0)
	{
		result = -2;
	}
	else if (pylen < maxlen)
	{
		rpc.PayloadLen = pylen;
		memcpy(rpc.Payload, msgbuff, pylen);

		int pklen = RpcFix(&rpc);
		result = pklen;
	}

	// 使用 cJSON_Delete 内使用的 free 函数
	if (cJSON_free)cJSON_free(msgbuff);
	else GlobleFree(msgbuff);

	cJSON_Delete(root);
	return result;
}

/// <summary>匹配条码/二维码</summary>
/// <param name="codestr">条码/二维码</param>
/// <param name="buff"></param>
/// <param name="bufflen"></param>
/// <returns></returns>
int BuildMatchMsg(char* codestr, int posi, byte* buff, int bufflen)
{
	RpcInfo_t rpc;
	int maxlen = RpcInit(buff, bufflen, "HdDev/Match", &rpc);
	rpc.PayloadLen = snprintf(rpc.Payload, maxlen,
		"{\"Qr\":\"%s\",\"Uid\":\"%s\",\"Offset\":%d} ", codestr, SysInfo.Uid, posi);

	int pklen = RpcFix(&rpc);
	return pklen;
}


/// <summary>网络数据包处理</summary>
/// <param name="data"></param>
/// <param name="bufflen"></param>
void NetDataProcess(byte* data, int bufflen)
{
	// 最小数据长度，起码要把数据头放满吧。
	if (bufflen < sizeof(SrmpHead_t))return;

	SrmpHead_t* head = (SrmpHead_t*)data;
	if (head->Length == 0)	return;
	if (head->Length + sizeof(SrmpHead_t) > bufflen)	return;

	RpcInfo_t rpc;
	int err = RpcPktParse(data, bufflen, &rpc);
	if (err < 0)
	{
		DebugPrintf("RpcPktParse %d\r\n", err);
		return;
	}

	// 加字符串结标志\0 ，会破坏消息内容，确定不会造成影响才使用。
	rpc.Action[rpc.ActionLen] = '\0';
	rpc.Payload[rpc.PayloadLen] = '\0';

	if (!rpc.Reply)
	{
		// 请求消息处理。
		// DebugPrintf("RpcPktParse %d\r\n", err);

		// 不支持被请求的处理
		static byte cache[80];
		RpcInfo_t rs;
		int maxlen = RpcCreatReply(cache, sizeof(cache), 400, &rpc, &rs);
		memcpy(rs.Payload,"Not support",11);
		rs.PayloadLen = 11;
		int pklen = RpcFix(&rs);

		DebugPrintf("SEND ");
		RpcShow(&rs);

		DSend((byte*)rs.Head, pklen);

		return;
	}
	else
	{
		// 回复消息处理
		//		错误打印出来
		if (rpc.ErrCode != 0)
		{
			DebugPrintf("Rev %s Err %d\r\n\t%s\r\n", rpc.Action, rpc.ErrCode, rpc.Payload);
			return;
		}

		//		从action列表中找到对应的处理函数进行处理
		DebugPrintf("Rev %s %d\r\n\t%s\r\n", rpc.Action, rpc.ErrCode, rpc.Payload);
		for (int i = 0; i < sizeof(ActionProcess) / sizeof(ActionProcess[0]); i++)
		{
			if (strcmp(rpc.Action, ActionProcess[i].action) == 0)
			{
				ActionFunction_t pro = ActionProcess[i].Process;
				if (pro != NULL) pro(rpc.Payload, rpc.PayloadLen, &rpc);
				return;
			}
		}
	}
}

*/

