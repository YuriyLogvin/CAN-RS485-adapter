/* 
* ReceiveMetodHost.h
*
* Created: 15.07.2013 20:26:38
* Author: Yuriy
*/


#ifndef __RECEIVEMETODHOST_H__
#define __RECEIVEMETODHOST_H__

#include "MetodTypes.h"

class ReceiveMetodHost
{
    unsigned char* _Buff;
    unsigned char _BuffLength;

	unsigned char _GetArgumentLenght(MetodTypes mType, unsigned char nextByte);
	unsigned char _GetArgPositionAndCheckIt(unsigned char argNum, MetodTypes metodType);
	
public:
	ReceiveMetodHost();
	~ReceiveMetodHost();
	
	void InitNewMetod(unsigned char* buff, unsigned char buffLength);
	unsigned char GetMetodNumber();
	bool GetArgumentByte(unsigned char argNum, unsigned char& val);
	bool GetArgumentBool(unsigned char argNum, bool& val);
	bool GetArgumentShort(unsigned char argNum, short& val);
	bool GetArgumentUshort(unsigned char argNum, unsigned short& val);
	bool GetArgumentInt(unsigned char argNum, long& val);
	bool GetArgumentUint(unsigned char argNum, unsigned long& val);
	bool GetArgumentData(unsigned char argNum, unsigned char* data, unsigned char& dataLen);
	short GetArgument(unsigned char argNum);
	unsigned char GetArgCount();
	MetodTypes GetArgType(unsigned char argNum);
	bool CheckMetod();
	bool CheckMetod(MetodTypes argType0);
protected:
private:
	ReceiveMetodHost( const ReceiveMetodHost &c );
	ReceiveMetodHost& operator=( const ReceiveMetodHost &c );

}; //ReceiveMetodHost

#endif //__RECEIVEMETODHOST_H__
