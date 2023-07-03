/* 
* SendMetodHost.cpp
*
* Created: 15.07.2013 20:54:29
* Author: Yuriy
*/


#include "SendMetodHost.h"
#include "MetodTypes.h"
//#include "new.h"

// default constructor
SendMetodHost::SendMetodHost()
{
	_BuffLength = 20;
	_Buff = new unsigned char[_BuffLength];
	_BuffPosition = 0;
} //SendMetodHost

// default destructor
SendMetodHost::~SendMetodHost()
{
	if (_Buff)
		delete _Buff;
	_Buff = 0;
} //~SendMetodHost

unsigned char* SendMetodHost::GetBufferForTransfer( unsigned char& len )
{
	len = _BuffPosition;
	return _Buff;
}

void SendMetodHost::InitNewMetod( unsigned char metodNumber )
{
	_BuffPosition = 1;
	_Buff[0] = metodNumber;
}

void SendMetodHost::AddArgumentByte( unsigned char arg )
{
	_CheckBuffSize(2);
	_Buff[_BuffPosition++] = (unsigned char)mtByte;
	_Buff[_BuffPosition++] = arg;
}

void SendMetodHost::AddArgumentBool( bool arg )
{
	_CheckBuffSize(2);
	_Buff[_BuffPosition++] = (unsigned char)mtBool;
	_Buff[_BuffPosition++] = (unsigned char)(arg ? 1 : 0);
}

void SendMetodHost::AddArgumentShort( short arg )
{
	_CheckBuffSize(3);
	_Buff[_BuffPosition++] = (unsigned char)mtShort;
	_Buff[_BuffPosition++] = (unsigned char)(arg >> 8);
	_Buff[_BuffPosition++] = (unsigned char)(arg);
}

void SendMetodHost::AddArgumentUshort( unsigned short arg )
{
	_CheckBuffSize(3);
	_Buff[_BuffPosition++] = (unsigned char)mtUshort;
	_Buff[_BuffPosition++] = (unsigned char)(arg >> 8);
	_Buff[_BuffPosition++] = (unsigned char)(arg);
}

void SendMetodHost::AddArgumentInt( long arg )
{
	_CheckBuffSize(5);
	_Buff[_BuffPosition++] = (unsigned char)mtInt;
	_Buff[_BuffPosition++] = (unsigned char)(arg >> 24);
	_Buff[_BuffPosition++] = (unsigned char)(arg >> 16);
	_Buff[_BuffPosition++] = (unsigned char)(arg >> 8);
	_Buff[_BuffPosition++] = (unsigned char)(arg);
}

void SendMetodHost::AddArgumentUint( unsigned long arg )
{
	_CheckBuffSize(5);
	_Buff[_BuffPosition++] = (unsigned char)mtUint;
	_Buff[_BuffPosition++] = (unsigned char)(arg >> 24);
	_Buff[_BuffPosition++] = (unsigned char)(arg >> 16);
	_Buff[_BuffPosition++] = (unsigned char)(arg >> 8);
	_Buff[_BuffPosition++] = (unsigned char)(arg);
}

void SendMetodHost::AddArgumentData(unsigned char* data, unsigned char len)
{
	_CheckBuffSize(len + 2);
	_Buff[_BuffPosition++] = (unsigned char)mtData;
	_Buff[_BuffPosition++] = len;
	for (unsigned char i = 0; i < len; i++)
	{
		_Buff[_BuffPosition++] = data[i];
	}
}


void SendMetodHost::_CheckBuffSize( unsigned char p )
{
	if (_BuffPosition + p <= _BuffLength)
	return;

	_BuffLength = (unsigned char)(_BuffPosition + p);
	unsigned char* newBuf = new unsigned char[_BuffLength];
	if (newBuf == 0)
		return;

	for (unsigned char i = 0; i < _BuffPosition; i++)
	{
		newBuf[i] = _Buff[i];
	}
	
	delete _Buff;
	
	_Buff = newBuf;
}
