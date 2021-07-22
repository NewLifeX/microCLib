
#include "BitmapMlmeMem.h"
#include "Debug.h"

// 初始化位图管理器
// mem 需要管理的内存起始地址
// len 需要管理的内存起始大小
// fragmentSize   bitmap 一个bit标记的内存大小
// reservedHeadSize   保留头部，SetData  GetData 对其不操作。
bool BitmapMlmeInit(BitmapMlme_t* mlme, byte* mem, int len, int fragmentSize, ushort reservedHeadSize);

// 向 bitmap 填充数据。
// mlme		this
// bitIdx	位编号 下标，从0开始。  最大 mlme->BitCount -1;
// data		数据
// 数据长度在 mlme 内部有。  自动判断末尾分片大小。
bool BitmapMlmeSetData(BitmapMlme_t* mlme, ushort bitIdx, byte* data);

// 拿数据。  不判断数据是否有效
// mlme		this
// bitIdx	位编号 下标，从0开始。  最大 mlme->BitCount -1;
// data		数据
// 返回实际拿到数据长度
// 自动判断末尾分片大小。
int BitmapMlmeGetData(BitmapMlme_t* mlme, ushort bitIdx, byte* buff, int bufflen);

// 检测某位是否有效。
bool BitmapMlmeCheck(BitmapMlme_t* mlme, ushort bitIdx);

// 获取无效编号的数组
// mlme			this
// bitStart		获取bitmap的起始index
// array		数组的内存缓冲区
// arraylen		array的大小
// 返回获取到的数组长度。 小于1 表示没有空分片。
int BitmapMlmeGetEmptyIdx(BitmapMlme_t* mlme, ushort bitStart, byte* buff, int bufflen);

// 算是析构吧。
void BitmapMlmeDeinit(BitmapMlme_t* mlme);

// 内存管理函数
static MallocFunc BitMapMalloc = NULL;
static FreeFunc BitMapFree = NULL;

void BitMapSetMemFunc(MallocFunc ma, FreeFunc fr)
{
	if (ma == NULL)return;
	if (fr == NULL)return;

	if ((BitMapMalloc != NULL) || (BitMapFree != NULL))
	{
		DebugPrintf("BitMapSetMemFunc() BitMapMalloc  BitMapFree");
		DebugPrintf(" 只能配置一次。");
		return;
	}

	BitMapMalloc = ma;
	BitMapFree = fr;
}

BitmapMlme_f const BitmapMlme =
{
	.Init = BitmapMlmeInit,
	.SetData = BitmapMlmeSetData,
	.GetData = BitmapMlmeGetData,
	.Check = BitmapMlmeCheck,
	.GetEmptyIdxArray = BitmapMlmeGetEmptyIdx,
	.DeInit = BitmapMlmeDeinit,
};

// 初始化内存管理
bool BitmapMlmeInit(BitmapMlme_t* mlme, byte* mem, int len, int fragmentSize, ushort reservedHeadSize)
{
	if (BitMapMalloc == NULL)ErrPrintf("Please configure BitMapMalloc & BitMapFree first.Use BitMapSetMemFunc().");

	mlme->MapAddr = NULL;
	if (mlme == NULL) { DebugPrintf("BitmapMlmeInit mlme == NULL error\r\n"); return false; }
	if (reservedHeadSize > fragmentSize) { DebugPrintf("BitmapMlmeInit reservedHeadSize > fragmentSize  error\r\n"); return false; }

	mlme->MemAddr = mem;
	mlme->MemLen = len;
	mlme->FragmentSize = fragmentSize;
	mlme->ReservedHeadSize = reservedHeadSize;

	// 计算 bitmap 大小
	uint bitcount = len / fragmentSize;
	if (len % fragmentSize != 0)bitcount += 1;
	if (bitcount > 0xffff)
	{
		DebugPrintf("BitmapMlmeInit mem Too big or grainsize too small\r\n");
		return false;
	}

	mlme->BitCount = bitcount;

	// uint mapsize = bitcount / 8;
	// if (bitcount % 8 != 0)mapsize += 1;
	// 上面两行的优化处理：
	uint mapsize = bitcount >> 3;
	if ((bitcount & 0x7) != 0x00)mapsize += 1;

	mlme->BitMapByteSize = mapsize;

	byte* p = (byte*)BitMapMalloc(mapsize);
	if (p == NULL)
	{
		DebugPrintf("BitmapMlmeInit malloc(map) Error\r\n");
		return false;
	}
	// DebugPrintf("MapAddr Addr 0x%08X\r\n",(uint)p);

	// 清空。
	memset(p, 0x00, mapsize);

	mlme->MapAddr = p;

	// DebugPrintf("BitmapMlmeInit mem %08X size %d\r\n", mlme->MemAddr, mlme->MemLen);
	return true;
}

bool BitmapMlmeSetData(BitmapMlme_t* mlme, ushort bitIdx, byte* data)
{
	if (mlme == NULL) { DebugPrintf("BitmapMlmeSetData mlme = NULL\r\n"); return false; }
	if (data == NULL) { DebugPrintf("BitmapMlmeSetData data = NULL\r\n"); return false; }
	if (mlme->MapAddr == NULL)return false;

	if (bitIdx >= mlme->BitCount)
	{
		DebugPrintf("BitmapMlmeSetData bitIdx %d > %d too big\r\n", bitIdx, mlme->BitCount);
		return false;
	}

	// 除以8  就是右移3位
	byte* mapitem_p = &mlme->MapAddr[bitIdx >> 3];
	// 取余8就是&0x7
	byte itemmask = 0x01 << (bitIdx & 0x7);

	// 内存位置
	byte* mem = mlme->MemAddr + (bitIdx * mlme->FragmentSize);
	int len = mlme->FragmentSize;

	// DebugPrintf("MlmeSetData mem %08X size %d\r\n", mem, len);

	// copy内存数据
	if (bitIdx == 0)
	{
		mem += mlme->ReservedHeadSize;
		len -= mlme->ReservedHeadSize;

		memcpy(mem, data + mlme->ReservedHeadSize, len);
	}
	else if (bitIdx == (mlme->BitCount - 1))
	{
		// 最后一个分片大小需要处理一下。
		len = mlme->MemLen - (mlme->FragmentSize * bitIdx);
		memcpy(mem, data, len);
	}
	else
	{
		memcpy(mem, data, len);
	}

	// 标记
	(*mapitem_p) |= itemmask;

	return true;
}

int BitmapMlmeGetData(BitmapMlme_t* mlme, ushort bitIdx, byte* buff, int bufflen)
{
	if (mlme == NULL) { DebugPrintf("BitmapMlmeGetData mlme = NULL\r\n"); return -1; }
	if (buff == NULL) { DebugPrintf("BitmapMlmeGetData buff = NULL\r\n"); return -1; }
	if (mlme->MemAddr == NULL) { DebugPrintf("BitmapMlmeGetData MemAddr = NULL\r\n"); return -1; }
	if (bitIdx >= mlme->BitCount) { DebugPrintf("BitmapMlmeGetData bitIdx too big\r\n"); return -1; }

	// 内存位置
	byte* mem = mlme->MemAddr + (bitIdx * mlme->FragmentSize);
	int len = mlme->FragmentSize;
	byte* dstmem = buff;

	if (bitIdx == 0)
	{
		// 提取的长度需要满足要求
		if (len > bufflen)len = bufflen;

		mem += mlme->ReservedHeadSize;
		len -= mlme->ReservedHeadSize;

		dstmem += mlme->ReservedHeadSize;
	}
	else if (bitIdx == (mlme->BitCount - 1))
	{
		// 最后一个分片大小需要处理一下。
		len = mlme->MemLen - (mlme->FragmentSize * bitIdx);

		// 提取的长度需要满足要求
		if (len > bufflen)len = bufflen;
	}
	else
	{
		// 提取的长度需要满足要求
		if (len > bufflen)len = bufflen;
	}

	// copy 数据
	memcpy(dstmem, mem, len);
	return len;
}

bool BitmapMlmeCheck(BitmapMlme_t* mlme, ushort bitIdx)
{
	if (mlme == NULL)return false;
	if (mlme->MapAddr == NULL)return false;
	if (bitIdx >= (mlme->BitCount))return false;

	// 除以8  就是右移3位
	byte* mapitem_p = &mlme->MapAddr[bitIdx >> 3];
	// 取余8就是&0x7
	byte itemmask = 0x01 << (bitIdx & 0x7);

	// 判断是否置1
	return ((*mapitem_p) & itemmask != 0x00);
}

int BitmapMlmeGetEmptyIdx(BitmapMlme_t* mlme, ushort bitStart, byte* buff, int bufflen)
{
	if (mlme == NULL) { DebugPrintf("BitmapMlmeGetEmptyIdx mlme = NULL\r\n"); return -1; }
	if (buff == NULL) { DebugPrintf("BitmapMlmeGetEmptyIdx buff = NULL\r\n"); return -1; }
	if (mlme->MemAddr == NULL) { DebugPrintf("BitmapMlmeGetEmptyIdx MemAddr = NULL\r\n"); return -1; }

	// 这些变量直接拿到，编译时候直接放在R1 R2..  不需要内存寻址，加快速度。
	ushort* ar = (ushort*)buff;
	int arlen = bufflen / 2;
	int aridx = 0;
	int bitcount = mlme->BitCount;

	for (int i = bitStart >> 3; i < mlme->BitMapByteSize; i++)
	{
		// 位图里面的字节
		byte mapitem = mlme->MapAddr[i];
		// 字节起始bit的index
		ushort idxbase = i * 8;
		for (int j = 0; j < 8; j++)
		{
			byte itemmask = 0x01 << j;
			if ((mapitem & itemmask) == 0)
			{
				ushort idx = idxbase + j;

				// 判定界限。
				if (idx < bitStart)continue;
				if (idx >= bitcount)break;

				// 填充
				ar[aridx] = idx;
				aridx++;

				// 判断是否满了。
				if (aridx >= arlen)break;
			}
		}

		// 判断是否满了。
		if (aridx >= arlen)break;
	}

	// 返回的是 字节数组的使用量。
	return aridx * 2;
}

void BitmapMlmeDeinit(BitmapMlme_t* mlme)
{
	if (mlme == NULL) { DebugPrintf("BitmapMlmeDeinit mlme = NULL\r\n"); return; }

	mlme->FragmentSize = 0;
	if (mlme->MapAddr == NULL)
	{
		DebugPrintf("BitmapMlmeDeinit MapAddr = NULL\r\n");
		return;
	}

	BitMapFree(mlme->MapAddr);
	mlme->MapAddr = NULL;

	mlme->BitMapByteSize = 0;
	mlme->BitCount = 0;
}


#ifdef BITMAPMLMEMEMDEBUG

void BitmapMlmeTest(void* param)
{
	DebugPrintf("BitmapMlmeTest\r\n");

	int memsize = 2033;
	byte* mem = (byte*)BitMapMalloc(memsize);
	if (mem == NULL)
	{
		DebugPrintf("BitMapMalloc %d Error\r\n\r\n\r\n", memsize);
		return;
	}
	DebugPrintf("BitMapMalloc 0x%08X\r\n", mem);

	BitmapMlme_t mlme;
	BitmapMlme.Init(&mlme, mem, memsize, 10, 0);

	byte test[32];
	for (int i = 0; i < sizeof(test); i++) test[i] = '0' + (i % 10);
	test[31] = '\0';

	int lastidx = mlme.BitCount - 1;

	DebugPrintf("SetData %s\r\n", test);
	// DebugPrintf("%s");
	BitmapMlme.SetData(&mlme, 0, test);
	BitmapMlme.SetData(&mlme, lastidx / 2, test);
	BitmapMlme.SetData(&mlme, lastidx, test);

	for (int i = 0; i < sizeof(test); i++) test[i] = 0;
	int rs = 0;
	rs = BitmapMlme.GetData(&mlme, 0, test, sizeof(test));
	test[rs - 1] = '\0';
	DebugPrintf("Get %d fragment Data %d %s\r\n", 0, rs, test);


	for (int i = 0; i < sizeof(test); i++) test[i] = 0;
	rs = BitmapMlme.GetData(&mlme, lastidx / 2, test, sizeof(test));
	test[rs - 1] = '\0';
	DebugPrintf("Get %d fragment Data %d  %s\r\n", lastidx / 2, rs, test);


	for (int i = 0; i < sizeof(test); i++) test[i] = 0;
	rs = BitmapMlme.GetData(&mlme, lastidx, test, sizeof(test));
	test[rs - 1] = '\0';
	DebugPrintf("Get %d fragment Data %d  %s\r\n", lastidx, rs, test);


	BitmapMlme.DeInit(&mlme);

	BitMapFree(mem);
}

#else

void BitmapMlmeTest(void* param) { }

#endif
