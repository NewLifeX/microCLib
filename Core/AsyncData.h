#pragma once

#include "Type.h"

// 异步数据。用于缓冲消息，记录消息内容。
// 包含获取缓冲区，获取缓冲消息，清空缓冲内容 等方法。

typedef struct
{
	// 缓冲区头位置，也是数据头位置
	byte* Data;
	// 数据长度
	int Datalen;
	// 缓冲区真实长度
	int BuffLen;

	// 异步时间
	Time_t AsyncTime;

	// 是否需要 Free
	bool needFree;
	// Free 函数指针
	FreeFunc free;
}AsyncData_t;

/// <summary>初始化结构体</summary>
void InitAsyncData(AsyncData_t* adt);

/// <summary>初始化结构体+建立缓冲区</summary>
/// <param name="adt">结构体指针</param>
/// <param name="len">缓冲区大小</param>
/// <returns>是否成功</returns>
bool InitAsyncData2(AsyncData_t* adt, int len);

/// <summary>释放缓冲区</summary>
void TryFreeAsyncData(AsyncData_t* adt);

/// <summary>移除数据</summary>
void RemoveAsyncData(AsyncData_t* adt);

/// <summary>获取异步数据内容</summary>
int GetAsyncData(AsyncData_t* adt, byte** data);

/// <summary>将数据copy到其中，如果data本身就是其中的缓冲区本身，将不copy</summary>
/// <param name="adt"></param>
/// <param name="data">需要进去的数据</param>
/// <param name="len">需要push的长度</param>
/// <param name="atime">异步时间</param>
/// <returns>返回实际push长度</returns>
int PushAsyncData(AsyncData_t* adt, byte* data, int len , Time_t atime);
