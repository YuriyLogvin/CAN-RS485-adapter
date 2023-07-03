/* 
* SendMetodHost.h
*
* Created: 15.07.2013 20:54:29
* Author: Yuriy
*/


#ifndef __SENDMETODHOST_H__
#define __SENDMETODHOST_H__


class SendMetodHost
{
	unsigned char* _Buff;
	unsigned char _BuffLength;
	unsigned char _BuffPosition;

	void _CheckBuffSize(unsigned char p);
	
public:
	SendMetodHost();
	~SendMetodHost();
	
	unsigned char* GetBufferForTransfer(unsigned char& len);
	void InitNewMetod(unsigned char metodNumber);
	void AddArgumentByte(unsigned char arg);
	void AddArgumentBool(bool arg);
	void AddArgumentShort(short arg);
	void AddArgumentUshort(unsigned short arg);
	void AddArgumentInt(long arg);
	void AddArgumentUint(unsigned long arg);
	void AddArgumentData(unsigned char* data, unsigned char len);
	
protected:
private:


}; //SendMetodHost

#endif //__SENDMETODHOST_H__
