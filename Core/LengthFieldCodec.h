#pragma once

#include "Type.h"

// ���б�������ֱ���޸ģ���������⡣
typedef struct
{
	// �����ֶ�����λ��
	short offset;
	// �����ֶεĳ��ȣ�1/2/4/-2/-4  ����Ϊ��ˡ�
	short size;

	// ���ڽ����õ����ݻ�����
	// byte* cache;
	// cache ����
	int cachelen;
}LengthFieldCodec_t;

// ��ʼ�� LengthFieldCodec_t ����
void LfcInit(LengthFieldCodec_t* lfc, short offset, short size);

// ����������ȣ����س��ȣ�С��1ʧ�ܡ�֧�� 7 λѹ������
int LfcGetLength(LengthFieldCodec_t* lfc, byte* data, int dlen);

// д�����ֶ�
// ����д����ֽ�����С��1ʧ�ܡ���֧�� 7 λѹ������
// ����ʹ�� Stream ������ 7λѹ���������⡣���ļ�ĩβ��
int LfcSetLength(LengthFieldCodec_t* lfc, byte* data, int dlen, int length);


#include "CircularQueue.h"
#include "Stream.h"


// �ӻ��ζ��ж���Ϣ����, ֧�� 7 λѹ������
int LfcGetLenCircularQueue(LengthFieldCodec_t* lfc, CircularQueue_t* queue);

// ������ȡ��Ϣ����, ֧�� 7 λѹ������
int LfcGetLenStream(LengthFieldCodec_t* lfc, Stream_t* st);


/*
	// д 7λѹ���������ⷽ����
	byte cache[32];
	int len = sizeof(cache);

	// д length ǰ�������
	Stream_t head;
	StreamInit(&head, data, len);
	StreamWriteByte(&head, xxxx);

	// ͷ���������ݣ�Ԥ��5���ֽڵĳ���
	Stream_t tail;
	StreamInit(&tail,  cache + st.Position + 5 , len - st.Position - 5 );
	StreamWriteByte(&tail, xxxx);

	// �ع�ͷ����䳤����Ϣ��
	StreamWriteCompressionUint(&head,tail.Position);

	// Ų���������
	StreamWriteBytes(&head,tail.MemStart,tail.Position);

*/

