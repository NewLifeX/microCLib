#pragma once

#include "Type.h"
#include "Config.h"

typedef void(*CheckCfgFunc)(Config_t* cfg);

// 注册函数。
void CheckConfigRegister(CheckCfgFunc loaddDefault, CheckCfgFunc check);

// 检查配置是否正常
void CheckConfig(Config_t* cfg);

void LoadDefaultConfig(Config_t* cfg);
