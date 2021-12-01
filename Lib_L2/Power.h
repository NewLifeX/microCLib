#ifndef __POWER_H__
#define __POWER_H__

#include "Include.h"

typedef enum
{
    LOW_POWER_MODE_SLEEP = 1,
    LOW_POWER_MODE_STOP
} LowPowerMode_t;

typedef struct
{
	LowPowerMode_t Mode;
	// LvdVol;
}PowerCfg_t;

typedef struct
{
    void *next;			// PowerCfgItem_t
	PowerCfg_t* Config;
} PowerCfgItem_t;

static PowerCfgItem_t* PowerCfgList;

// void LowPowerListInsert(PowerCfgList_t *p );
// PowerCfgList_t GetLowPowerMode( void );
// void SetLowPowerModeContext(PowerCfgList_t context );
// void McuUpdateLowPowerMode( void );
// void McuLowPowerStopModeEnable( bool status );
// 
// // 电源管理切换
// void McuPowerContext(PowerCfgItem_t* cfg);
// 
void McuEnterLowPowerMode( void );
void McuRecoverFromLowPowerMode( void );

#endif
