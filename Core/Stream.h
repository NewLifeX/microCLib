#pragma once

#include "Type.h"

typedef struct
{
	// 内存空间
	byte* MemStart;

	// 游标位置，下标。
	// 游标永远指向下一个待读待写的位置
	int Position;
	// 内存容量 = MemEnd - MemStart
	int Size;
}Stream_t;

typedef enum
{
	// 起始位置
	SeekBegin,
	// 末尾位置
	SeekEnd,
	// 当前位置
	SeekCurrent,
}SeekOrigin_e;

void StreamInit(Stream_t* st, byte* p, int len);

int StreamWriteByte(Stream_t* st, byte data);
int StreamWriteBytes(Stream_t* st, byte* p, int len);

int StreamReadByte(Stream_t* st,byte* p);
int StreamReadBytes(Stream_t* st, byte* p, int len);

// 获取空闲空间
int StreamRemian(Stream_t* st);
// 返回当前游标
int StreamSeek(Stream_t* st, int sek, SeekOrigin_e of);

// 自动扩容的 Steam 函数 API。
// 默认初始化的时候自动分配 128 字节。后续扩容按照 128 字节增加。
// 扩容的内存来自 MemOptions_t* opt。 可以指定长度。
void AutoStreamInit(Stream_t* st,int len, MemOptions_t* opt);
void AutoStreamDeInit(Stream_t* st, MemOptions_t* opt);

// 手动扩容。
// len 是扩容后的大小。
bool AutoStreamSetCapacity(Stream_t* st, int len, MemOptions_t* opt);

int AutoStreamWriteByte(Stream_t* st, byte data, MemOptions_t* opt);
int AutoStreamWriteBytes(Stream_t* st, byte* p, int len, MemOptions_t* opt);
