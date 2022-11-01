
#include "ConfigBase.h"
#include "Flash.h"
#include "Crc.h"

#define DisableIrq() __disable_irq()
#define EnableIrq() __enable_irq()

// 获取 Config
bool GetConfig(Config_t* cfg,uint addr)
{
	// uint addr = GetChipSize(0) - GetFlashBlockSize(0) + GetChipStartAddr(0);
	ConfigBase_t* cf = (ConfigBase_t*)addr;

	// 魔数  默认为 ： 'M' 'a' 'g' '&'   0x4D616726
	if (cf->MagicNum != 0x4D616726)return false;

	// 计算校验
	uint crc = CaclcCRC32B((byte*)cf, sizeof(ConfigBase_t) - sizeof(cf->Crc32));
	if (cf->Crc32 != crc)return false;

	memcpy(cfg, (byte*)&cf->Config, cf->Length);

	// 判断是否完全兼容, 不要求强制升级好了。
	// 让用的人自己去区分。
	// if (sizeof(Config_t) != cf->Length)
	// {
	// 	CleanConfig();
	// }

	return true;
}

bool SetConfig(Config_t* cfg2, uint addr)
{
	// uint addr = GetChipSize(0) - GetFlashBlockSize(0) + GetChipStartAddr(0);
	// 硬件要求不能被中断打断  否则会失败
	DisableIrq();

	ConfigBase_t base;
	base.MagicNum = 0x4D616726;
	base.Length = sizeof(Config_t);

	memcpy((byte*)&base.Config, (byte*)cfg2, sizeof(Config_t));

	// 计算校验
	uint crc = CaclcCRC32B((byte*)&base, sizeof(ConfigBase_t) - sizeof(base.Crc32));
	base.Crc32 = crc;

	const int tryCount = 3;
	bool stat = false;
	int count = 0;
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

		if (WriteBlock(addr, (byte*)&base, sizeof(ConfigBase_t), true))
		{
			stat = true;
			break;
		}
		count++;
	}
	EnableIrq();

	return stat;
}

bool CleanConfig(uint addr)
{
	// 硬件要求不能被中断打断  否则会失败
	DisableIrq();

	// uint addr = GetChipSize(0) - GetFlashBlockSize(0) + GetChipStartAddr(0);
	bool rs = EraseBlock(addr, false);

	EnableIrq();

	return rs;
}
