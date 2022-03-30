
#include "Type.h"
#include "Stream.h"

void StreamInit(Stream_t* st, byte* p, int len)
{
	if (st == NULL)return;

	st->MemStart = p;
	st->Position = 0;
	st->Size = len;
}

int StreamWriteByte(Stream_t* st, byte data)
{
	if (st == NULL)return 0;

	// 下标最大值是 Size-1  超过就说明写满了。
	if (st->Position >= st->Size)return 0;

	st->MemStart[st->Position] = data;
	st->Position++;

	return 1;
}

int StreamWriteUshort(Stream_t* st, ushort data)
{
	if (st == NULL)return 0;

	// 下标最大值是 Size-1  超过就说明写满了。
	if (st->Position + 2 > st->Size)return 0;

	if (st == NULL)return 0;

	st->MemStart[st->Position] = (byte)data;
	st->MemStart[st->Position + 1] = (byte)(data >> 8);
	st->Position += 2;

	return 2;
}

int StreamWriteUint(Stream_t* st, uint data)
{
	if (st == NULL)return 0;
	// 下标最大值是 Size-1  超过就说明写满了。
	if (st->Position + 4 > st->Size)return 0;

	st->MemStart[st->Position] = (byte)data;
	st->MemStart[st->Position + 1] = (byte)(data >> 8);
	st->MemStart[st->Position + 2] = (byte)(data >> 16);
	st->MemStart[st->Position + 3] = (byte)(data >> 24);
	st->Position += 4;

	return 4;
}

// 7位压缩编码。 取字节最高bit作为标记，标记后续字节还是不是内容。
// 每字节有效数据 7bit
int StreamWriteCompressionUint(Stream_t* st, uint data)
{
	if (st == NULL)return 0;

	int len;
	if (data < 128) len = 1;
	else if (data < 16384)len = 2;
	else if (data < 2097152)len = 3;
	else if (data < 268435456)len = 4;
	else len = 5;

	if (st->Position + len > st->Size)return 0;

	do
	{
		byte d = (data & 0x7f);
		data >>= 7;
		if (data > 0)d |= 0x80;
		st->MemStart[st->Position++] = d;
	} while (data > 0);

	return len;
}

int StreamWriteBytes(Stream_t* st, byte* p, int len)
{
	if (st == NULL)return 0;

	// 空间不够一个不写。 避免出现写了半截造成错误。
	if ((st->Size - st->Position) < len)return 0;

	memcpy(&st->MemStart[st->Position], p, len);
	st->Position += len;

	return len;
}

int StreamReadByte(Stream_t* st, byte* p)
{
	if (st == NULL)return 0;

	if (st->Position >= st->Size)return 0;

	*p = st->MemStart[st->Position];
	st->Position++;

	return 1;
}

int StreamReadUshort(Stream_t* st, ushort* data)
{
	if (st == NULL)return 0;

	// 下标最大值是 Size-1  超过就说明写满了。
	if (st->Position + 2 > st->Size)return 0;

	if (st == NULL)return 0;

	memcpy((byte*)data, st->MemStart + st->Position, 2);
	st->Position += 2;
	return 2;
}

int StreamReadUint(Stream_t* st, uint* data)
{
	if (st == NULL)return 0;
	// 下标最大值是 Size-1  超过就说明写满了。
	if (st->Position + 4 > st->Size)return 0;

	memcpy((byte*)data, st->MemStart + st->Position, 4);
	st->Position += 4;
	return 4;
}

// 7位压缩编码。 取字节最高bit作为标记，标记后续字节还是不是内容。
// 每字节有效数据 7bit
int StreamReadCompressionUint(Stream_t* st, uint* data)
{
	if (st == NULL)return 0;
	byte* p = st->MemStart + st->Position;
	int remain = st->Size - st->Position;
	if (remain > 5)remain = 5;

	// 结果值
	uint res = 0;
	// 移位数，避免乘法
	uint mov = 0;
	// 拿出来使用，避免读失败的时候无法回退
	int posi = st->Position;
	// 读了的数据长度
	int len = 0;
	for (int i = 0; i < remain; i++)
	{
		// 取7bit  然后按照次数往后填充。
		uint temp = (p[i] & 0x7f) << mov;
		res |= temp;
		mov += 7;
		posi++;

		if ((p[i] & 0x80) == 0)
		{
			// 下标跟长度的转换。
			len = i + 1;
			break;
		}
	}

	// 超出字节数也没找到结尾字节，len == 0 。读取失败
	if (len != 0)
	{
		*data = res;
		st->Position = posi;
	}

	return len;
}

int StreamReadBytes(Stream_t* st, byte* p, int len)
{
	if (st == NULL)return 0;

	// 空间不够一个不写。 避免出现写了半截造成错误。
	if ((st->Size - st->Position) < len)return 0;

	memcpy(p, &st->MemStart[st->Position], len);
	st->Position += len;

	return len;
}

// 获取空闲空间
int StreamRemian(Stream_t* st)
{
	if (st == NULL)return 0;

	return (st->Size - st->Position);
}

int StreamSeek(Stream_t* st, int sek, SeekOrigin_e of)
{
	if (st == NULL)return 0;

	switch (of)
	{
	case SeekBegin:
		// sek 只能是正数
		if (sek < 0)sek = 0;
		if (sek > st->Size)sek = st->Size;
		st->Position = sek;
		break;
	case SeekEnd:
		// sek 只能是负数。
		if (sek > 0)sek = 0;
		if (sek < (0 - st->Size))sek = (0 - st->Size);
		st->Position = st->Size + sek;
		break;
	case SeekCurrent:
		if (sek + st->Position > st->Size)
		{
			st->Position = st->Size;
			break;
		}

		if (sek + st->Position < 0)
		{
			st->Position = 0;
			break;
		}

		st->Position += sek;
		break;
	}

	return st->Position;
}

static MemOptions_t const OptDefault =
{
	.malloc = malloc,
	.free = free,
};

// 自动扩容的 Steam 函数 API。
// 初始化的时候自动分配 128 字节。后续扩容按照 128 字节增加。
// 扩容的内存来自 GlobalMalloc GlobalFree 这对函数。
void AutoStreamInit(Stream_t* st, int len, MemOptions_t* opt)
{
	if (opt == NULL)opt = (MemOptions_t*)&OptDefault;
	if (len < 1)len = 128;

	byte* p = (byte*)opt->malloc(len);

	if (p != NULL)
	{
		StreamInit(st, p, len);
	}
	else
	{
		st->Size = 0;
		st->Position = 0;
		st->MemStart = NULL;
	}
}

void AutoStreamDeInit(Stream_t* st, MemOptions_t* opt)
{
	if (opt == NULL)opt = (MemOptions_t*)&OptDefault;

	if (st->MemStart != NULL)
	{
		opt->free(st->MemStart);
	}

	st->MemStart = NULL;
	st->Position = 0;
	st->Size = 0;
}

bool AutoStreamSetCapacity(Stream_t* st, int len, MemOptions_t* opt)
{
	if (st->Size >= len)return true;

	if (opt == NULL)opt = (MemOptions_t*)&OptDefault;

	byte* p = (byte*)opt->malloc(len);
	byte* oldp = st->MemStart;

	if (p == NULL)return false;
	memcpy(p, st->MemStart, st->Size);

	st->MemStart = p;
	st->Size = len;

	opt->free(oldp);
	return true;
}

int AutoStreamWriteByte(Stream_t* st, byte data, MemOptions_t* opt)
{
	if (opt == NULL)opt = (MemOptions_t*)&OptDefault;

	int remain = StreamRemian(st);
	if (remain < 1)
	{
		byte* oldp = st->MemStart;

		// 128 字节对齐
		int newsize = ((st->Size + 1) + 127) & (~127);

		byte* p = (byte*)opt->malloc(newsize);
		if (p == NULL) return 0;

		memcpy(p, st->MemStart, st->Size);

		st->MemStart = p;
		st->Size = newsize;

		if (oldp != NULL)opt->free(oldp);
	}

	return StreamWriteByte(st, data);
}

int AutoStreamWriteBytes(Stream_t* st, byte* p, int len, MemOptions_t* opt)
{
	if (opt == NULL)opt = (MemOptions_t*)&OptDefault;

	int remain = StreamRemian(st);
	if (remain < len)
	{
		byte* oldp = st->MemStart;
		// 128 字节对齐
		int newsize = ((st->Size + len) + 127) & (~127);

		byte* p = (byte*)opt->malloc(newsize);
		if (p == NULL) return 0;

		memcpy(p, st->MemStart, st->Size);

		st->MemStart = p;
		st->Size = newsize;

		if (oldp != NULL)opt->free(oldp);
	}

	return StreamWriteBytes(st, p, len);
}
