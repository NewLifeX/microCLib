#pragma once

#include "Type.h"

// #include "SysInfo.h"
/// <summary>����SysInfo����RS485</summary>
void Rs485Init(void);

/// <summary>RS232��������</summary>
/// <param name="data"></param>
/// <param name="len"></param>
void Rs485Send(byte* data, int len);

/// <summary>RS232��������</summary>
void Rs485Send2(Buffer_t* buff);

/// <summary>�첽�������ݡ�����RS485�ǰ�˫������������ֱ��ʹ��ͬ������</summary>
/// <param name=""></param>
bool AsyncSendData(void);
