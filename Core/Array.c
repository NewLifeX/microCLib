
#include "Array.h"

int ArrayIndexOf(byte* src, int srclen, byte* sub, int sublen)
{
	if (srclen < 1)return -1;
	if (sublen < 1)return -1;
	if (srclen < sublen)return -1;

	if (src == NULL)return -1;
	if (sub == NULL)return -1;

	int i, j;
	// ƥ���һ���ֽڵ��������ֽ�����
	int maxsearch = srclen - sublen + 1;
	for (i = 0; i < maxsearch; i++)
	{
		// �ҵ���һ����ͬ�ֽ�
		if (sub[0] == src[i])
		{
			// ƥ������ֽ�
			for (j = 1; j < sublen; j++)
			{
				if (sub[j] != src[i + j]) break;
			}

			// ƥ��ɹ�
			if (j == sublen) return i;

			// ƥ��ʧ��������ҵ�һ���ֽڡ�
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
		// �ҵ���һ����ͬ�ֽ�
		if (sub[0] == src[i])
		{
			// ƥ������ֽ�
			for (j = 1; j < sublen; j++)
			{
				if (sub[j] != src[i + j]) break;
			}

			// ƥ��ɹ�
			if (j == sublen) return i;

			// ƥ��ʧ��������ҵ�һ���ֽڡ�
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
	// ����
	int count = len / 2;
	// ����
	byte temp;
	// ͷβ
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

