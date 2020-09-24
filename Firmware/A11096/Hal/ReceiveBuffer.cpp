/* 
* ReceiveBuffer.cpp
*
* Created: 15.07.2013 20:18:04
* Author: Yuriy
*/


#include "ReceiveBuffer.h"

// default constructor
ReceiveBuffer::ReceiveBuffer()
{
	_Buff = 0;
	_Init(128);
} //ReceiveBuffer

 ReceiveBuffer::ReceiveBuffer( short buffSize )
{
	_Buff = 0;
	_Init(buffSize);
}

// default destructor
ReceiveBuffer::~ReceiveBuffer()
{
#ifndef DEBUG
	if (_Buff)
		delete _Buff;
#endif
	_Buff = 0;
} //~ReceiveBuffer

#ifdef DEBUG
unsigned char _ReceiveBuffer[256] = {0xff};
#endif

void ReceiveBuffer::_Init( short buffSize )
{
#ifndef DEBUG
	if (_Buff)
		delete _Buff;
		
	_Buff = new unsigned char[buffSize];
	if (_Buff == 0)
		return ;
	_BuffSize = buffSize;
#else
	_Buff = _ReceiveBuffer;
	_BuffSize = sizeof(_ReceiveBuffer);
#endif
		
	_BuffPushPos = 0;
	_BuffGetPos = 0;
	//_BuffFilledCount = 0;
}

bool ReceiveBuffer::Push( unsigned char b )
{
	/*if (_BuffFilledCount >= _BuffSize)
	{
		return false;
	}*/
	
	//перезатирание данных по кругу не контролируется. Неизвестно, что лучше - потерять или перезаписать данные. Зато неоднозначностей меньше.
	_Buff[_BuffPushPos] = b;
	_BuffPushPos++;
	if (_BuffPushPos >= _BuffSize)
		_BuffPushPos = 0;

	//_BuffFilledCount++;
	return true;
}

bool ReceiveBuffer::Push(const unsigned char* b, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (!Push(b[i]))
			return false;;
	}
	return true;
}

bool ReceiveBuffer::Get( unsigned char& b )
{
	/*if (_BuffFilledCount == 0)
	{
		return false;
	}*/
	if (_BuffGetPos == _BuffPushPos)
		return false; //значит буфер пуст
	
	b = _Buff[_BuffGetPos];
	_BuffGetPos++;
	if (_BuffGetPos >= _BuffSize)
		_BuffGetPos = 0;
	//_BuffFilledCount--;
	return true;
}
