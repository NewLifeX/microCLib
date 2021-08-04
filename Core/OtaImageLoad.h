#pragma once

#include "Flash.h"
#include "BootLoadConfig.h"


#include "HardwareInfo.h"


/// <summary>д��Image��������</summary>
/// <param name="offset">�̼�offset</param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <returns>����д���Ƿ�ɹ�</returns>
bool OtaImageSet(uint offset, byte* data, int len);

/// <summary>У��CRC</summary>
/// <param name="offset"></param>
/// <param name="len"></param>
/// <returns></returns>
uint OtaImageCrc(uint offset, int len);

/// <summary>У��̼��Ƿ�����</summary>
/// <param name="offset"></param>
/// <param name="len"></param>
/// <param name="crc2"></param>
/// <returns></returns>
bool OtaImageCrcCheck(uint offset, int len, uint crc2);

/// <summary>����ota��Ϣ����������ǰ�Ķ���</summary>
/// <param name="size">��app��С</param>
/// <param name="crc">��app crc</param>
/// <param name="version">�汾������0</param>
/// <returns></returns>
bool OtaUpdateImageInfo(int size, uint crc, uint version);
