
#include "MyString.h"
#include "Array.h"

int StringIndexOf(char* src, char* sub)
{
	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	int srclen = strlen(src);
	int sublen = strlen(sub);
	if (srclen < sublen)return -1;

	return ArrayIndexOf((byte*)src, srclen, (byte*)sub, sublen);
}

int StringLastIndexOf(char* src, char* sub)
{
	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	int srclen = strlen(src);
	int sublen = strlen(sub);
	if (srclen < sublen)return -1;

	return ArrayLastIndexOf((byte*)src, srclen, (byte*)sub, sublen);
}

// 检索字符位置，出现 chars 中的一个就返回索引值。
int StringIndexOfChars(char* src, char* chars, int chlen)
{
	int len = strlen(src);

	for (int i = 0; i < len; i++)
	{
		char ch = src[i];

		for (int j = 0; j < chlen; j++)
		{
			if (ch == chars[j])return i;
		}
	}

	return -1;
}

int StringLineLen(char* str)
{
	if (str == NULL)return -1;
	int len = strlen(str);

	for (int i = 0; i < len; i++)
	{
		char data = str[i];

		if ((data == '\r') || (data == '\n'))return i;
	}

	return len;
}

int StringTrim(char* str, int len)
{
	if (str == NULL)return -1;

	for (int i = 0; i < len; i++)
	{
		char data = str[i];
		// 不等于这些，就返回。
		if (!((data == '\r') || (data == '\n') || (data == '\0')))return i;
	}

	return len;
}

// 返回真实剪切长度。
// 当 src	 没有换行符的时候,剪切全长;
// 参数不正确返回 -1;
// 当 dstlen 长度不足时，		返回 -2;
int StringCutLine(char* src, char* dst, int dstlen)
{
	if (src == NULL)return -1;
	// if (dst == NULL)return -1;
	if (dst == NULL)dstlen = 0x7fffffff;
	
	int linelen = StringLineLen(src);
	if (linelen + 1 > dstlen)return -2;

	if ((linelen > 0) && (dst != NULL)) memcpy(dst, src, linelen);
	dst[linelen] = '\0';

	char* str = src + linelen;
	int trimlen = StringTrim(str, strlen(str));

	return linelen + trimlen;
}

// 子字符串查找。返回最先出现的子字符串索引。
int StringFindStrs(char* src, char** subs, int subslen)
{
	int subidx = -1, posi = 0x7fffffff;

	for (int i = 0; i < subslen; i++)
	{
		int p = StringIndexOf(src, subs[i]);
		if (p == -1)p = 0x7fffffff;

		if (p < posi)
		{
			posi = p;
			subidx = i;
		}
	}

	// if (posi == 0x7fffffff)return -1;
	return subidx;
}

// src 是 chars 中的哪一个，chlen 是 chars 个数。
int CharIsChars(char src, char* chars, int chlen)
{
	for (int i = 0; i < chlen; i++)
	{
		if (src == chars[i])return i;
	}

	return -1;
}
