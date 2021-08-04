#pragma once

#include "Flash.h"
#include "BootLoadConfig.h"


#include "HardwareInfo.h"


/// <summary>写入Image到储存区</summary>
/// <param name="offset">固件offset</param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <returns>返回写入是否成功</returns>
bool OtaImageSet(uint offset, byte* data, int len);

/// <summary>校验CRC</summary>
/// <param name="offset"></param>
/// <param name="len"></param>
/// <returns></returns>
uint OtaImageCrc(uint offset, int len);

/// <summary>校验固件是否正常</summary>
/// <param name="offset"></param>
/// <param name="len"></param>
/// <param name="crc2"></param>
/// <returns></returns>
bool OtaImageCrcCheck(uint offset, int len, uint crc2);

/// <summary>更新ota信息，启动升级前的动作</summary>
/// <param name="size">新app大小</param>
/// <param name="crc">新app crc</param>
/// <param name="version">版本，可填0</param>
/// <returns></returns>
bool OtaUpdateImageInfo(int size, uint crc, uint version);
