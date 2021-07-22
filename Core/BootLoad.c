
#include "BootLoad.h"
#include "BootLoadConfig.h"

#include "Flash.h"
#include "Crc.h"
#include "Cpu.h"
#include "Debug.h"

// dst 必须是第一 chip。 
// 返回是否成功。
bool CopyBin(uint dst, uint src, int size)
{
	int bk = GetFlashBlockSize(0);

	while (true)
	{
		EraseBlock(dst, false);
		WriteBlock(dst, (byte*)src, bk, false);

		dst += bk;
		src += bk;
		size -= bk;

		if (size < bk)break;
	}

	EraseBlock(dst, false);
	WriteBlock(dst, (byte*)src, size, true);

	return true;
}

// 检测新APP 是否满足升级条件
bool CheckNewApp(BootLoadConfig_t* cfg)
{
	// 校验数量
	if (cfg->NewAppSize < 1024)return false;
	if (cfg->NewAppSize > (GetFlashSize() / 2))return false;

	// 检测新APP存放位置是否合法
	if ((cfg->NewAppSaveAddr + cfg->NewAppSize > GetFlashSize() + GetChipStartAddr(0))
		|| (cfg->NewAppSaveAddr < GetChipStartAddr(0)))
	{
		DebugPrintf("CheckNewApp Save Error\r\n");
		return false;
	}

	// 校验版本
	// if (cfg->NewAppVersion <= cfg->CurrentVersion)return false;

	// 校验 CRC
	uint crc = CaclcCRC32((byte*)cfg->NewAppSaveAddr, cfg->NewAppSize);
	if (crc != cfg->NewAppCrc32)
	{
		DebugPrintf("CheckNewApp NewAppCrc32 Error\r\n");
		return false;
	}

	return true;
}

// 检测和升级。
bool CheckAddUpdate(BootLoadConfig_t* cfg)
{
	// 检测
	if (!CheckNewApp(cfg)) return false;

	// DebugPrintf("Found a new version of software in flash\r\n");
	DebugPrintf("Ready for upgrade\r\n");

	// 开始升级过程
	bool isOk = false;
	for (int i = 0; i < 32; i++)
	{
		CopyBin(cfg->NewAppLoadAddr, cfg->NewAppSaveAddr, cfg->NewAppSize);

		uint crc = CaclcCRC32((byte*)cfg->NewAppLoadAddr, cfg->NewAppSize);
		if (crc == cfg->NewAppCrc32)
		{
			isOk = true;
			break;
		}
	}

	// 处理升级结果。
	if (!isOk)
	{
		DebugPrintf("Flash is corrupt and cannot complete upgrade\r\n");

		// 已经覆盖了 Flash 原有App已经不能用了。
		cfg->CurrentAppLoadAddr = 0x00000000;

		for (int i = 0; i < 3; i++)
		{
			if (BootLoadSetConfig(cfg))break;
		}
	}
	else
	{
		// 搬运和校验完成。
		DebugPrintf("Upgrade completed\r\n");

		// 更新当前APP信息。
		cfg->CurrentVersion = cfg->NewAppVersion;
		cfg->CurrentAppLoadAddr = cfg->NewAppLoadAddr;
		// 销毁新APP信息。
		cfg->NewAppCrc32 = 0;
		cfg->NewAppSaveAddr = 0;
		cfg->NewAppLoadAddr = 0;
		cfg->NewAppVersion = 0;

		// DebugPrintf("Update configuration information\r\n");

		// 配置信息存盘
		for (int i = 0; i < 3; i++)
		{
			if (BootLoadSetConfig(cfg))break;
		}
	}

	return isOk;
}

// BootLoad 主要逻辑。
void BootLoadMain(void)
{
	// 配置信息。
	BootLoadConfig_t Cfg;

	if (!BootLoadGetConfig(&Cfg))
	{
		DebugPrintf("BootLoadGetConfig Fail, Load default\r\n");

		// 清零
		memset(&Cfg, 0, sizeof(Cfg));

		// 获取失败。应该是第一次启动。 这里装载默认配置。
		Cfg.CurrentAppLoadAddr = BootLoadFlashSize + GetChipStartAddr(0);
		Cfg.CurrentVersion = 0;

		DebugPrintf("\tCurrent App Addr    : 0x%08X\r\n", Cfg.CurrentAppLoadAddr);
		DebugPrintf("\tCurrent App Version : 0x%08X\r\n",Cfg.CurrentVersion);
		// DebugPrintf("\tCurrent App Version : 0x");
		// uint* p = (uint*)&Cfg.CurrentVersion;
		// DebugPrintf("%08X", p[1]);
		// DebugPrintf("%08X\r\n", p[0]);

		// 启动
		Boot(Cfg.CurrentAppLoadAddr);
	}
	else
	{
		DebugPrintf("BootLoadGetConfig OK\r\n");
		DebugPrintf("Cur App Address	: 0x%08X\r\n", Cfg.CurrentAppLoadAddr);
		DebugPrintf("Cur App Version	: 0x%08X\r\n", Cfg.CurrentVersion);

		DebugPrintf("New App Version	: 0x%08X\r\n",Cfg.NewAppVersion);

		// 获取成功
		// 尝试升级
		CheckAddUpdate(&Cfg);

		// 启动
		Boot(Cfg.CurrentAppLoadAddr);
	}
}

// 保存 BootLoad 升级信息。
bool SaveUpdateConfig(uint appSaveAddr,uint appLoadAddr, int appSize,uint64 version)
{
	// 配置信息。
	BootLoadConfig_t Cfg;

	if (!BootLoadGetConfig(&Cfg))
	{
		// 清零
		memset(&Cfg, 0, sizeof(Cfg));

		// 获取失败。应该是第一次启动。 这里装载默认配置。
		Cfg.CurrentAppLoadAddr = BootLoadFlashSize + GetChipStartAddr(0);
		Cfg.CurrentVersion = 0;
	}

	// 计算 CRC 存盘。
	uint crc = CaclcCRC32((byte*)appSaveAddr, appSize);

	// 销毁新APP信息。
	Cfg.NewAppCrc32 = crc;
	Cfg.NewAppSaveAddr = appSaveAddr;
	Cfg.NewAppLoadAddr = appLoadAddr;
	Cfg.NewAppVersion = version;

	// 配置信息存盘
	return BootLoadSetConfig(&Cfg);
}

