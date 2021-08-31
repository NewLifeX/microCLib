
#include "CircularQueue.h"
#include "Debug.h"
// #include "Array.h"

#ifndef __INLINE
#define __INLINE inline
#endif

// 初始化
bool CircularQueueInit(CircularQueue_t* queue, byte* p, int len)
{
	if (queue == NULL)ErrPrintf("queue = NULL");
	if (p == NULL)ErrPrintf("p = NULL");

	if (len < 16) WarningPrintf("len = %d small\r\n", len);

	queue->MemStart = p;
	queue->MemEnd = queue->MemStart + len;
	queue->Size = len;

	queue->pHead = queue->MemStart;
	queue->pTail = queue->MemStart;

	return true;
}

// 写入一字节
__INLINE int CircularQueueWrite(CircularQueue_t* queue, byte data)
{
	// 剩余可写长度
	int remian = queue->Size - CircularQueueGetLength(queue) - 1;
	if (remian < 1)return 0;

	*queue->pHead = data;
	queue->pHead++;
	if (queue->pHead >= queue->MemEnd)queue->pHead = queue->MemStart;

	return 1;
}

// 写入多字节
int CircularQueueWrites(CircularQueue_t* queue, byte* pdata, int len)
{
	// 剩余可写长度
	int remian = CircularQueueRemain(queue);

	// 确保不写爆掉。
	if (len > remian)len = remian;

	// 距离结尾处的长度
	int xxlen = queue->MemEnd - queue->pHead;
	if (xxlen >= len)
	{
		memcpy((byte*)queue->pHead, pdata, len);
		queue->pHead += len;
		if (queue->pHead == queue->MemEnd)queue->pHead = queue->MemStart;
		return len;
	}
	else
	{
		memcpy((byte*)queue->pHead, pdata, xxlen);
		pdata += xxlen;

		int cpylen2 = len - xxlen;
		memcpy(queue->MemStart, pdata, cpylen2);
		queue->pHead = queue->MemStart + cpylen2;
		return len;
	}

	// return 0;
}

// 获取有效数据长度
__INLINE int CircularQueueGetLength(CircularQueue_t* queue)
{
	uint head = (uint)queue->pHead;
	uint tail = (uint)queue->pTail;

	if (head >= tail)
	{
		return head - tail;
	}
	else
	{
		int queuelen = queue->Size;
		// 上计算简化为：
		return head + queuelen - tail;
	}
}

__INLINE int CircularQueueRemain(CircularQueue_t* queue)
{
	return queue->Size - CircularQueueGetLength(queue) - 1;
}

// 读一字节数据，返回是否成功
__INLINE bool CircularQueueRead(CircularQueue_t* queue, byte* pdata)
{
	if (queue->pTail == queue->pHead)return 0;

	*pdata = *(byte*)queue->pTail;
	queue->pTail++;
	if (queue->pTail >= queue->MemEnd)queue->pTail = queue->MemStart;

	return false;
}

// 读数据， maxlen 是最大读取字节数，返回实际读出字节数
int CircularQueueReads(CircularQueue_t* queue, byte* pdata, int maxlen, bool justread)
{
	byte* headnow = (byte*)queue->pHead;
	if (queue->pTail != headnow)
	{
		if (headnow > queue->pTail)
		{
			int cpylen = headnow - queue->pTail;
			if (cpylen > maxlen)cpylen = maxlen;
			memcpy(pdata, (byte*)queue->pTail, cpylen);

			if (!justread)queue->pTail += cpylen;
			return cpylen;
		}
		else
		{
			int queuelen = queue->Size;

			int cpylen = queue->MemStart + queuelen - queue->pTail;
			if (cpylen > maxlen)
			{
				cpylen = maxlen;
				memcpy(pdata, (byte*)queue->pTail, cpylen);

				if (!justread)
				{
					queue->pTail += cpylen;
					if (queue->pTail >= queue->MemEnd)
					{
						queue->pTail = queue->MemStart;
					}
				}
				return cpylen;
			}

			memcpy(pdata, (byte*)queue->pTail, cpylen);
			pdata += cpylen;
			maxlen -= cpylen;

			int cpylen2 = headnow - queue->MemStart;
			if (cpylen2 > maxlen)cpylen2 = maxlen;

			memcpy(pdata, (byte*)queue->MemStart, cpylen2);

			if (!justread) { queue->pTail = queue->MemStart + cpylen2; }
			return cpylen + cpylen2;
		}
	}

	return 0;
}

// 移位环形队列，只允许往前seek，不许后退。
uint CircularQueueSeek(CircularQueue_t* queue, uint offset)
{
	byte* headnow = (byte*)queue->pHead;
	// 不允许越界
	if (queue->pTail == headnow) return 0;

	// 内存大小
	int queuelen = queue->Size;

	// 计算 queue 内有效数据长度
	int hasdata;
	if (headnow < queue->pTail)
	{
		hasdata = headnow + queuelen - queue->pTail;
	}
	else
	{
		hasdata = headnow - queue->pTail;
	}

	// 数据长度不够offset的，直接处理。
	if (hasdata <= offset)
	{
		queue->pTail = headnow;
		return hasdata;
	}

	// 有效数据大于需要seek的数据长度。
	uint newTail = (uint)queue->pTail + offset;
	if (newTail > (uint)queue->MemEnd) newTail -= queuelen;
	queue->pTail = (byte*)newTail;

	return offset;
}

// 查找数据位置。没有找到返回 -1
int CircularQueueIndexOf(CircularQueue_t* queue, byte* sub, int sublen)
{
	if (queue == NULL)return -1;
	if (sub == NULL)return -1;
	if (sublen < 1)return -1;
	int queuelen = CircularQueueGetLength(queue);
	if (queuelen < sublen)return -1;

	// 考虑到中断会被修改，直接线copy出来用。
	// byte* head = (byte*)queue->pHead;
	byte* tail = (byte*)queue->pTail;
	byte* memend = queue->MemEnd;

	// 大概就是抄 ArrayIndexOf  但是处理了不连续内存的问题。
	// 原本区分，连续内存和不连续内存。分开处理。
	// 重新考量了一下，只是在  if (tail >= queue->MemEnd) 多了几条汇编，
	// 优化成 if (tail2 >= memend) 即一条汇编（判断跳转指令）。 性能损失忽略不计。
	int maxsearch = queuelen - sublen + 1;
	int i, j;
	for (i = 0; i < maxsearch; i++)
	{
		if (sub[0] == *tail)
		{
			if (sublen == 1)return i;
			byte* tail2 = tail;
			for (j = 1; j < sublen; j++)
			{
				tail2++;
				if (tail2 >= memend)tail2 = queue->MemStart;

				if (sub[j] != *tail2)break;
			}

			if (j == sublen)
				return i;
		}

		tail++;
		if (tail >= memend)tail = queue->MemStart;
	}

	return -1;
}
