#pragma once

// 硬件设计时间，锁板上一定有标记
// 格式按照 yyyy-MM-dd 格式写。
#define HARDWARETIME "2021-11-22"


/************** OTA配置 ****************/

// OTA Flash存储位置。
#define OTAFLASHADDRESS 0x800a000
#define OTAFLASHSIZE (23*1024)

// OTA CRC 使用的函数
#define OTACRCFUNC CaclcCRC16_MODBUS

