#pragma once

// Ӳ�����ʱ�䣬������һ���б��
// ��ʽ���� yyyy-MM-dd ��ʽд��
#define HARDWARETIME "2021-11-22"


/************** OTA���� ****************/

// OTA Flash�洢λ�á�
#define OTAFLASHADDRESS 0x800a000
#define OTAFLASHSIZE (23*1024)

// OTA CRC ʹ�õĺ���
#define OTACRCFUNC CaclcCRC16_MODBUS

