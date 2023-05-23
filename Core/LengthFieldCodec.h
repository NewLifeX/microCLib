#pragma once

#include "Type.h"

// 所有变量不可直接修改，避免出问题。
typedef struct
{
	// 长度字段所在位置
	short offset;
	// 长度字段的长度，1/2/4/-2/-4  负数为大端。
	short size;

	// 用于解析用的数据缓冲区
	// byte* cache;
	// cache 长度
	int cachelen;
}LengthFieldCodec_t;

// 初始化 LengthFieldCodec_t 变量
void LfcInit(LengthFieldCodec_t* lfc, short offset, short size);

// 从数组读长度，返回长度，小于1失败。支持 7 位压缩编码
int LfcGetLength(LengthFieldCodec_t* lfc, byte* data, int dlen);

// 写长度字段
// 返回写入的字节数，小于1失败。不支持 7 位压缩编码
// 可以使用 Stream 来处理 7位压缩编码问题。见文件末尾。
int LfcSetLength(LengthFieldCodec_t* lfc, byte* data, int dlen, int length);


#include "CircularQueue.h"
#include "Stream.h"


// 从环形队列读消息长度, 支持 7 位压缩编码
int LfcGetLenCircularQueue(LengthFieldCodec_t* lfc, CircularQueue_t* queue);

// 从流获取消息长度, 支持 7 位压缩编码
int LfcGetLenStream(LengthFieldCodec_t* lfc, Stream_t* st);


/*
	// 写 7位压缩编码问题方案。
	byte cache[32];
	int len = sizeof(cache);

	// 写 length 前面的数据
	Stream_t head;
	StreamInit(&head, data, len);
	StreamWriteByte(&head, xxxx);

	// 头部后面数据，预留5个字节的长度
	Stream_t tail;
	StreamInit(&tail,  cache + st.Position + 5 , len - st.Position - 5 );
	StreamWriteByte(&tail, xxxx);

	// 回过头来填充长度信息。
	StreamWriteCompressionUint(&head,tail.Position);

	// 挪后面的数据
	StreamWriteBytes(&head,tail.MemStart,tail.Position);

*/

