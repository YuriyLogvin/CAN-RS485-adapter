/* 
* ProtocolHost.cpp
*
* Created: 15.07.2013 20:06:39
* Author: Yuriy
*/

#include "stdint.h"
#include "ProtocolHost.h"
#include "CrcTool.h"
//#include "new.h"
//#include <avr/io.h>
//#include <avr/iom168.h>

ProtocolHost::ProtocolHost( EmkAddr address )
{
	for (int8_t i = 0; i < _AddressSelfCount; i++)
		_AddressSelf[i] = EmkAddr::Broadcast;
	_AddressSelf[0] = address;
	_AddressDest = address;
	_SendingState = None;
	_DataBufferLength = 32;
	_DataBuffer = new unsigned char[_DataBufferLength];
	if (_DataBuffer == 0)
		return;
	_DataBufferPosition = 0;
	_ReceiveState = None;
	_EscSymvol = false;
	_SendEscChar = 0;
	_CheckSum = 0;
	_RefPointSendedDataBuffCrc = 0;
	_RefPointSendedDataBuffLength = 0;
	_RefPointSendedDataBuffPosition = 0;
	_RefPointSendedDataBuff = 0;
	_DataPacketLength = 0;

}

// default destructor
ProtocolHost::~ProtocolHost()
{
	if (_DataBuffer)
		delete _DataBuffer;
	_DataBuffer = 0;
} //~ProtocolHost

unsigned char* ProtocolHost::ReceiveData( unsigned char data, unsigned char& dataLength )
{
	bool escSymv = false;
	if (_EscSymvol && data != _CharStart)
	{
		switch (data)
		{
			case 0x1:
			data = _CharStart;
			escSymv = true;
			break;
			case 0x2:
			data = _CharEsc;
			escSymv = true;
			break;
			case _CharStart:
			break;
			default:
			data = 0;
			break;
		}
		_EscSymvol = false;
	}

	if ( ! escSymv && data == _CharStart)
	{
		_ReceiveState = Address;
		_DataPacketLength = 0;
		_DataBufferPosition = 0;
	}
	else if (!escSymv && data == _CharEsc)
	{
		_EscSymvol = true;
	}
	else
	{
		switch (_ReceiveState)
		{
			case Address:
				_ReceiveState = None;
				_AddressPacket = (EmkAddr)data;
				if (data == _BroadcastAddress)
				{
					_ReceiveState = DataLength;
					break;
				}
				for (int8_t i = 0; i < _AddressSelfCount; i++)
					if (data == (unsigned char)_AddressSelf[i])
					{
						_ReceiveState = DataLength;
						break;
					}
				break;
			case DataLength:
				_DataPacketLength = data;
				if (_DataBufferLength < _DataPacketLength)
				{
					delete _DataBuffer;
					_DataBuffer = new unsigned char[_DataPacketLength];
					if (_DataBuffer == 0)
						return 0;
					_DataBufferLength = _DataPacketLength;
				}
				_ReceiveState = Data;
				break;
			case Data:
				_DataBuffer[_DataBufferPosition] = data;
				_DataBufferPosition++;
				if (_DataBufferPosition >= _DataPacketLength)
				_ReceiveState = CheckSum0;
				break;
			case CheckSum0:
				_CheckSum = (unsigned short)(data << 8);
				_ReceiveState = CheckSum1;
				break;
			case CheckSum1:
				{
					_CheckSum |= data;
					_ReceiveState = None;
					unsigned short i_CheckSum = CrcTool::Crc16(_DataBuffer, _DataBufferPosition);
					if (i_CheckSum == _CheckSum)
					{
						dataLength = _DataBufferPosition;
						return _DataBuffer;
					}
					break;
				}
			default:
				break;
		}
		
	}
	return 0;
}

void ProtocolHost::InitSendData( unsigned char* data, unsigned char dataLength )
{
	_RefPointSendedDataBuffPosition = 0;
	_RefPointSendedDataBuff = data;
	_RefPointSendedDataBuffLength = dataLength;
	_RefPointSendedDataBuffCrc = CrcTool::Crc16(_RefPointSendedDataBuff, dataLength);
	_SendingState = None;
	_SendEscChar = _CharEsc;
}

void ProtocolHost::InitSendData( SendMetodHost& smHost )
{
	unsigned char* data = 0;
	unsigned char dataLen = 0;

	data = smHost.GetBufferForTransfer(dataLen);

	InitSendData(data, dataLen);
}

void ProtocolHost::_SendData( unsigned char inData, unsigned char& data )
{
	switch (inData)
	{
		case _CharStart:
			_SendEscChar = 0x1;
			data = _CharEsc;
			break;
		case _CharEsc:
			_SendEscChar = 0x2;
			data = _CharEsc;
			break;
		default:
			data = inData;
			break;
	}
}

void ProtocolHost::DestAddr(EmkAddr address)
{
	_AddressDest = address;
}

void ProtocolHost::AddSelfAddr(EmkAddr address)
{
	for (int8_t i = 0; i < _AddressSelfCount; i++)
	{
		if (_AddressSelf[i] != EmkAddr::Broadcast)
			continue;

		_AddressSelf[i] = address;
		break;
	}

}

EmkAddr ProtocolHost::PacketAddr()
{
	return _AddressPacket;
}

bool ProtocolHost::SendData( unsigned char& data )
{
	if (_SendEscChar != _CharEsc)
	{
		data = _SendEscChar;
		_SendEscChar = _CharEsc;
		return true;
	}

	switch (_SendingState)
	{
		case None:
			data = _CharStart;
			_SendingState = Address;
			return true;
		case Address:
			_SendData((unsigned char)_AddressDest, data);
			_SendingState = DataLength;
			return true;
		case DataLength:
			_SendData((unsigned char)_RefPointSendedDataBuffLength, data);
			_SendingState = Data;
			return true;
		case Data:
			_SendData(_RefPointSendedDataBuff[_RefPointSendedDataBuffPosition], data);
			_RefPointSendedDataBuffPosition++;
			if (_RefPointSendedDataBuffPosition == _RefPointSendedDataBuffLength)
			_SendingState = CheckSum0;
			return true;
		case CheckSum0:
			_SendData((unsigned char)(_RefPointSendedDataBuffCrc >> 8), data);
			_SendingState = CheckSum1;
			return true;
		case CheckSum1:
			_SendData((unsigned char)_RefPointSendedDataBuffCrc, data);
			_SendingState = Finished;
			return true;
		case Finished:
			data = 0;
			return false;
		default:
			data = 0;
			break;
	}
	return false;
}
