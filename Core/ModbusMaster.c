
#include "ModbusMaster.h"

#include "Type.h"
#include "ModbusRtuCodec.h"
#include "ModbusTcpCodec.h"

#include "CircularQueue.h"
#include "Debug.h"

#include "SysHelper.h"
#define FILE_ID  (2)

// modbus rtu 主机部分
#if 1

bool RtuWriteReg(RtuMst_t* thiss, ushort addr, ushort regaddr, ushort data, int timeout)
{
	dTraceThiss(FILE_ID);

	byte cache[64];
	int txlen = Mrc05a06(addr, 6, regaddr, data, cache, sizeof(cache));

	CircularQueue_t* queue = (thiss->RxQueue);
	CircularQueueClear(queue);

	// ShowArray("Rtu Send ",cache, txlen, txlen);
	thiss->Send(cache, txlen);

	while (true)
	{
		thiss->Delay(1);
		timeout--;

		int pklen = MrcMasterGetRxLenCircularQueue(queue);
		if (pklen > 0)
		{
			CircularQueueReads(queue, cache, pklen, false);
			// ShowArray("Rtu Recv ", cache, pklen, pklen);
			if (cache[0] != addr)continue;

			return true;
		}
		if (pklen < 0)
		{
			CircularQueueSeek(queue, 1);
		}

		if (timeout < 1)return false;
	}
}

bool RtuWriteRegs(RtuMst_t* thiss, ushort addr, ushort regaddr, byte* data, int dlen, int timeout)
{
	dTraceThiss(FILE_ID);

	byte cache[130];
	int txlen = Mrc10(addr, regaddr, data, (dlen + 1) / 2, cache, sizeof(cache));

	CircularQueue_t* queue = (thiss->RxQueue);

	CircularQueueClear(queue);
	thiss->Send(cache, txlen);

	while (true)
	{
		thiss->Delay(1);
		timeout--;

		int pklen = MrcMasterGetRxLenCircularQueue(queue);
		if (pklen > 0)
		{
			CircularQueueReads(queue, cache, pklen, false);
			if (cache[0] != addr)continue;

			return true;
		}
		if (pklen < 0)
		{
			CircularQueueSeek(queue, 1);
		}

		if (timeout < 1)return false;
	}
}


bool RtuReadReg(RtuMst_t* thiss, ushort addr, ushort regaddr, ushort* data, int timeout)
{
	dTraceThiss(FILE_ID);

	byte cache[64];
	int txlen = Mrc03a04(addr, 4, regaddr, 1, cache, sizeof(cache));

	CircularQueue_t* queue = (thiss->RxQueue);

	CircularQueueClear(queue);
	thiss->Send(cache, txlen);

	while (true)
	{
		thiss->Delay(1);

		int pklen = MrcMasterGetRxLenCircularQueue(queue);
		if (pklen > 0)
		{
			CircularQueueReads(queue, cache, pklen, false);			
			if (cache[0] != addr)continue;
			if (cache[1] != 4)continue;

			int of = MrcMasterGetRxPyOffset(cache);
			memcpy((byte*)data, cache + of, 2);
			return true;
		}
		if (pklen < 0)
		{
			// 出错，去掉一个字节，往后找
			CircularQueueSeek(queue, 1);
		}

		timeout--;
		if (timeout < 1)return false;
	}
}

bool RtuReadRegs(RtuMst_t* thiss, ushort addr, ushort regaddr, byte* data, int dlen, int timeout)
{
	dTraceThiss(FILE_ID);

	byte cache[130];
	int txlen = Mrc03a04(addr, 3, regaddr, (dlen + 1) / 2, cache, sizeof(cache));

	CircularQueue_t* queue = (thiss->RxQueue);

	CircularQueueClear(queue);
	thiss->Send(cache, txlen);

	while (true)
	{
		thiss->Delay(1);

		int pklen = MrcMasterGetRxLenCircularQueue(queue);
		if (pklen > 0)
		{
			CircularQueueReads(queue, cache, pklen, false);
			if (cache[0] != addr)continue;
			if (cache[1] != 3)continue;

			int of = MrcMasterGetRxPyOffset(cache);
			memcpy(data, cache + of, dlen);
			return true;
		}
		if (pklen < 0)
		{
			// 出错，去掉一个字节，往后找
			CircularQueueSeek(queue, 1);
		}

		timeout--;
		if (timeout < 1)return false;
	}
}

#endif


// 另外一个项目的代码，仅供参考。
#if 0

#include "RS485_2.h"
static RtuMst_t RtuMst = { 0 };
static void _DelayMs(int ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

/// <summary>执行动作</summary>
/// <param name="devAddr"></param>
/// <param name="value">启停</param>
/// <param name="speed">速度</param>
/// <returns></returns>
bool _C41B_Act(int devAddr, bool value, ushort speed)
{
	dTraceThiss(FILE_ID);

	if (value)
	{
		// 清异常
		if (!RtuWriteReg(&RtuMst, devAddr, 0x2007, 0x0001, 100))return false;
		vTaskDelay(pdMS_TO_TICKS(5));
		// 设置速度
		if (!RtuWriteReg(&RtuMst, devAddr, 0x2002, speed, 100))return false;
		vTaskDelay(pdMS_TO_TICKS(5));
	}

	// 启停
	bool result = RtuWriteReg(&RtuMst, devAddr, 0x2001, value ? 0x0001 : 0x0000, 100);
	// 加个延迟，避免总线繁忙
	if (result) vTaskDelay(pdMS_TO_TICKS(5));
	return result;
}

/// <summary>读电机电流</summary>
/// <param name="devAddr"></param>
/// <returns>0-300 之间，-1 未读到</returns>
int _C41B_Is(int devAddr)
{
	ushort Is = 0;
	if (!RtuReadReg(&RtuMst, devAddr, 0x2084, &Is, 100))return -1;

	return Is;
}

/// <summary>读电机速度</summary>
/// <param name="devAddr"></param>
/// <returns></returns>
bool _C41B_Speed(int devAddr, short* spd)
{
	short speed = 0;
	if (!RtuReadReg(&RtuMst, devAddr, 0x2081, (ushort*)&speed, 100))return false;

	if (spd != NULL)*spd = speed;
	return true;
}

#include "BitHelper.h"
#include "SysInfo.h"

/// <summary>设备状态，根据这个状态进行状态跟踪</summary>
static ushort _Enables = 0;

#define __DEF_SPEED 1500
/// <summary>电机默认速度</summary>
static ushort _Speed = __DEF_SPEED;

void _C41B_Task(void* param)
{
	static byte rxbuff[64];
	CircularQueueInit(&Rs485Info2.Rx, rxbuff, sizeof(rxbuff));

	Rs485_2Init(115200);
	Rs485_2Open();

	RtuMst.Send = Rs485_2Send2;
	RtuMst.RxQueue = &Rs485Info2.Rx;
	RtuMst.Delay = _DelayMs;

	while (true)
	{
		if (SysInfo.RunState == RS_EStop)
		{
			DebugPrintf("_C41B_Task EStop\r\n");
			for (int i = 0; i < 9; i++)
			{
				int devAddr = i + 3;
				_C41B_Act(devAddr, false, _Speed);
			}

			vTaskSuspend(NULL);
		}
		else
		{
			ushort _ens = _Enables;
			for (int i = 0; i < 9; i++)
			{
				vTaskDelay(pdMS_TO_TICKS(50));

				// 目标状态
				bool en = GetBit((byte*)&_Enables, i);
				int devAddr = i + 3;

				for (int j = 0; j < 5; j++)
				{
					// // 电流
					// short currspeed = 0;
					// if (!_C41B_Speed(devAddr, &currspeed))
					// {
					// 	DebugPrintf("Read C41B_Speed %d Addr %d false\r\n", i, devAddr);
					// 	continue;
					// }

					// 当前状态
					// bool run = (currspeed != 0);

					// 修改过去
					// if (run != en)
					{
						// 成功就不尝试了
						if (_C41B_Act(devAddr, en, _Speed))
						{
							DebugPrintf("_C41B_Act(%d,%s,%d) true\r\n", devAddr, en ? "true" : "false", _Speed);
							break;
						}
					}
				}
			}

			if (_ens == _Enables)vTaskSuspend(NULL);
		}
	}
}

static TaskHandle_t _TaskHandle = NULL;
void C41B_SetEnable(int idx, bool value)
{
	if (idx > 9)return;

	bool old = GetBit((byte*)&_Enables, idx);
	SetBit((byte*)&_Enables, idx, value);

	if (_TaskHandle == NULL)
	{
		DebugPrintf("Create _C41B_Task\r\n");
		xTaskCreate(_C41B_Task, "C41B_Task", 256, NULL, 4, &_TaskHandle);
		return;
	}

	// 没有变化。
	if (old == value)return;
	if (_TaskHandle != NULL)vTaskResume(_TaskHandle);
	DebugPrintf("C41B_SetEnable %d %s\r\n", idx, value ? "true" : "false");
}

void C41B_SetSpeed(ushort spd)
{
	if (spd < 500)spd = __DEF_SPEED;
	if (spd > 3000)spd = __DEF_SPEED;

	_Speed = spd;
}

void C41B_EStop(ushort idx)
{
	if (idx >= 9)return;

	ushort devAddr = idx + 3;
	RtuWriteReg(&RtuMst, devAddr, 0x2001, 0x0000, 100);
}

#include "IO.h"

void C41B_AllOff(void)
{
	if (SysInfo.HdConfig.DevType != Line_PreSort)return;
	vTaskSuspend(_TaskHandle);

	for (int i = 0; i < 9; i++)
	{
		C41B_EStop(i);
	}
}

#endif
