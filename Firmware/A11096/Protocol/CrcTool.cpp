/* 
* CrcTool.cpp
*
* Created: 15.07.2013 15:53:42
* Author: Yuriy
*/


#include "CrcTool.h"

// default constructor
CrcTool::CrcTool()
{
} //CrcTool

// default destructor
CrcTool::~CrcTool()
{
} //~CrcTool

unsigned char CrcTool::Crc7(unsigned char* arr, unsigned char count)
{
	unsigned char crc = 0;
	unsigned short i = 0, len = (unsigned short)(count * 8), arrpos = 0;
	unsigned char bitN = 0;

	while (i++ < len)
	{
		crc = (unsigned char)(crc << 1);

		crc ^= (unsigned char)((arr[arrpos] << bitN++) & 0x80);

		if ((crc & 0x80) != 0)
		crc ^= 0x89;

		if (bitN == 8)
		{
			bitN = 0;
			arrpos++;
		}

	}

	return (unsigned char)(crc & 0x7f);
}

unsigned short CrcTool::Crc16(unsigned char* arr, short len)
{
	unsigned short crc = 0;
	short i, arrpos = 0;

	while (len-- != 0)
	{
		crc ^= (unsigned short)(((unsigned short)arr[arrpos++]) << 8);

		for (i = 0; i < 8; i++)
		crc = (unsigned short)((crc & 0x8000) != 0 ? (crc << 1) ^ 0x1021 : crc << 1);
	}

	return crc;
}

void CrcTool::Crc16Init()
{
	_Crc16 = 0;
}

unsigned short CrcTool::Crc16Update(unsigned char b)
{
	_Crc16 ^= (unsigned short)(((unsigned short)b) << 8);

	for (unsigned char i = 0; i < 8; i++)
		_Crc16 = (unsigned short)((_Crc16 & 0x8000) != 0 ? (_Crc16 << 1) ^ 0x1021 : _Crc16 << 1);

	return _Crc16;
}
