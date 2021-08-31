#ifndef __TYPE_H__
#define __TYPE_H__

typedef char					sbyte;
typedef unsigned char			byte;
typedef unsigned short			ushort;
typedef unsigned int			uint;
typedef long long int			int64;
typedef unsigned long long int	uint64;

// 系统时间使用64bit  （32bit不够用）
typedef uint64 Time_t;

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <cType.h>
// #include <math.h>

// 没有参数和返回值的委托
typedef void(*Func)(void);
// 一个参数没有返回值的委托，一般param参数用作目标对象，调用者用静态函数包装成员函数
typedef void(*Action)(void* param);
typedef void(*Action2)(void*, void*);
typedef void(*Action3)(void*, void*, void*);
// 事件处理委托，一般sender表示事件发出者，param用作目标对象，调用者用静态函数包装成员函数
typedef void(*EventHandler)(void* sender, void* param);

// malloc free 函数类型定义。
typedef void* (*MallocFunc)(size_t size);
typedef void (*FreeFunc)(void* p);

// 全局的 malloc free 函数。 
// 用于用系统和不用系统时候的切换。避免需要修改太多东西。
// 需要手动实现。
extern MallocFunc	GlobleMalloc;
extern FreeFunc		GlobleFree;

typedef struct
{
	MallocFunc malloc;
	FreeFunc free;
}MemOptions_t;

// 缓冲区
typedef struct
{
	byte* p;
	int len;
	bool needFree;
	FreeFunc free;
}Buffer_t;

// 缓冲区, 用于从特定内存中抽取一部分数据。
typedef struct
{
	// 数据指针
	byte* data;
	// 数据长度
	uint len;

	// 是否需要 Free
	bool needFree;
	// Free 函数指针
	FreeFunc free;
	// Free 函数测参数  data在这其中。
	byte* freePoint;
}Buffer2_t;

void InitBuffer(Buffer_t* data);
void InitBuffer2(Buffer2_t* data);

void TryCleanBuffer(Buffer_t* data);
void TryCleanBuffer2(Buffer2_t* data);

void ShowBuff(Buffer_t* buff, int len);
void ShowBuff2(Buffer2_t* buff, int len);

/// <summary>打印数组内容</summary>
/// <param name="head">数组名称</param>
/// <param name="pdata">数组指针</param>
/// <param name="datalen">数组长度</param>
/// <param name="showlen">打印长度</param>
void ShowArray(char* name, byte* pdata, int datalen, int showlen);
// 数组长度
#define ArrayLength(arr) ((int)(sizeof(arr)/sizeof(arr[0])))

// 大小端转换
#define __REV16x(x) ((ushort)((x >> 8) & 0x00ff) | ((x << 8) & 0xff00))

// 大小端切换
static uint __REV32x(uint x)
{
	uint y = 0;
	y = (x << 24) & 0xff000000;
	y |= (x << 8) & 0xff0000;
	y |= (x >> 8) & 0xff00;
	y |= (x >> 24) & 0xff;

	return y;
}

// 计算结构体内变量的内存偏移地址。
// 为了避免与c库冲突，末尾添加 2.
// 来源于linux、uboot源码
#define offsetof2(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

// 通过结构体成员指针获取结构体的指针。
// 成员 ptr 地址
// 结构体名称
// 成员名称
// 返回结构体指针。
// 来源于linux、uboot源码
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof2(type,member) );})

#endif
