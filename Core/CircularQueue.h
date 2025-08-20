#pragma once

#include "Type.h"

typedef struct
{
	// 内存空间
	byte* MemStart;
	byte* MemEnd;
	// 内存容量 = MemEnd - MemStart
	uint Size;

	// 两个游标，写游标和读游标
	volatile byte* pHead;
	volatile byte* pTail;
}CircularQueue_t;

// 初始化。  p 和 len 是 CircularQueueInit 管理的内存区域。
bool CircularQueueInit(CircularQueue_t* CQHandle, byte* p, int len);

// 写入一字节,返回写入字节数
int CircularQueueWrite(CircularQueue_t* CQHandle, byte data);

// 写入多字节,返回写入字节数
int CircularQueueWrites(CircularQueue_t* CQHandle, byte* pdata, int len);

/// <summary>写7位压缩编码</summary>
/// <param name="queue"></param>
/// <param name="data"></param>
/// <returns>返回写入的字节数</returns>
int CircularQueueWriteCompressionUint(CircularQueue_t* queue, uint data);

// 获取有效数据长度
int CircularQueueGetLength(CircularQueue_t* CQHandle);

// 获取剩余容量
int CircularQueueRemain(CircularQueue_t* queue);

// 读一字节数据，返回是否成功
bool CircularQueueRead(CircularQueue_t* CQHandle, byte* pdata);

// 读数据， maxlen 是最大读取字节数，返回实际读出字节数
// justread = true 的时候,不修改 Stream 数据。只读
int CircularQueueReads(CircularQueue_t* CQHandle, byte* pdata, int maxlen, bool justread);

// 尝试读数据 peek 1字节。
bool CircularQueuePeek(CircularQueue_t* queue, byte* pdata);
// 尝试读数据s，peek 多字节。要么不成功，要么全成功
bool CircularQueuePeeks(CircularQueue_t* queue, byte* pdata, int len);

/// <summary>读7位压缩编码</summary>
/// <param name="queue"></param>
/// <param name="data">输出</param>
/// <returns>返回读掉的长度</returns>
int CircularQueueReadCompressionUint(CircularQueue_t* queue, uint* data);

// 移位,环形队列，只允许往前 seek，不许后退。
// 相当于 CircularQueueReads(queue, NULL,offset,false); 不读数据但是要移位
// offset 会被处理成不会被越界！。
// 返回实际 offset 的值。
uint CircularQueueSeek(CircularQueue_t* CQHandle, uint offset);

// 查找数据位置。没有找到返回 -1
int CircularQueueIndexOf(CircularQueue_t* CQHandle, byte* data, int len);

/// <summary>清空数据内容</summary>
/// <param name="CQHandle"></param>
void CircularQueueClear(CircularQueue_t* CQHandle);

/// <summary>打印队列内容</summary>
/// <param name="CQHandle"></param>
/// <param name="showlen">打印长度。 -1 表示全部</param>
/// <returns></returns>
void CircularQueueShow(CircularQueue_t* CQHandle, int showlen);
