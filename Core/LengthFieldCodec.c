
#include "LengthFieldCodec.h"
#include "CircularQueue.h"
#include "Stream.h"

void LfcInit(LengthFieldCodec_t* lfc, short offset, short size)
{
	if (size > 4)while (1);
	if (offset > 64)while (1);

	int clen = offset + abs(size);
	lfc->cachelen = clen;

	lfc->offset = offset;
	lfc->size = size;
}

static int normalRead(byte* p, int size)
{
	switch (size)
	{
	case 1:
	case -1: return p[0]; 
	case 2: return (p[1] << 8) + p[0];
	case -2: return (p[0] << 8) + p[1]; 
	case 4:
	{
		int len = 0;
		len += p[0];
		len += p[1] << 8;
		len += p[2] << 16;
		len += p[3] << 24;

		return len;
	}
	case -4:
	{
		int len = 0;
		len += p[3];
		len += p[2] << 8;
		len += p[1] << 16;
		len += p[0] << 24;

		return len;
	}
	}

	return -3;
}

// 返回写入的字节数
static int normalWrite(byte* p, uint len, int size)
{
	switch (size)
	{
	case 1:
	case -1:
	{
		p[0] = (byte)len;
		return 1;
	}	
	case 2:
	{
		p[0] = len;
		p[1] = len >> 8;
		return 2;
	}
	case -2:
	{
		p[1] = len;
		p[0] = len >> 8;
		return 2;
	}
	case 4:
	{
		p[0] = len;
		p[1] = len >> 8;
		p[2] = len >> 16;
		p[3] = len >> 24;
		return 4;
	}
	case -4:
	{
		p[3] = len;
		p[2] = len >> 8;
		p[1] = len >> 16;
		p[0] = len >> 24;
		return 4;
	}
	}

	return -3;
}

int LfcGetLength(LengthFieldCodec_t* lfc, byte* data, int dlen)
{
	if (data == NULL)return -1;
	if (dlen < lfc->cachelen)return -2;

	byte* p = data + lfc->offset;
	dlen -= lfc->offset;

	if (lfc->size == 0)
	{
		int len = 0;
		for (int i = 0; i < 5; i++)
		{
			// 数据不够长，失败
			if (dlen - i < 1)return -3;

			byte temp = p[i];
			// 处理数据
			int v7 = temp & 0x7f;
			len += (v7 << (7 * i));
			// 判断是不是结尾。
			if ((temp & 0x80) == 0) return len;
		}

		// 5个字节没有找到结尾，报错。数据大于 int 范围了。
		return -4;
	}

	return normalRead(p, lfc->size);
}

// 返回写入的字节数，小于0失败。
int LfcSetLength(LengthFieldCodec_t* lfc, byte* data, int dlen, int length)
{
	if (data == NULL)return -1;
	if (dlen < lfc->cachelen)return -2;

	if (lfc->size != 0)	return normalWrite(data + lfc->offset, length, lfc->size);
	return -4;
}

int LfcGetLenCircularQueue(LengthFieldCodec_t* lfc, CircularQueue_t* queue)
{
	if (CircularQueueGetLength(queue) < lfc->cachelen)return -1;

	if (lfc->size != 0)
	{
		// copy 出来，读是不影响原 queue 的
		CircularQueue_t q2;
		memcpy(&q2, queue, sizeof(q2));
		CircularQueueSeek(&q2, lfc->offset);

		byte p[4];
		CircularQueueReads(&q2, p, abs(lfc->size), true);

		return normalRead(p, lfc->size);
	}
	else
	{
		// copy 出来，读是不影响原 queue 的
		CircularQueue_t q2;
		memcpy(&q2, queue, sizeof(q2));
		CircularQueueSeek(&q2, lfc->offset);

		// 直接使用 queue 提供的办法
		uint len = 0xffffffff;
		CircularQueueReadCompressionUint(&q2, &len);

		// 读失败判断。
		if (len == 0xffffffff)return -4;
		return len;
	}
}

int LfcGetLenStream(LengthFieldCodec_t* lfc, Stream_t* st)
{
	if (StreamRemian(st) < lfc->cachelen)return -1;

	if (lfc->size != 0)
	{
		byte* p = st->MemStart + st->Position + lfc->offset;
		return normalRead(p, lfc->size);
	}
	else
	{
		// 直接用 stream 的读法。
		int posi = st->Position;

		StreamSeek(st, lfc->offset, SeekCurrent);
		uint len = 0;
		if (StreamReadCompressionUint(st, &len) < 1)
		{
			st->Position = posi;
			return -5;
		}

		// 恢复 posi
		st->Position = posi;

		return len;
	}
}
