#pragma once

#include "Type.h"
#include "VirtualArea.h"
#include "CircularQueue.h"

typedef struct
{
	/// <summary>从站地址,小心这个是指针</summary>
	byte* SlaveAddr;
	/// <summary>位区域</summary>
	Area_t* BitAreas;
	/// <summary>字节区域，注意modbus寄存器是2字节</summary>
	Area_t* ByteAreas;
	/// <summary>发送函数，param 提供其他参数的可能，比如串口号</summary>
	void(*Send)(byte* data, int len, void* param);
}ModbusSlave_t;

/// <summary>MODBUS RTU 数据包处理</summary>
/// <param name="mrs"></param>
/// <param name="p"></param>
/// <param name="pklen"></param>
/// <param name="sendparam">mrs->Send 函数的第三个参数</param>
void ModbusRtuMsgProcess(const ModbusSlave_t* mrs, byte* p, int pklen, void* sendparam);

/// <summary>尝试处理 MODBUS RTU 数据</summary>
/// <param name="queue"></param>
/// <param name="mrs"></param>
/// <param name="sendparam">mrs->Send 函数的第三个参数</param>
void ModbusRtuTryProcess(CircularQueue_t* queue, const ModbusSlave_t* mrs, void* sendparam);

/// <summary>MODBUS TCP 数据包处理</summary>
/// <param name="mrs">注意：不使用 SlaveAddr 字段</param>
/// <param name="pk"></param>
/// <param name="pklen"></param>
/// <param name="sendparam">mrs->Send 函数的第三个参数</param>
void ModbusTcpMsgProcess(const ModbusSlave_t* mrs, byte* pk, int pklen, void* sendparam);

/// <summary>尝试处理 MODBUS TCP 数据</summary>
/// <param name="queue"></param>
/// <param name="mrs">注意：不使用 SlaveAddr 字段</param>
/// <param name="sendparam">mrs->Send 函数的第三个参数</param>
void ModbusTcpTryProcess(CircularQueue_t* queue, const ModbusSlave_t* mrs, void* sendparam);
