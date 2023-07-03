/* 
* ReceiveMetodHost.cpp
*
* Created: 15.07.2013 20:26:38
* Author: Yuriy
*/


#include "ReceiveMetodHost.h"

// default constructor
ReceiveMetodHost::ReceiveMetodHost()
{
} //ReceiveMetodHost

// default destructor
ReceiveMetodHost::~ReceiveMetodHost()
{
} //~ReceiveMetodHost

unsigned char ReceiveMetodHost::_GetArgumentLenght( MetodTypes mType, unsigned char nextByte )
{
	switch (mType)
	{
		case mtBool:
		return 1;
		case mtByte:
		return 1;
		case mtShort:
		return 2;
		case mtUshort:
		return 2;
		case mtInt:
		return 4;
		case mtUint:
		return 4;
		case mtData:
		return 1 + nextByte;
		default:
		return 0xff;
	}
}

unsigned char ReceiveMetodHost::_GetArgPositionAndCheckIt( unsigned char argNum, MetodTypes metodType )
{
	unsigned char curPos;
	for (curPos = 1; curPos < _BuffLength; )
	{
		unsigned char argLen = _GetArgumentLenght((MetodTypes)_Buff[curPos], _Buff[curPos+1]);

		if (argLen == 0xff)
		return argLen;

		argLen++;

		if (curPos + argLen > _BuffLength)
		return 0xff;

		if (argNum == 0)
		return curPos;

		curPos += argLen;
		argNum--;
	}
	return 0xff;
}

void ReceiveMetodHost::InitNewMetod( unsigned char* buff, unsigned char buffLength )
{
	_Buff = buff;
	_BuffLength = buffLength;
}

unsigned char ReceiveMetodHost::GetMetodNumber()
{
	return _Buff[0];
}

bool ReceiveMetodHost::GetArgumentByte( unsigned char argNum, unsigned char& val )
{
	unsigned char argPos = _GetArgPositionAndCheckIt(argNum, mtByte);
	if (argPos == 0xff)
	return false;

	val = _Buff[argPos + 1];
	return true;
}

bool ReceiveMetodHost::GetArgumentData(unsigned char argNum, unsigned char* data, unsigned char& dataLen)
{
	unsigned char argPos = _GetArgPositionAndCheckIt(argNum, mtData);
	if (argPos == 0xff)
		return false;

	unsigned char lenT = _Buff[argPos + 1];
	unsigned char i = 0;
	for (; (i < dataLen) && (i < lenT); i++)
		data[i] = _Buff[argPos + 1 + i];

	if (lenT != i)
		return false;

	return true;
}

bool ReceiveMetodHost::GetArgumentBool( unsigned char argNum, bool& val )
{
	unsigned char argPos = _GetArgPositionAndCheckIt(argNum, mtBool);
	if (argPos == 0xff)
	return false;

	val = _Buff[argPos + 1] == 0 ? false : true;
	return true;
}

bool ReceiveMetodHost::GetArgumentShort( unsigned char argNum, short& val )
{
	unsigned char argPos = _GetArgPositionAndCheckIt(argNum, mtShort);
	if (argPos == 0xff)
	return false;

	val = (short)(_Buff[argPos + 1] << 8);
	val |= (short)(_Buff[argPos + 2]);
	return true;
}

bool ReceiveMetodHost::GetArgumentUshort( unsigned char argNum, unsigned short& val )
{
	unsigned char argPos = _GetArgPositionAndCheckIt(argNum, mtUshort);
	if (argPos == 0xff)
	return false;

	val = (unsigned short)(_Buff[argPos + 1] << 8);
	val |= (unsigned short)(_Buff[argPos + 2]);
	return true;
}

bool ReceiveMetodHost::GetArgumentInt( unsigned char argNum, long& val )
{
	unsigned char argPos = _GetArgPositionAndCheckIt(argNum, mtInt);
	if (argPos == 0xff)
	return false;

	val = (long)(((long)_Buff[argPos + 1]) << 24);
	val |= (long)(((long)_Buff[argPos + 2]) << 16);
	val |= (long)(((long)_Buff[argPos + 3]) << 8);
	val |= (long)(_Buff[argPos + 4]);
	return true;
}

bool ReceiveMetodHost::GetArgumentUint( unsigned char argNum, unsigned long& val )
{
	unsigned char argPos = _GetArgPositionAndCheckIt(argNum, mtUint);
	if (argPos == 0xff)
	return false;

	val = (unsigned long)(((unsigned long)_Buff[argPos + 1]) << 24);
	val |= (unsigned long)(((unsigned long)_Buff[argPos + 2]) << 16);
	val |= (unsigned long)(((unsigned long)_Buff[argPos + 3]) << 8);
	val |= (unsigned long)(_Buff[argPos + 4]);
	return true;
}

short ReceiveMetodHost::GetArgument( unsigned char argNum )
{
	unsigned char argPos = _GetArgPositionAndCheckIt(argNum, mtShort);
	if (argPos == 0xff)
	return -1;
	short res = 0;
	res = (short)(_Buff[argPos + 1] << 8);
	res |= (short)(_Buff[argPos + 2]);
	return res;
}

unsigned char ReceiveMetodHost::GetArgCount()
{
	unsigned char argNum = 0;
	unsigned char curPos;
	for (curPos = 1; curPos < _BuffLength; )
	{
		unsigned char argLen = _GetArgumentLenght((MetodTypes)_Buff[curPos], _Buff[curPos+1]);

		if (argLen == 0xff)
		return argNum;

		argLen++;

		if (curPos + argLen > _BuffLength)
		return argNum;

		curPos += argLen;
		argNum++;
	}
	return argNum;
}

MetodTypes ReceiveMetodHost::GetArgType( unsigned char argNum )
{
	unsigned char curPos;
	for (curPos = 1; curPos < _BuffLength; )
	{
		unsigned char argLen = _GetArgumentLenght((MetodTypes)_Buff[curPos], _Buff[curPos+1]);

		if (argLen == 0xff)
		return mtUndefined;

		argLen++;

		if (curPos + argLen > _BuffLength)
		return mtUndefined;

		if (argNum == 0)
		return (MetodTypes)_Buff[curPos];

		curPos += argLen;
		argNum--;
	}
	return mtUndefined;
}

bool ReceiveMetodHost::CheckMetod()
{
	return GetArgCount() == 0;
}

bool ReceiveMetodHost::CheckMetod( MetodTypes argType0 )
{
	if (GetArgCount() != 1)
		return false;
	if (GetArgType(0) != argType0)
		return false;
	return true;
}
