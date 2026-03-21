#pragma once

#include "Type.h"

// 未测试代码

typedef struct SMItem
{
    /// @brief 状态名称
    const char* Name;

    /// @brief 进入状态时执行的动作
    void(*Enter)(struct SMItem*);
    /// @brief 运行状态时执行的动作
    void (*Run)(struct SMItem*);
    /// @brief 退出状态时执行的动作
    void (*Exit)(struct SMItem*);   
}SMItem_t;

typedef struct
{
    /// @brief 当前状态
    struct SMItem* Current;
    /// @brief 下一个状态
    struct SMItem* Next;

    /// @brief 所有状态，最后一个状态 State=-1 & Run=NULL
    SMItem_t** Items; 
}StateMachine_t;

void StateMachineMian(StateMachine_t* sm);

void StateMachineGo(StateMachine_t* sm, SMItem_t* item);
