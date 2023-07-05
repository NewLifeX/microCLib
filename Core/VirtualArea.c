
#include "VirtualArea.h"
#include "Stream.h"
#include "BitHelper.h"

// 检测空间是否有交集
static bool AreaCheck(Area_t* a1, Area_t* a2)
{
	uint a1end = a1->Addr + a1->Size;
	if (a1end <= a2->Addr)return false;
	uint a2end = a2->Addr + a2->Size;
	if (a2end <= a1->Addr)return false;

	return true;
}

static int min(int a, int b)
{
	if (a < b)return a;
	return b;
}

// 将 src 的数据写到 dst 内。
int AreaByteCopy(Area_t* dst, Area_t* src)
{
	Stream_t dstSt;
	StreamInit(&dstSt, dst->Buff, dst->Size);

	if (dst->Addr <= src->Addr)
	{
		// 从 src 头读，写到 dst 后面位置。
		int posi = src->Addr - dst->Addr;
		StreamSeek(&dstSt, posi, SeekBegin);

		// dst 空间剩余别越界,取最小值即可。
		int remian = StreamRemian(&dstSt);
		return StreamWriteBytes(&dstSt, src->Buff, min(remian, src->Size));
	}
	else
	{
		// 从 src 半截位置，写到 dst 头部。
		byte* src_p = src->Buff;
		byte src_len = src->Size;

		int posi = dst->Addr - src->Addr;
		src_p += posi;
		src_len -= posi;

		// 控制一下剩余别越界。
		int remain = StreamRemian(&dstSt);
		return StreamWriteBytes(&dstSt, src_p, min(remain, src_len));
	}
}

// 将 src 的 bit 数据写到 dst 内。
int AreaBitCopy(Area_t* dst, Area_t* src)
{
	if (dst->Addr <= src->Addr)
	{
		// 从 src 头读，写到 dst 后面位置。
		int dst_offset = src->Addr - dst->Addr;

		int cplen = min(dst->Size - dst_offset, src->Size);

		SetBits(dst->Buff, dst_offset, src->Buff, 0, cplen);

		return cplen;
	}
	else
	{
		int src_offset = dst->Addr - src->Addr;

		int cplen = min(src->Size - src_offset, dst->Size);

		SetBits(dst->Buff, 0, src->Buff, src_offset, cplen);

		return cplen;
	}
}

/// <summary>读多个空间数据，合并，半截</summary>
/// <param name="dst">目标空间</param>
/// <param name="srcs">空间数组，最后一项的 SIZE 小于 1</param>
/// <param name="isBit">是否是bit空间</param>
void AreaRead(Area_t* dst, Area_t* srcs, bool isBit)
{
	if (dst->Init)dst->Init(dst);

	while (true)
	{
		if (srcs->Size < 1)break;

		if (AreaCheck(dst, srcs))
		{
			if (srcs->Init) srcs->Init(srcs);

			if (isBit)AreaBitCopy(dst, srcs);
			else AreaByteCopy(dst, srcs);
		}

		srcs++;
	}
}

/// <summary>从多个空间读取数据</summary>
/// <param name="srcs">被读空间s</param>
/// <param name="addr">读地址</param>
/// <param name="data">读缓冲</param>
/// <param name="len">读长度</param>
/// <param name="isBit">是否是bit空间</param>
void AreaRead2(Area_t* srcs, int addr, byte* data, int len, bool isBit)
{
	Area_t ar;
	ar.Addr = addr;
	ar.Buff = data;
	ar.Size = len;
	ar.Init = NULL;
	ar.Fresh = NULL;

	AreaRead(&ar, srcs, isBit);
}

/// <summary>写多个空间数据，从大空间拆到小空间，拆出来 fresh 一下</summary>
/// <param name="dsts">目标空间数组，最后一项的 SIZE 小于 1</param>
/// <param name="src">源</param>
/// <param name="isBit">是否是bit空间</param>
void AreaWrite(Area_t* dsts, Area_t* src, bool isBit)
{
	if (src->Init)src->Init(src);

	while (true)
	{
		if (dsts->Size < 1)break;

		if (AreaCheck(dsts, src))
		{
			if (dsts->Init)dsts->Init(dsts);
			// 分出去
			if (isBit)AreaBitCopy(dsts, src);
			else AreaByteCopy(dsts, src);
			// 刷新一下，比如：使用数据
			if (dsts->Fresh)dsts->Fresh(dsts);
		}

		dsts++;
	}
}

/// <summary>向多个空间写数据</summary>
/// <param name="dsts">被写入</param>
/// <param name="addr">写入地址</param>
/// <param name="data">写入数据</param>
/// <param name="len">写入长度</param>
/// <param name="isBit">是否是bit空间</param>
void AreaWrite2(Area_t* dsts, int addr, byte* data, int len, bool isBit)
{
	Area_t ar;
	ar.Addr = addr;
	ar.Buff = data;
	ar.Size = len;
	ar.Init = NULL;
	ar.Fresh = NULL;

	AreaWrite(dsts, &ar, isBit);
}
