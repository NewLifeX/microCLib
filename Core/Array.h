#pragma once

#include "Type.h"

// 返回第一个符合的位置
// 没有找到返回 -1
int ArrayIndexOf(byte* src, int srclen, byte* sub, int sublen);
int ArrayIndexOf_Offset(byte* src, int srclen, byte* sub, int sublen, int offset);
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
