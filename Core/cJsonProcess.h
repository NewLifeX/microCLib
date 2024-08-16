#pragma once

#include "Type.h"
#include "cJSON.h"

// 解析消息框架。
// 消息格式：
// {"action":"xxx", .....}

// 整体思路，序列化消息与业务逻辑分开。
// 使用方式见本文件末。

// 业务逻辑处理
typedef void(*MsgProcess)(void* param);
// 序列化消息并回调业务逻辑处理函数。
typedef bool(*JsonProcess)(cJSON* json, MsgProcess func);

typedef struct
{
	// Action 名称
	char* ActionName;
	// Json 处理函数
	JsonProcess JsonProcess;
	// Json 处理函数回调的 二进制处理函数。
	MsgProcess Process;
}cJsonProcess_t;

// {"action":"xxx", .....} 消息处理分发。
// ProcessArray 最后一项必须 ActionName = NULL 。 否则跑飞。
bool JsonProcessMsg(Buffer2_t* buff, cJsonProcess_t* ProcessArray);

/*
使用方式

--------------------------------------------------------------
// JsonToBin.c		专门处理序列化。 JSON <-> Bin

/// <summary>Open命令序列化成二进制形式</summary>
bool OpenJson(cJSON* json, MsgProcess func)
{
	openparam_t param
	// json to param
	...

	// 回调业务逻辑
	func(&param);

	return true;
}

// 序列化返回消息
/// <summary>创建返回消息</summary>
/// <param name="action">命令名称</param>
/// <param name="msgout">输出消息内容</param>
/// <returns>返回是否创建成功</returns>
bool BuildReslut(char* action, ...., Buffer_t* msgout)
{
	retrun true;
}


--------------------------------------------------------------
// MsgProcess.c		业务逻辑

// 打开xx
void Open(void* param)
{
	// 处理逻辑
	openparam_t* par = (openparam_t*)param; 
	....

	// 回复消息
	Buffer_t msgout
	if(BuildReslut("Open", ...., &msgout))
	{
		SendMsg(msgout);
	}
}

--------------------------------------------------------------
// xxxxDevice.c		驱动部分回调。

// 消息分发处理
cJsonProcess_t ProcessArray[] =
{
	{.ActionName = "Open",  .JsonProcess = OpenJson,		 .Process = Open },
	{.ActionName = "Close",	.JsonProcess = JsonCloseProcess, .Process = Close },
	// 最后一项必须 ActionName = NULL
	{.ActionName = NULL,		.ToBin = NULL, .Process = NULL},
};

// 驱动回调，收到数据
void DeviceRevData(Buffer2_t* buff)
{
	// 消息预处理，比如去除非Json的消息头。校验什么的。
	if(HeadProcess(buff)!)return;

	// 消息处理。
	JsonProcessMsg(buff,ProcessArray);

	// 清理现场
	TryClearBuff2(buff);
}

*/

