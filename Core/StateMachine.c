
#include "StateMachine.h"

/// @brief 初始化状态机
/// @param sm 
/// @param items 
void StateMachineInit(StateMachine_t* sm, SMItem_t* items)
{
	sm->Current = NULL;
	sm->Next = items;
}

/// @brief 状态机主循环
/// @param sm 
void StateMachineMain(StateMachine_t* sm)
{
    if(sm == NULL)return;
    // 首次进入状态机
    if(sm->Current == NULL)
    {
        if(sm->Next == NULL)return;
        sm->Current = sm->Next;
        if(sm->Current->Enter != NULL) sm->Current->Enter(sm->Current);
        return;
    }    
    // 下一个状态为空，则保持当前状态不变
    if(sm->Next == NULL) sm->Next = sm->Current; 

    if(sm->Current == sm->Next)
    {
        // 当前状态不变，则继续执行当前状态
        sm->Current->Run(sm->Current);
    }
    else
    {
        // 状态切换。 old 退出， new 进入
        if(sm->Current->Exit != NULL)sm->Current->Exit(sm->Current);

        sm->Current = sm->Next;
        // 如果下一个状态为空，则直接退出
        if(sm->Current == NULL)return;
        if(sm->Current->Enter != NULL) sm->Current->Enter(sm->Current);
    }
}

/// @brief 切换状态
/// @param sm 
/// @param next 
void StateMachineGo(StateMachine_t* sm, SMItem_t* next)
{
    if(next == NULL)return;
    sm->Next = next;
}

/* 延迟状态举例

/// @brief 延迟状态结构
typedef struct
{
    SMItem_t DelayItem;
    /// @brief 状态机主体，方便处理切换问题
    StateMachine_t* SM;
    /// @brief 延迟开始时间
    Time_t Start;
    /// @brief 延时时间
    int DelayMs;
    /// @brief 延迟完成后进入的状态
    SMItem_t* Next;
} SMDelay_t;

void SMDelayEnter(struct SMItem* param)
{
    SMDelay_t* thiss = container_of(param, SMDelay_t, DelayItem);
    thiss->Start = GetCurrentTimeMs();
}

void SMDelayRun(struct SMItem* param)
{
    SMDelay_t* thiss = container_of(param, SMDelay_t, DelayItem);
    Time_t* now = GetCurrentTimeMs();

    if(now > thiss->Start + thiss->DelayMs)
    {
        StateMachineGo(thiss->SM, thiss->Next);
    }
}


SMDelay_t SMDelay=
{
    .DelayItem = 
    {
        .Name = "Delay",
        .Enter = SMDelayEnter,
        .Run = SMDelayRun,
        .Exit = NULL,
    },
    .Start = 0,
    .DelayMs = 100,
    .Next = NULL,
};

void SMDelayInit(SMDelay_t* delay, StateMachine_t* sm, int delayMs, SMItem_t* next)
{
    delay.DelayItem.Name = "Delay";
    delay.DelayItem.Enter = SMDelayEnter;
    delay.DelayItem.Run = SMDelayRun;
    delay.DelayItem.Exit = NULL;

    delay->SM = sm;
    delay->DelayMs = delayMs;
    delay->Next = next;
}

*/
