#pragma once

#include "Type.h"

// 必须设定为1字节对齐，否则offsetof会得到错误的位置
#pragma pack(push)	// 保存对齐状态
// 强制结构体紧凑分配空间
#pragma pack(1)

typedef struct
{
	// 魔数  内部使用。 外部不要碰
	uint MagicNum;

	// 当前需要启动的应用地址
	uint CurrentAppLoadAddr;
	// 当前应用版本。一般使用2000年至今的秒数。
	uint CurrentVersion;

	// 新APP的存放地址。
	uint NewAppSaveAddr;
	// 新APP的大小。单位字节
	uint NewAppSize;
	// 新APP的CRC32校验值
	uint NewAppCrc32;
	// 新应用升级到Flash什么位置。 
	// 升级完成会Copy到 CurrentAppLoadAddr
	uint NewAppLoadAddr;
	// 新应用的版本。一般使用2000年至今的秒数。
	// 升级完成会Copy到 CurrentVersion
	uint NewAppVersion;

	// 校验  内部使用。 外部不要碰
	uint Crc32;
}BootLoadConfig_t;

#pragma pack(pop)	// 恢复对齐状态

// 32KBYTE 的 BootLoad 大小。配置放在此区间的最后一个 Block
// #define BootLoadFlashSize	(32*1024)
// #define BootLoadFlashSize	(16*1024)
#define BootLoadFlashSize	(8*1024)

// Config 默认放在最后一个Flash块内
// 魔数  默认为 ： 'M' 'a' 'g' '&'   0x4D616726
// Config 有效性 取决于 魔数和 Crc32

// 获取配置
bool BootLoadGetConfig(BootLoadConfig_t* cfg);

// 保存配置
bool BootLoadSetConfig(BootLoadConfig_t* cfg);

// 删除配置内容
bool BootLoadCleanConfig(void);

