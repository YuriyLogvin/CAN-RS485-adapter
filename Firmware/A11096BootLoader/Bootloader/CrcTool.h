/* 
* CrcTool.h
*
* Created: 15.07.2013 15:53:43
* Author: Yuriy
*/


#ifndef __CRCTOOL_H__
#define __CRCTOOL_H__

#if defined __cplusplus
extern "C" {
#endif

void Crc16Init();
unsigned short Crc16Update(unsigned char b);

unsigned short Crc16(unsigned char* arr, short len);


unsigned char Crc7(unsigned char* arr, unsigned char count);
	

void Crc32Init();
unsigned int Crc32Update(unsigned char b);

#if defined __cplusplus
}
#endif

#endif //__CRCTOOL_H__
