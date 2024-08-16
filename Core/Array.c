
#include "Array.h"

int ArrayIndexOf(byte* src, int srclen, byte* sub, int sublen)
{
	if (srclen < 1)return -1;
	if (sublen < 1)return -1;
	if (srclen < sublen)return -1;

	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	int i, j;
	// 匹配第一个字节的最大可能字节数。
	int maxsearch = srclen - sublen + 1;
	for (i = 0; i < maxsearch; i++)
	{
		// 找到第一个相同字节
		if (sub[0] == src[i])
		{
			// 匹配后续字节
			for (j = 1; j < sublen; j++)
			{
				if (sub[j] != src[i + j]) break;
			}

			// 匹配成功
			if (j == sublen) return i;

			// 匹配失败则继续找第一个字节。
		}
	}

	return -1;
}

int ArrayIndexOf_Byte(byte* src, int len, byte by)
{
	if (src == NULL)return -1;
	if (len < 1)return -1;

	for (int i = 0; i < len; i++)
	{
		if (src[i] == by)return i;
	}

	return -1;
}

int ArrayIndexOf_Offset(byte* src, int srclen, byte* sub, int sublen, int offset)
{
	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	src += offset;
	srclen -= offset;

	if (srclen < 1)return -1;
	if (sublen < 1)return -1;
	if (srclen < sublen)return -1;

	int idx = ArrayIndexOf(src, srclen, sub, sublen);
	if (idx < 0)return -1;

	return idx + offset;
}

int ArrayIndexOf_Byte_Offset(byte* src, int len, byte by, int offset)
{
	src += offset;
	len -= offset;

	if (src == NULL)return -1;
	if (len < 1)return -1;

	for (int i = 0; i < len; i++)
	{
		if (src[i] == by)return i + offset;
	}

	return -1;
}

int ArrayIndexOfBuff(Buffer_t* src, Buffer_t* sub)
{
	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	return ArrayIndexOf(src->p, src->len, sub->p, sub->len);
}

int ArrayIndexOfBuff2(Buffer2_t* src, Buffer2_t* sub)
{
	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	return ArrayIndexOf(src->data, src->len, sub->data, sub->len);
}

int ArrayLastIndexOf(byte* src, int srclen, byte* sub, int sublen)
{
	if (srclen < 1)return -1;
	if (sublen < 1)return -1;
	if (srclen < sublen)return -1;

	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	int i, j;
	int maxsearch = srclen - sublen + 1;
	for (i = maxsearch; i >= 0; i--)
	{
		// 找到第一个相同字节
		if (sub[0] == src[i])
		{
			// 匹配后续字节
			for (j = 1; j < sublen; j++)
			{
				if (sub[j] != src[i + j]) break;
			}

			// 匹配成功
			if (j == sublen) return i;

			// 匹配失败则继续找第一个字节。
		}
	}

	return -1;
}

int ArrayLastIndexOfBuff(Buffer_t* src, Buffer_t* sub)
{
	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	return ArrayLastIndexOf(src->p, src->len, sub->p, sub->len);
}

int ArrayLastIndexOfBuff2(Buffer2_t* src, Buffer2_t* sub)
{
	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	return ArrayLastIndexOf(src->data, src->len, sub->data, sub->len);
}

void ArrayReverse(byte* data, int len)
{
	if (data == NULL)return;
	if (len < 2)return;
	// 计数
	int count = len / 2;
	// 缓存
	byte temp;
	// 头尾
	byte* head = data;
	byte* end = data + len - 1;
	for (int i = 0; i < count; i++)
	{
		temp = *head;
		*head = *end;
		*end = temp;

		++head;
		--end;
	}
}

void ArrayReverseBuff(Buffer_t* buff)
{
	if (buff == NULL)return;

	ArrayReverse(buff->p, buff->len);
}

void ArrayReverseBuff2(Buffer2_t* buff)
{
	if (buff == NULL)return;

	ArrayReverse(buff->data, buff->len);
}

void ArraySortByte(byte* data, int len)
{
	for (int i = 0; i < len - 1; i++)
	{
		for (int j = 0; j < len - 1 - i; j++)
		{
			if (data[j] > data[j + 1])
			{
				byte temp = data[j + 1];
				data[j + 1] = data[j];
				data[j] = temp;
			}
		}
	}
}

void ArraySortUint(uint* data, int len)
{
	for (int i = 0; i < len - 1; i++)
	{
		for (int j = 0; j < len - 1 - i; j++)
		{
			if (data[j] > data[j + 1])
			{
				uint temp = data[j + 1];
				data[j + 1] = data[j];
				data[j] = temp;
			}
		}
	}
}

void ArraySortInt(int* data, int len)
{
	for (int i = 0; i < len - 1; i++)
	{
		for (int j = 0; j < len - 1 - i; j++)
		{
			if (data[j] > data[j + 1])
			{
				uint temp = data[j + 1];
				data[j + 1] = data[j];
				data[j] = temp;
			}
		}
	}
}

bool ArrayEqual(byte* b1, byte* b2, int len)
{
	if (b1 == NULL)return false;
	if (b2 == NULL)return false;
	if (len < 1)return false;

	for (int i = 0; i < len; i++)
	{
		if (b1[i] != b2[i])return false;
	}

	return true;
}

static inline char tohex(byte by)
{
	if (by > 9)return by - 10 + 'A';
	return by + '0';
}

int ArrayToHex(byte* bs, int len, char* hex)
{
	if (bs == NULL)return 0;
	if (hex == NULL)return 0;

	for (int i = 0; i < len; i++)
	{
		*hex++ = tohex(bs[i] >> 4);
		*hex++ = tohex(bs[i] & 0x0f);
	}

	return len * 2;
}

static inline byte toby(char ch)
{
	if ((ch >= 'a') && (ch <= 'f'))return ch - 'A' + 10;
	if ((ch >= 'A') && (ch <= 'F'))return ch - 'A' + 10;
	if ((ch >= '0') && (ch <= '9'))return ch - '0';

	return 0xff;
}

int HexToArray(char* hex, int len, byte* bs)
{
	if (hex == NULL)return 0;
	if (bs == NULL)return 0;
	len /= 2;

	for (int i = 0; i < len; i++)
	{
		byte h = toby(hex[i * 2]);
		byte l = toby(hex[i * 2 + 1]);

		if (h == 0xff)return 0;
		if (l == 0xff)return 0;

		bs[i] = (h << 4) + l;
	}

	return len;
}

/// <summary>二分法查询</summary>
/// <param name="array">从小到大排序的数组</param>
/// <param name="len">数组长度</param>
/// <param name="value">查询值</param>
/// <returns>返回第一个有效值。 -1 未找到</returns>
int DichotomyFind_Int(int* array, int len, int value)
{
	if (array == NULL)return -1;
	if (len == 0)return -1;
	if (len == 1)return array[0] == value ? 0 : -1;

	int head = 0;
	int tail = len - 1;

	while (true)
	{
		int half = (head + tail) / 2;
		int p = array[half];

		if (value == p)
		{
			return half;
		}
		else if (value < p)
		{
			tail = half - 1;
		}
		else
		{
			head = half + 1;
		}

		if (head > tail)return -1;
	}
}

/// <summary>二分法，从结构体查询</summary>
/// <param name="array">结构体数组，按照KEY从小到大排序</param>
/// <param name="len">结构体长度</param>
/// <param name="func">获取KEY方法</param>
/// <param name="key">查询项</param>
/// <returns>返回第一个有效值。 -1 未找到</returns>
int DichotomyFind_Obj(void* array, int len, DichotomyGetKey func, int key)
{
	if (array == NULL)return -1;
	if (len == 0)return -1;
	if (len == 1)return func(array,0) == key ? 0 : -1;

	int head = 0;
	int tail = len - 1;

	while (true)
	{
		int half = (head + tail) / 2;
		int p = func(array, half);

		if (key == p)
		{
			return half;
		}
		else if (key < p)
		{
			tail = half - 1;
		}
		else
		{
			head = half + 1;
		}

		if (head > tail)return -1;
	}
}
