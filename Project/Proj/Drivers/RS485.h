#pragma once

#include "Type.h"

// #include "SysInfo.h"
/// <summary>根据SysInfo配置RS485</summary>
void Rs485Init(void);

/// <summary>RS232发送数据</summary>
/// <param name="data"></param>
/// <param name="len"></param>
void Rs485Send(byte* data, int len);

/// <summary>RS232发送数据</summary>
void Rs485Send2(Buffer_t* buff);

/// <summary>异步发送数据。由于RS485是半双工，所以这里直接使用同步发送</summary>
/// <param name=""></param>
bool AsyncSendData(void);
