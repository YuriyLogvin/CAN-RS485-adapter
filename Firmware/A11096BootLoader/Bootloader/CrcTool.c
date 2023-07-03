/* 
* CrcTool.cpp
*
* Created: 15.07.2013 15:53:42
* Author: Yuriy
*/


#include "../Bootloader/CrcTool.h"

static volatile unsigned short _Crc16;
static volatile unsigned int _Crc32;

unsigned char Crc7(unsigned char* arr, unsigned char count)
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

unsigned short Crc16(unsigned char* arr, short len)
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

void Crc16Init()
{
	_Crc16 = 0;
}

unsigned short Crc16Update(unsigned char b)
{
	unsigned char i;

	_Crc16 ^= (unsigned short)(((unsigned short)b) << 8);

	for (i = 0; i < 8; i++)
		_Crc16 = (unsigned short)((_Crc16 & 0x8000) != 0 ? (_Crc16 << 1) ^ 0x1021 : _Crc16 << 1);

	return _Crc16;
}

unsigned int aCrc32(unsigned char* arr, int len)
{
        int i;
	char j;
        unsigned int b, c, crc;
        const unsigned int g0 = 0xEDB88320, g1 = g0 >> 1,
                              g2 = g0 >> 2,    g3 = g0 >> 3;

           i = 0;
           crc = 0xFFFFFFFF;
           for (i = 0; i < len; i++) 
           {
              b = arr[i];                // Get next byte.
              crc = crc ^ b;
              for (j = 1; j >= 0; j--) {        // Do two times.
                 c = ((crc<<31>>31) & g3) ^ ((crc<<30>>31) & g2) ^
                     ((crc<<29>>31) & g1) ^ ((crc<<28>>31) & g0);
                 crc = (crc >> 4) ^ c;
              }
              i = i + 1;
           }
           return ~crc;
}

unsigned int Crc32(unsigned char* arr, int len)
{
   int i, j;
   unsigned int b, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   for (i = 0; i < len; i++) 
   {
      b = arr[i];      
      crc = crc ^ b;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(int)(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

void Crc32Init()
{
	_Crc32 = 0xFFFFFFFF;
}

unsigned int Crc32Update(unsigned char b)
{

    unsigned int byte, c, mask;
	char j;

	byte = b;

	_Crc32 = _Crc32 ^ byte;

	for (j = 8; j > 0; j--)
	{    // Do eight times.
		 mask = (_Crc32 & 1) != 0 ? 0xFFFFFFFF : 0;
		 _Crc32 = (_Crc32 >> 1) ^ (0xEDB88320 & mask);
	}
        return ~_Crc32;
}

unsigned int aCrc32Update(unsigned char b)
{

        unsigned int c;
	char j;
        const unsigned int g0 = 0xEDB88320, g1 = g0 >> 1,
                              g2 = g0 >> 2,    g3 = g0 >> 3;

	_Crc32 = _Crc32 ^ b;
        for (j = 1; j >= 0; j--) 
	{        // Do two times.
        	c = 	((_Crc32<<31>>31) & g3) ^ ((_Crc32<<30>>31) & g2) ^
                	((_Crc32<<29>>31) & g1) ^ ((_Crc32<<28>>31) & g0);
		_Crc32 = (_Crc32 >> 4) ^ c;
        }
        return ~_Crc32;
}
