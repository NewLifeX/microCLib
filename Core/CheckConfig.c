
#include "CheckConfig.h"
#include "Type.h"
#include "Debug.h"

static CheckCfgFunc LoadDefFunc[16];
static CheckCfgFunc CheckFunc[16];

// 注册函数。
void CheckConfigRegister(CheckCfgFunc loadDefault, CheckCfgFunc check)
{
	static bool init = false;
	if (!init)
	{
		memset(LoadDefFunc, NULL, sizeof(LoadDefFunc));
		memset(CheckFunc, NULL, sizeof(CheckFunc));
		init = true;
	}

	bool added = false;

	for (int i = 0; i < sizeof(LoadDefFunc) / sizeof(LoadDefFunc[0]); i++)
	{
		if (LoadDefFunc[i] == loadDefault)
		{
			added = true;
			break;
		}
	}
	if (!added)
	{
		for (int i = 0; i < sizeof(LoadDefFunc) / sizeof(LoadDefFunc[0]); i++)
		{
			if (LoadDefFunc[i] == NULL)
			{
				LoadDefFunc[i] = loadDefault;
				added = true;
				break;
			}
		}
	}
	if (added == false) ErrPrintf("CheckConfigRegister added = false");

	added = false;

	for (int i = 0; i < sizeof(CheckFunc) / sizeof(CheckFunc[0]); i++)
	{
		if (CheckFunc[i] == check)
		{
			added = true;
			break;
		}
	}
	if (!added)
	{
		for (int i = 0; i < sizeof(CheckFunc) / sizeof(CheckFunc[0]); i++)
		{
			if (CheckFunc[i] == NULL)
			{
				CheckFunc[i] = check;
				added = true;
				break;
			}
		}
	}

	if (added == false) ErrPrintf("CheckConfigRegister added = false");
}

void CheckConfig(Config_t* cfg)
{
	for (int i = 0; i < sizeof(CheckFunc) / sizeof(CheckFunc[0]); i++)
	{
		if (CheckFunc[i] != NULL)CheckFunc[i](cfg);
	}
}

void LoadDefaultConfig(Config_t* cfg)
{
	for (int i = 0; i < sizeof(LoadDefFunc) / sizeof(LoadDefFunc[0]); i++)
	{
		if (LoadDefFunc[i] != NULL)LoadDefFunc[i](cfg);
	}
}
