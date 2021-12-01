#pragma once

#include "Type.h"
#include "HC165.h"
#include "HC595.h"

// 输入输出点状态
typedef struct
{
	HC595_t HC595;
	HC165_t HC165;

	byte Out[2];
	byte In[1];
}InOutInfo_t;
// 对外公开，方便直接访问 Out In 扩展其他功能
extern InOutInfo_t InOutInfo;

typedef enum
{
	/// <summary>前门</summary>
	Door = 0,
}InPointName_e;

typedef enum
{
	/// <summary>前门</summary>
	Door_Out = 0,
}OutPointName_e;

/// <summary>初始化输入输出口</summary>
void InOutPointInit(void);

/// <summary>输出点输出</summary>
/// <param name="idx">下标[0,7]</param>
/// <param name="value">值。开漏，输出漏为true </param>
/// <param name="flashOut">刷新输出</param>
void SetOutPoint(OutPointName_e idx, bool value, bool flashOut);

/// <summary>读输出状态</summary>
/// <param name="idx"></param>
/// <returns></returns>
bool GetOutPoint(OutPointName_e idx);

/// <summary>所有IO输出无效</summary>
/// <param name="flashOut">刷新输出</param>
void OutPointClear(bool flashOut);

/// <summary>刷新输出</summary>
void OutPointFlash(void);

/// <summary>读输入点</summary>
/// <param name="idx">下标[0,7]</param>
/// <returns>值。接地返回true </returns>
bool ReadInPoint(InPointName_e idx);

/// <summary>刷新输入状态</summary>
void InPointFlash(void);


/**************************************************/


/// <summary>状态指示灯输出</summary>
/// <param name="stat">8个灯一起</param>
/// <param name="flashOut">是否立即刷出</param>
void OutLedsStat(byte stat, bool flashOut);


