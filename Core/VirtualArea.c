
#include "VirtualArea.h"
#include "Stream.h"
#include "BitHelper.h"

// ���ռ��Ƿ��н���
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

// �� src ������д�� dst �ڡ�
int AreaByteCopy(Area_t* dst, Area_t* src)
{
	Stream_t dstSt;
	StreamInit(&dstSt, dst->Buff, dst->Size);

	if (dst->Addr <= src->Addr)
	{
		// �� src ͷ����д�� dst ����λ�á�
		int posi = src->Addr - dst->Addr;
		StreamSeek(&dstSt, posi, SeekBegin);

		// dst �ռ�ʣ���Խ��,ȡ��Сֵ���ɡ�
		int remian = StreamRemian(&dstSt);
		return StreamWriteBytes(&dstSt, src->Buff, min(remian, src->Size));
	}
	else
	{
		// �� src ���λ�ã�д�� dst ͷ����
		byte* src_p = src->Buff;
		byte src_len = src->Size;

		int posi = dst->Addr - src->Addr;
		src_p += posi;
		src_len -= posi;

		// ����һ��ʣ���Խ�硣
		int remain = StreamRemian(&dstSt);
		return StreamWriteBytes(&dstSt, src_p, min(remain, src_len));
	}
}

// �� src �� bit ����д�� dst �ڡ�
int AreaBitCopy(Area_t* dst, Area_t* src)
{
	if (dst->Addr <= src->Addr)
	{
		// �� src ͷ����д�� dst ����λ�á�
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

/// <summary>������ռ����ݣ��ϲ������</summary>
/// <param name="dst">Ŀ��ռ�</param>
/// <param name="srcs">�ռ����飬���һ��� SIZE С�� 1</param>
/// <param name="isBit">�Ƿ���bit�ռ�</param>
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

/// <summary>�Ӷ���ռ��ȡ����</summary>
/// <param name="srcs">�����ռ�s</param>
/// <param name="addr">����ַ</param>
/// <param name="data">������</param>
/// <param name="len">������</param>
/// <param name="isBit">�Ƿ���bit�ռ�</param>
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

/// <summary>д����ռ����ݣ��Ӵ�ռ��С�ռ䣬����� fresh һ��</summary>
/// <param name="dsts">Ŀ��ռ����飬���һ��� SIZE С�� 1</param>
/// <param name="src">Դ</param>
/// <param name="isBit">�Ƿ���bit�ռ�</param>
void AreaWrite(Area_t* dsts, Area_t* src, bool isBit)
{
	if (src->Init)src->Init(src);

	while (true)
	{
		if (dsts->Size < 1)break;

		if (AreaCheck(dsts, src))
		{
			if (dsts->Init)dsts->Init(dsts);
			// �ֳ�ȥ
			if (isBit)AreaBitCopy(dsts, src);
			else AreaByteCopy(dsts, src);
			// ˢ��һ�£����磺ʹ������
			if (dsts->Fresh)dsts->Fresh(dsts);
		}

		dsts++;
	}
}

/// <summary>�����ռ�д����</summary>
/// <param name="dsts">��д��</param>
/// <param name="addr">д���ַ</param>
/// <param name="data">д������</param>
/// <param name="len">д�볤��</param>
/// <param name="isBit">�Ƿ���bit�ռ�</param>
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
