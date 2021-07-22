
#ifndef  __INTERRUPT_H__
#define  __INTERRUPT_H__


#include "Include.h"

void IRQ_Config(IRQn_Type irqNum, byte prio, byte subPrio, bool stat);

// 中断分组函数
void NVIC_SetPriorityGroup(byte group);

#endif
