#pragma once

#include "Type.h"
#include "VirtualArea.h"
#include "CircularQueue.h"

typedef struct
{
	/// <summary>��վ��ַ,С�������ָ��</summary>
	byte* SlaveAddr;
	/// <summary>λ����</summary>
	Area_t* BitAreas;
	/// <summary>�ֽ�����ע��modbus�Ĵ�����2�ֽ�</summary>
	Area_t* ByteAreas;
	/// <summary>���ͺ�����param �ṩ���������Ŀ��ܣ����紮�ں�</summary>
	void(*Send)(byte* data, int len, void* param);
}ModbusSlave_t;

/// <summary>MODBUS RTU ���ݰ�����</summary>
/// <param name="mrs"></param>
/// <param name="p"></param>
/// <param name="pklen"></param>
/// <param name="sendparam">mrs->Send �����ĵ���������</param>
void ModbusRtuMsgProcess(const ModbusSlave_t* mrs, byte* p, int pklen, void* sendparam);

/// <summary>���Դ��� MODBUS RTU ����</summary>
/// <param name="queue"></param>
/// <param name="mrs"></param>
/// <param name="sendparam">mrs->Send �����ĵ���������</param>
void ModbusRtuTryProcess(CircularQueue_t* queue, const ModbusSlave_t* mrs, void* sendparam);

/// <summary>MODBUS TCP ���ݰ�����</summary>
/// <param name="mrs">ע�⣺��ʹ�� SlaveAddr �ֶ�</param>
/// <param name="pk"></param>
/// <param name="pklen"></param>
/// <param name="sendparam">mrs->Send �����ĵ���������</param>
void ModbusTcpMsgProcess(const ModbusSlave_t* mrs, byte* pk, int pklen, void* sendparam);

/// <summary>���Դ��� MODBUS TCP ����</summary>
/// <param name="queue"></param>
/// <param name="mrs">ע�⣺��ʹ�� SlaveAddr �ֶ�</param>
/// <param name="sendparam">mrs->Send �����ĵ���������</param>
void ModbusTcpTryProcess(CircularQueue_t* queue, const ModbusSlave_t* mrs, void* sendparam);
