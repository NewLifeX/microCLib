#pragma once

#include "Type.h"
#include "CircularQueue.h"

// modbus rtu 主机部分
#if 1

typedef struct
{
	/// <summary>发送</summary>
	void(*Send)(byte* data, int len);
	/// <summary>收数据</summary>
	CircularQueue_t* RxQueue;
	/// <summary>延迟（等待数据）</summary>
	void(*Delay)(int ms);
}RtuMst_t;

bool RtuWriteReg(RtuMst_t* thiss, ushort addr, ushort regaddr, ushort data, int timeout);
bool RtuWriteRegs(RtuMst_t* thiss, ushort addr, ushort regaddr, byte* data, int dlen, int timeout);


bool RtuReadReg(RtuMst_t* thiss, ushort addr, ushort regaddr, ushort* data, int timeout);
bool RtuReadRegs(RtuMst_t* thiss, ushort addr, ushort regaddr, byte* data, int dlen, int timeout);

#endif

/*
/// <summary>C41B 变频器启停</summary>
/// <param name="idx"></param>
/// <param name="value"></param>
void C41B_SetEnable(int idx, bool value);

/// <summary>设置速度</summary>
/// <param name="spd"></param>
void C41B_SetSpeed(ushort spd);
*/
