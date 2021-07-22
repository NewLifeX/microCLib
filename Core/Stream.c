
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
void AutoStreamInit(Stream_t* st, int len ,MemOptions_t* opt)
{
	if (opt == NULL)opt = (MemOptions_t*)&OptDefault;
	if (len < 1)len = 128;

	byte* p = (byte*)opt->malloc(len);

	if (p != NULL)
	{
		StreamInit(st, p, 128);
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
