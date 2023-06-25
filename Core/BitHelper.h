#pragma once

#include "Type.h"

/// <summary>获取数组中的bit值</summary>
/// <param name="idx">位置</param>
/// <param name="data">数据源</param>
/// <returns>1 true,0 false</returns>
bool GetBit(byte* data, int idx);

/// <summary>设置bit</summary>
/// <param name="idx">位置</param>
/// <param name="value">设定值</param>
void SetBit(byte* data, int idx, bool value);

/// <summary>设置多bit</summary>
/// <param name="dst">目标</param>
/// <param name="dst_offset">目标偏移量</param>
/// <param name="src">源</param>
/// <param name="src_offset">源偏移量</param>
/// <param name="bits">需要set的数量</param>
void SetBits(byte* dst, int dst_offset, byte* src, int src_offset, int bits);

/// <summary>字节bit倒序</summary>
byte BitReverse(byte data);

/// <summary>统计数组内所有bit 为1 的总数</summary>
int BitCount(byte* data, int len);
