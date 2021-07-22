#pragma once

#include "Type.h"
// #include "FreeRTOS.h"

// 配置BitMap的内存管理函数。
// 此内存管理函数只用于位图本身 MapAddr 的申请和释放。不对被管理对象进行操作。
// 全局操作中途不可修改。
// 使用前必须配置。
void BitMapSetMemFunc(MallocFunc ma, FreeFunc fr);

// 内存管理内部不处理 管理对象的正确与否。只标记分片是否被填充过。
// 最大管理内存为 65535 * grainsize
typedef struct
{
	// 管理对象
	byte* MemAddr;
	uint MemLen;

	// 分片大小
	int FragmentSize;

	// bitmap 对象
	byte* MapAddr;
	// bitmap 占用内存大小  BitMapByteSize = BitCount/8   (+ 1)
	ushort BitMapByteSize;
	// 有效 bit个数  BitCount = MemLen/MapAddr  (+ 1)
	ushort BitCount;

	// 保留头部，SetData  GetData 对其不操作。
	ushort ReservedHeadSize;
}BitmapMlme_t;

// 初始化位图管理器
// mem 需要管理的内存起始地址
// len 需要管理的内存起始大小
// fragmentSize   bitmap 一个bit标记的内存大小
// reservedHeadSize   保留头部，SetData  GetData 对其不操作。
typedef bool (*BitmapMlmeInit_f)(BitmapMlme_t* mlme, byte* mem, int len, int fragmentSize, ushort reservedHeadSize);

// 向 bitmap 填充数据。
// mlme		this
// bitIdx	位编号 下标，从0开始。  最大 mlme->BitCount -1;
// data		数据
// 数据长度在 mlme 内部有。  自动判断末尾分片大小。
typedef bool (*BitmapMlmeSetData_f)(BitmapMlme_t* mlme, ushort bitIdx, byte* data);

// 拿数据。  不判断数据是否有效
// mlme		this
// bitIdx	位编号 下标，从0开始。  最大 mlme->BitCount -1;
// data		数据
// 返回实际拿到数据长度
// 自动判断末尾分片大小。
typedef int (*BitmapMlmeGetData_f)(BitmapMlme_t* mlme, ushort bitIdx, byte* buff, int bufflen);

// 检测某位是否有效。
typedef bool (*BitmapMlmeCheck_f)(BitmapMlme_t* mlme, ushort bitIdx);

// 获取无效编号的数组
// mlme			this
// bitStart		获取bitmap的起始index
// buff			数组的内存缓冲区
// bufflen		array的大小         字节数组！，  内部保存的item 是ushort。
// 返回获取到的数组长度。 小于1 表示没有空分片。   返回的是字节数组的长度。
typedef int (*BitmapMlmeGetEmptyIdxArray_f)(BitmapMlme_t* mlme, ushort bitStart, byte* buff, int bufflen);

// 算是析构吧。
typedef void(*BitmapMlmeDeinit_f)(BitmapMlme_t* mlme);

typedef struct
{
	BitmapMlmeInit_f Init;
	BitmapMlmeSetData_f SetData;
	BitmapMlmeGetData_f GetData;
	BitmapMlmeCheck_f Check;
	BitmapMlmeGetEmptyIdxArray_f GetEmptyIdxArray;
	BitmapMlmeDeinit_f DeInit;
}BitmapMlme_f;

extern BitmapMlme_f const BitmapMlme;


#define BITMAPMLMEMEMDEBUG

// 测试例程
void BitmapMlmeTest(void* param);

