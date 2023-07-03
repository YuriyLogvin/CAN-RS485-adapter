/* 
* ReceiveBuffer.h
*
* Created: 15.07.2013 20:18:04
* Author: Yuriy
*/


#ifndef __RECEIVEBUFFER_H__
#define __RECEIVEBUFFER_H__


class ReceiveBuffer
{
	unsigned char* _Buff;
	short _BuffPushPos;
	short _BuffGetPos;
	short _BuffSize;
	//short _BuffFilledCount;

	void _Init(short buffSize);
	
public:
	ReceiveBuffer();
	ReceiveBuffer(short buffSize);
	~ReceiveBuffer();
	
	bool Push(unsigned char b);
	bool Push(const unsigned char* b, int len);

	bool Get(unsigned char& b);
protected:
private:
	ReceiveBuffer( const ReceiveBuffer &c );
	ReceiveBuffer& operator=( const ReceiveBuffer &c );

}; //ReceiveBuffer

#endif //__RECEIVEBUFFER_H__
