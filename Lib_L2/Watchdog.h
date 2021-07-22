#pragma once

#include "Type.h"

typedef enum
{
	WatchDog_DIV4 = 0x00,
	WatchDog_DIV8,
	WatchDog_DIV16,
	WatchDog_DIV32,
	WatchDog_DIV64,
	WatchDog_DIV128,
	WatchDog_DIV256,
}WatchDogPrescaler_t;


void IWDG_Config(WatchDogPrescaler_t prv, ushort rlv);

void IWDG_Feed(void);
