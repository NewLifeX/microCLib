#pragma once

#include "Type.h"

// ���ص�һ�����ϵ�λ��
// û���ҵ����� -1
int ArrayIndexOf(byte* src, int srclen, byte* sub, int sublen);
int ArrayIndexOf_Offset(byte* src, int srclen, byte* sub, int sublen, int offset);
int ArrayIndexOfBuff(Buffer_t* src,Buffer_t* sub);
int ArrayIndexOfBuff2(Buffer2_t* src, Buffer2_t* sub);

// ��β��ƥ��
// ���ش�ǰ������±ꡣ
int ArrayLastIndexOf(byte* src, int srclen, byte* sub, int sublen);
int ArrayLastIndexOfBuff(Buffer_t* src, Buffer_t* sub);
int ArrayLastIndexOfBuff2(Buffer2_t* src, Buffer2_t* sub);

// ����
void ArrayReverse(byte* data, int len);
void ArrayReverseBuff(Buffer_t* buff);
void ArrayReverseBuff2(Buffer2_t* buff);

// ���� ��С���� �ڲ�ʹ��ð������
void ArraySortByte(byte* data, int len);
void ArraySortUint(uint* data, int len);
void ArraySortInt(int* data, int len);

// ����
// copy ֱ���� c ��
