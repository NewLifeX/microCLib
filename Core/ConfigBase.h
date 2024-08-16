#pragma once

#include "Type.h"

// 需要在自己项目内补充。 ConfigBase_t 只提供读写保存。
#include "Config.h"

// Config 有效性 取决于 魔数和 Crc32
typedef struct
{
	// 魔数  内部使用。 外部不要碰
	// 默认为 ： 'M' 'a' 'g' '&'   0x4D616726
	uint MagicNum;
	// 实际配置长度。 
	// 为了做配置向后兼容。 必须有。
	uint Length;

	// 配置实际内容。
	Config_t Config;

	// 校验  内部使用。 外部不要碰
	uint Crc32;
}ConfigBase_t;


// 获取配置
bool GetConfig(Config_t* cfg, uint addr);

// 保存配置
bool SetConfig(Config_t* cfg, uint addr);

// 删除配置内容
bool CleanConfig(uint addr);

