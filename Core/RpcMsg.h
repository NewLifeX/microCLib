#pragma once

#include "Type.h"

/*
* ��λ�����ᳬ�� 65535 �ĸ��أ����Բ�֧�ֳ������ݰ���
*/

#pragma pack(push)
#pragma pack(1)

// NewLife SRMP ��׼���ݰ�ͷ
typedef struct
{
	// Flag Ϊλ��־
	// bit7  0x80  Reply
	// bit6  0x40  Error
	// bit 0-5	   ���ݰ���ʽ��Ĭ��ʹ��0 
	// ����δ����
	byte Flag;
	// ���ݰ� Seq
	byte Seq;
	// ���ݳ��ȣ�С��
	ushort Length;

	// �������ݰ������� Length=0XFFFF��Ȼ�� LengthExt=ʵ�ʳ��ȡ�
	// uint LengthExt;
} SrmpHead_t;

#pragma pack(pop)

// Ҳ����д�� NewLifeHead_t
#define NewLifeHead_t SrmpHead_t

#include "Stream.h"
#include "CircularQueue.h"

/// <summary>NewLife SRMP ���ݰ���ȡ����</summary>
/// <param name="data">������</param>
/// <param name="len">����������</param>
/// <returns>�������ݰ����ȣ�С��0����ȡʧ��</returns>
int SrmpTryGetLen(byte* data, int len);

/// <summary>NewLife SRMP ���ݰ���ȡ����</summary>
/// <param name="st">��</param>
/// <returns></returns>
int SrmpTryGetLen_Stream(Stream_t* st);

/// <summary>NewLife SRMP ���ݰ���ȡ����</summary>
/// <param name="queue">���ζ���</param>
/// <returns>�������ݰ����ȣ�С��0����ȡʧ��</returns>
int SrmpTryGetLen_CircularQueue(CircularQueue_t* queue);


typedef struct
{
	// ͷ��
	SrmpHead_t* Head;

	// ������Ϣ
	bool Reply;
	// �������
	int ErrCode;

	// ��������
	char* Action;
	// �����ַ�������
	uint ActionLen;

	// ����
	byte* Payload;
	// ���س��ȡ� 
	uint PayloadLen;
}RpcInfo_t;

/// <summary>��װ RPC ͨѶ��ʽ</summary>
/// <param name="data">�ڴ滺��</param>
/// <param name="datalen">�ڴ��С</param>
/// <param name="action">RPC ����</param>
/// <param name="rpc">RPC ����</param>
/// <returns>���ظ����������󳤶�</returns>
int RpcInit(byte* data, int datalen, char* action, RpcInfo_t* rpc);

/// <summary>��װ RPC ������Ϣ����</summary>
/// <param name="data">�ڴ滺��</param>
/// <param name="datalen">�ڴ��С</param>
/// <param name="action">RPC ����</param>
/// <param name="errCode">�����룬0�޴�</param>
/// <param name="rpc">RPC ����</param>
/// <returns>���ظ����������󳤶�</returns>
int RpcReplyInit(byte* data, int datalen, char* action, uint errCode, RpcInfo_t* rpc);

/// <summary>�����ظ���Ϣ</summary>
/// <param name="data">������</param>
/// <param name="datalen">��������С</param>
/// <param name="errCode">�����룬0�޴�</param>
/// <param name="src">ԭ����Ϣ</param>
/// <param name="dst">����������Ϣ</param>
/// <returns>�������ߴ�</returns>
int RpcCreatReply(byte* data, int datalen, uint errCode, RpcInfo_t* src, RpcInfo_t* dst);

/// <summary>����/���� RPC ���ݰ��ṹ</summary>
/// <param name="rpc">RPC ����</param>
/// <returns>����������Ϣ����</returns>
int RpcFix(RpcInfo_t* rpc);

/// <summary>����/���� RPC ���ݰ��ṹ</summary>
/// <param name="rpc">RPC ����</param>
/// <param name="seq">��ϢSEQ���</param>
/// <returns>����������Ϣ����</returns>
int RpcFixWithSeq(RpcInfo_t* rpc, byte seq);

/// <summary>��������Seqֵ</summary>
/// <param name="data">���ݰ�</param>
/// <param name="seq">Seq</param>
void RpcPktSetSeq(byte* data, byte seq);

/// <summary>���RPC����</summary>
/// <param name="data">����</param>
/// <param name="len">���ݴ�С</param>
/// <param name="rpc">rpc���λ��</param>
/// <returns>С��0��ʾ����</returns>
int RpcPktParse(byte* data, int len, RpcInfo_t* rpc);

/// <summary>��ӡRPC����</summary>
void RpcShow(RpcInfo_t* rpc);

/*

/// <summary>ƥ������/��ά��</summary>
/// <param name="codestr">����/��ά��</param>
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

	// ʹ�� cJSON_Delete ��ʹ�õ� free ����
	if (cJSON_free)cJSON_free(msgbuff);
	else GlobleFree(msgbuff);

	cJSON_Delete(root);
	return result;
}

/// <summary>ƥ������/��ά��</summary>
/// <param name="codestr">����/��ά��</param>
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


/// <summary>�������ݰ�����</summary>
/// <param name="data"></param>
/// <param name="bufflen"></param>
void NetDataProcess(byte* data, int bufflen)
{
	// ��С���ݳ��ȣ�����Ҫ������ͷ�����ɡ�
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

	// ���ַ������־\0 �����ƻ���Ϣ���ݣ�ȷ���������Ӱ���ʹ�á�
	rpc.Action[rpc.ActionLen] = '\0';
	rpc.Payload[rpc.PayloadLen] = '\0';

	if (!rpc.Reply)
	{
		// ������Ϣ����
		// DebugPrintf("RpcPktParse %d\r\n", err);

		// ��֧�ֱ�����Ĵ���
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
		// �ظ���Ϣ����
		//		�����ӡ����
		if (rpc.ErrCode != 0)
		{
			DebugPrintf("Rev %s Err %d\r\n\t%s\r\n", rpc.Action, rpc.ErrCode, rpc.Payload);
			return;
		}

		//		��action�б����ҵ���Ӧ�Ĵ��������д���
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

