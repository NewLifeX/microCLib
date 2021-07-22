#pragma once

#include "Type.h"
#include "BootLoadConfig.h"

// dst 必须是第一 chip。 
// 返回是否成功。
bool CopyBin(uint dst, uint src, int size);

// 检测新APP 是否满足升级条件
bool CheckNewApp(BootLoadConfig_t* cfg);

// 检测和升级。
bool CheckAddUpdate(BootLoadConfig_t* cfg);

// BootLoad 主要逻辑。
void BootLoadMain(void);

/*  BootLoad 工程实例：

// 工程Main函数只需要配置一下其他信息 然后调用 BootLoadMain() 即可
int main(void)
{
	DebugInit();
	// NVIC_SetPriorityGroup(4);

	BootLoadMain();
}

*/

// APP 会用到的函数。

// 保存 BootLoad 升级信息。
bool SaveUpdateConfig(uint appSaveAddr, uint appLoadAddr, int appSize, uint64 version);
