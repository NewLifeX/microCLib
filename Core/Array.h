#pragma once

#include "Type.h"

// 返回第一个符合的位置
// 没有找到返回 -1
int ArrayIndexOf(byte* src, int srclen, byte* sub, int sublen);
int ArrayIndexOf_Byte(byte* src, int len, byte by);

int ArrayIndexOf_Offset(byte* src, int srclen, byte* sub, int sublen, int offset);
int ArrayIndexOf_Byte_Offset(byte* src, int srclen, byte by, int offset);

int ArrayIndexOfBuff(Buffer_t* src,Buffer_t* sub);
int ArrayIndexOfBuff2(Buffer2_t* src, Buffer2_t* sub);

// 从尾部匹配
// 返回从前往后的下标。
int ArrayLastIndexOf(byte* src, int srclen, byte* sub, int sublen);
int ArrayLastIndexOfBuff(Buffer_t* src, Buffer_t* sub);
int ArrayLastIndexOfBuff2(Buffer2_t* src, Buffer2_t* sub);

// 倒序
void ArrayReverse(byte* data, int len);
void ArrayReverseBuff(Buffer_t* buff);
void ArrayReverseBuff2(Buffer2_t* buff);

// 排序 从小到大 内部使用冒泡排序
void ArraySortByte(byte* data, int len);
void ArraySortUint(uint* data, int len);
void ArraySortInt(int* data, int len);

// 其他
// copy 直接用 c 库


// 对比数组，相同返回true
bool ArrayEqual(byte* b1, byte* b2, int len);

// 数组转字符串，16进制
int ArrayToHex(byte* bs, int len, char* hex);
// len =>  hex.length 
int HexToArray(char* hex, int len, byte* bs);


/// <summary>二分法查询</summary>
/// <param name="array">从小到大排序的数组</param>
/// <param name="len">数组长度</param>
/// <param name="value">查询值</param>
/// <returns>返回第一个有效值。 -1 未找到</returns>
int DichotomyFind_Int(int* array, int len, int value);



/// <summary>提取KEY。二分法查询结构体时使用</summary>
typedef int(*DichotomyGetKey)(void* array, int idx);

/// <summary>二分法，从结构体查询</summary>
/// <param name="array">结构体数组，按照KEY从小到大排序</param>
/// <param name="len">结构体长度</param>
/// <param name="func">获取KEY方法</param>
/// <param name="key">查询项</param>
/// <returns>返回第一个有效值。 -1 未找到</returns>
int DichotomyFind_Obj(void* array, int len, DichotomyGetKey func, int key);

/*
* 二分法 从结构体查询  用法样例
typedef struct
{
	int key;
	int value;
}Item_t;

// DichotomyGetKey 类型的函数
int ItemGetKey(void* array, int idx)
{
	Item_t* arr = (Item_t*)array;
	return arr[idx].key;
}

int Test(void)
{
	Item_t Arr[50];

	// 要查询的值
	int findKey = 50;

	int idx = DichotomyFind_Obj(Arr, ArrayLength(Arr), ItemGetKey, findKey);
	return idx;
}
*
*/




