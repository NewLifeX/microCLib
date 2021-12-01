
#ifndef __SPI_H__
#define __SPI_H__

#include "Pin.h"

/*
*	   pin
*	STM32L1XX_HD  STM32L1XX_MDP 有三个SPI  其余为两个
*
*	SPI1_NSS	PA4		PA15			PE12
*	SPI1_SCK	PA5				PB3		PE13
*	SPI1_MISO	PA6		PA11	PB4		PE14
*	SPI1_MOSI	PA7		PA12	PB5		PE15
*
*	SPI2_NSS	PB12	PD0
*	SPI2_SCK	PB13	PD1
*	SPI2_MISO	PB14	PD3
*	SPI2_MOSI	PB15	PD4
*
*	SPI3_NSS	PA4		PA15
*	SPI3_SCK	PB3		PC10
*	SPI3_MISO	PB4		PC11
*	SPI3_MOSI	PB5		PC12
*/

/*
*	STM32F10X 不做引脚映射！ 要做也要自己去外部定义
*	STM32F10X_HD  有三个SPI  其余为两个
*
*	SPI1_NSS	PA4
*	SPI1_SCK	PA5
*	SPI1_MISO	PA6
*	SPI1_MOSI	PA7
*
*	SPI2_NSS	PB12
*	SPI2_SCK	PB13
*	SPI2_MISO	PB14
*	SPI2_MOSI	PB15
*
*	SPI3_NSS	PA15
*	SPI3_SCK	PB3
*	SPI3_MISO	PB4
*	SPI3_MOSI	PB5
*/


// 发送数据长度
typedef enum
{
	SpiBits8 ,		
	SpiBits16 ,		
}SpiBits_t;

// spi有4钟工作模式  具体参考如下网页
// http://www.eeworld.com.cn/qrs/2011/0506/article_5058.html

typedef enum
{						// CPHA	CPOL
	SpiMode1 = 0x00,	// 0	0
	SpiMode2 = 0x01,	// 0	1
	SpiMode3 = 0x10,	// 1	0
	SpiMode4 = 0x11,	// 1	1
}SpiMode_t;

// 传输序列顺序
typedef enum
{
	SpiLSB ,
	SpiMSB ,
}SpiFirstBit_t;

// Spi 参数配置
typedef struct
{							// 默认参数
	PinName Sck;			// P0
	PinName Mosi;			// P0
	PinName Miso;			// P0
	PinName Nss;			// P0
	uint32_t SpeedHz;		// 1000000
	SpiBits_t Bits;			// SpiBits8
	SpiFirstBit_t FirstBit;	// SpiMSB
	bool SoftNss;			// true
	SpiMode_t Mode;			// SpiMode1
}SpiCfg_t;


SpiCfg_t* SpiCfgCreateOrGet(SpiName spi);

void SpiCfgDelete(SpiName spi);

void SpiInit(SpiName spi);

void SpiDeInit(SpiName spi);

void SpiSetNss(SpiName spi, bool value);

// 单字节处理
uint16_t SpiInOut(SpiName spi, uint16_t data);
// 多字节处理;  tx  rx 可以是NULL。
void SpiInOutArray(SpiName spi, byte* tx, byte* rx, int size);

bool SpiCfgSet(SpiName spi, PinName miso, PinName mosi, PinName sck, PinName nss,
	bool softnss, SpiBits_t bits, SpiFirstBit_t firstbit, SpiMode_t mode, uint32_t speedHz);

// 使用方式
/*
*	SpiCfg_t* cfg =  SpiCfgCreateOrGet(Spi1);
*	cfg->Sck = PA5;
*	cfg->Mosi = PA7;
*	cfg->Miso = PA6;
*	cfg->Nss = PA4;
*	cfg->SpeedHz = 9000000;
*	// SpiCfgCreateOrGet 会填充默认参数 所以只需要修改自己需要的就可以
*
*	bool SpiCfgSet( ... )可以一次性搞定上面几行，但是没有了默认参数！
*	SpiInit(Spi1);		// 内部对引脚只做了判空，没有校验是否是相应COM口的引脚
*/

#endif  // __SPI_H__
