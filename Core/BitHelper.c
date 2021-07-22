
#include "BitHelper.h"

/// <summary>获取数组中的bit值</summary>
/// <param name="idx">位置</param>
/// <param name="data">数据源</param>
/// <returns>1true,0false</returns>
__inline bool GetBit(byte* data, int idx)
{
	if (data == NULL)return false;
	if (idx < 0)return false;

	// byte dat = data[idx / 8];
	// byte mask = 0x01 << (idx % 8);
	byte dat = data[(idx >> 3)];
	byte mask = 0x01 << (idx & 0x07);

	if ((dat & mask) == 0x00)return false;
	return true;
}

/// <summary>设置bit</summary>
/// <param name="data"></param>
/// <param name="idx"></param>
/// <param name="value"></param>
__inline void SetBit(byte* data, int idx, bool value)
{
	if (data == NULL)return;
	if (idx < 0)return;

	// byte dat = data[idx / 8];
	// byte mask = 0x01 << (idx % 8);
	byte dat = data[(idx >> 3)];
	byte mask = 0x01 << (idx & 0x07);

	if (value)
	{
		dat |= mask;
	}
	else
	{
		dat &= ~mask;
	}

	data[(idx >> 3)] = dat;
}

/// <summary>字节bit倒序</summary>
byte BitReverse(byte data)
{
	byte res = 0;
	for (int i = 0; i < 8; i++)
	{
		byte mask = 0x01 << i;
		byte mask2 = 0x80 >> i;

		if ((data & mask) != 0)res |= mask2;
	}

	return res;
}

/// <summary>统计数组内所有bit 为1 的总数</summary>
int BitCount(byte* data, int len)
{
	if (data == NULL)return 0;
	if (len < 1)return 0;

	int cnt = 0;
	for (int i = 0; i < len; i++)
	{
		byte dat = data[i];

		if (dat == 0)continue;
		if (dat == 0xff) 
		{
			cnt += 8;
			continue;
		}

		for (int j = 0; j < 8; j++)
		{
			byte mask = 0x01 << j;

			if ((dat & mask) != 0)cnt++;
		}
	}

	return cnt;
}

