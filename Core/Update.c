
#include "Update.h"
#ifdef DEBUG
#include "Debug.h"
#else
#define ErrPrintf( ... )
#define WarningPrintf( ... )
#define DebugPrintf( ... )
#define TraceThis( ... )
#endif

#include "Flash.h"
#include "Cpu.h"

#include "Crc.h"
#include "BootLoadConfig.h"

bool DoUpdate(UpdateBinInfo_t* info, uint saveAddr, byte* bin)
{
	// 最多尝试写3次
	for (int i = 0; i < 3; i++)
	{
		// 批量写 Flash 。 暂时只支持写第一片Flash。 
		// 不校验saveaddr合法性。
		if (FlashSaveBin(saveAddr, bin, info->Size))break;
	}

	// 校验Flash内的bin是否正确。
	uint flashcrc = CaclcCRC32B((byte*)saveAddr, info->Size);
	if (flashcrc != info->Crc32)return false;

	BootLoadConfig_t cfg;
	if (!BootLoadGetConfig(&cfg))
	{
		DebugPrintf("BootLoadGetConfig Fail\r\nLoadDefault:");
		memset(&cfg, 0x00, sizeof(cfg));

		// memcpy(cfg.Name, SysInfo.Config.Name, strlen(SysInfo.Config.Name));
		cfg.CurrentAppLoadAddr = 0x8000000 + BootLoadFlashSize;
		cfg.CurrentVersion = 0;
	}

	cfg.NewAppSaveAddr = saveAddr;
	cfg.NewAppSize = info->Size;
	cfg.NewAppCrc32 = info->Crc32;
	cfg.NewAppLoadAddr = info->LoadAddr;
	cfg.NewAppVersion = info->Version;

	if (!BootLoadSetConfig(&cfg))
	{
		DebugPrintf("BootLoadSetConfig Fail\r\n");
		return false;
	}

	DebugPrintf("BootLoadSetConfig OK\r\n");
	return true;
}
