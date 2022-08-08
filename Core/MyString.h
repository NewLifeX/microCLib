#pragma once

#include "Type.h"

// 查找位置
int StringIndexOf(char* src, char* sub);
int StringLastIndexOf(char* src, char* sub);
// 检索字符位置，出现 chars 中的一个就返回索引值。chlen 是 chars 个数。
int StringIndexOfChars(char* src, char* chars, int chlen);

// 统计到 \r \n 结束。
// 如果没有这些，就是字符串全长。
int StringLineLen(char* str);

// 返回 Trim 的长度。
// Trim 统计头部 \r \n \0 字符的个数。
// 因为 \0 是字符的结尾。 此处不能用此值作为依据！！
int StringTrim(char* str,int len);

// 返回真实剪切长度。
// 当 src 没有换行符的时候,剪切全长;
// 剪切长度是包含剪切内容和后面的换行符的，\r \n \0 都算。
// 剪切出来的内容是不含换行的。
// 允许 dst 为NULL，表示丢弃。
// 参数不正确返回 -1;
// 当 dstlen 长度不足时，		返回 -2;
int StringCutLine(char* src, char* dst, int dstlen);

// 子字符串查找。返回最先出现的子字符串索引。
int StringFindStrs(char* src, char** subs, int subslen);

// src 是 chars 中的哪一个，chlen 是 chars 个数。
int CharIsChars(char src, char* chars, int chlen);

// 比较字符串是否相等
bool StringCmp(char* s1, char* s2);

