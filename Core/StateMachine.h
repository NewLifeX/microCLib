#pragma once

#include "Type.h"

// 未测试代码

typedef struct SMItem
{
    /// @brief 状态名称
    const char* Name;

    /// @brief 进入状态时执行的动作
    void(*Enter)(struct SMItem*, void* sm);
    /// @brief 运行状态时执行的动作
    void (*Run)(struct SMItem*, void* sm);
    /// @brief 退出状态时执行的动作
    void (*Exit)(struct SMItem*, void* sm);
}SMItem_t;

typedef struct
{
    /// @brief 当前状态
    struct SMItem* Current;
    /// @brief 下一个状态
    struct SMItem* Next;
}StateMachine_t;

void StateMachineInit(StateMachine_t* sm, SMItem_t* items);

void StateMachineMain(StateMachine_t* sm);

void StateMachineGo(StateMachine_t* sm, SMItem_t* item);
