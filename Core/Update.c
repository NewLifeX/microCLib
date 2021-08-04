
#include "Update.h"
#include "Debug.h"

#include "Flash.h"
#include "Cpu.h"

#include "Crc.h"
#include "BootLoadConfig.h"


bool DoUpdate(UpdateBinInfo_t* info, uint saveAddr, byte* bin)
{
	// ��ೢ��д3��
	for (int i = 0; i < 3; i++)
	{
		// ����д Flash �� ��ʱֻ֧��д��һƬFlash�� 
		// ��У��saveaddr�Ϸ��ԡ�
		if (FlashSaveBin(saveAddr, bin, info->Size))break;
	}

	// У��Flash�ڵ�bin�Ƿ���ȷ��
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
