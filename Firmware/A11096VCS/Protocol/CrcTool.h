/* 
* CrcTool.h
*
* Created: 15.07.2013 15:53:43
* Author: Yuriy
*/


#ifndef __CRCTOOL_H__
#define __CRCTOOL_H__


class CrcTool
{
//variables
public:
protected:
private:

//functions
public:
	CrcTool();
	~CrcTool();
	
	void Crc16Init();
	unsigned short Crc16Update(unsigned char b);
	
	static unsigned short Crc16(unsigned char* arr, short len);
	static unsigned char Crc7(unsigned char* arr, unsigned char count);
	
protected:
private:
	CrcTool( const CrcTool &c );
	CrcTool& operator=( const CrcTool &c );

	volatile unsigned short _Crc16;

}; //CrcTool

#endif //__CRCTOOL_H__
