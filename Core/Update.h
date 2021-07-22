#pragma once

#include "Type.h"

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	bool isGw;
	uint LoadAddr;
	uint Version;
	uint Size;
	uint Crc32;
}UpdateBinInfo_t;

#pragma pack(pop)

// 升级逻辑： 将bin保存到特定位置。
// 并更新 BootLoadConfig 描述信息。然后重启。
// 由 BootLoad 将 bin 放到 LoadAddr。并引导启动。


// 升级。
// bin 的长度已经在 info 里面有。
// saveAddr  bin 保存的 Flash 地址
// 一般是 完成之后，修改一些系统配置，然后重启。
bool DoUpdate(UpdateBinInfo_t* info, uint saveAddr, byte* bin);

