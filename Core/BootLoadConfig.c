
#include "BootLoadConfig.h"
#include "Flash.h"
#include "Crc.h"
#include "arm_compat.h"

#define DisableIrq() __disable_irq()
#define EnableIrq() __enable_irq()

// 获取 Config
bool BootLoadGetConfig(BootLoadConfig_t* cfg)
{
	uint addr = BootLoadFlashSize - GetFlashBlockSize(0) + GetChipStartAddr(0);

	BootLoadConfig_t* cf = (BootLoadConfig_t*)addr;

	// 魔数  默认为 ： 'M' 'a' 'g' '&'   0x4D616726
	if (cf->MagicNum != 0x4D616726)return false;

	// 计算校验
	uint crc = CaclcCRC32((byte*)cf, sizeof(BootLoadConfig_t) - sizeof(cf->Crc32));
	if (cf->Crc32 != crc)return false;

	memcpy(cfg, cf, sizeof(BootLoadConfig_t));
	return true;
}

bool BootLoadSetConfig(BootLoadConfig_t* cfg)
{
	cfg->MagicNum = 0x4D616726;

	// 计算校验
	uint crc = CaclcCRC32((byte*)cfg, sizeof(BootLoadConfig_t) - sizeof(cfg->Crc32));
	cfg->Crc32 = crc;

	uint addr = BootLoadFlashSize - GetFlashBlockSize(0) + GetChipStartAddr(0);

	const int tryCount = 3;
	bool stat = false;
	int count = 0;
	// 硬件要求不能被中断打断  否则会失败
	DisableIrq();
	while (true)
	{
		// 超出擦除失败次数
		if (count > tryCount)break;

		if (EraseBlock(addr, false))
		{
			stat = true;
			break;
		}

		count++;
	}
	EnableIrq();

	if (!stat)return stat;

	stat = false;
	count = 0;
	// 硬件要求不能被中断打断  否则会失败
	DisableIrq();
	while (true)
	{
		// 超出写入出错次数
		if (count > tryCount)break;

		if (WriteBlock(addr, (byte*)cfg, sizeof(BootLoadConfig_t), true))
		{
			stat = true;
			break;
		}
		count++;
	}
	EnableIrq();

	return stat;
}

bool BootLoadCleanConfig(void)
{
	// 硬件要求不能被中断打断  否则会失败
	DisableIrq();

	uint addr = BootLoadFlashSize - GetFlashBlockSize(0) + GetChipStartAddr(0);
	bool rs = EraseBlock(addr, false);

	EnableIrq();

	return rs;
}
