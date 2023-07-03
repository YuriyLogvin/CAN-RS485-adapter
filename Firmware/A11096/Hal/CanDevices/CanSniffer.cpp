/*
 * CanSniffer.cpp
 *
 *  Created on: Dec 2, 2017
 *      Author: banz
 */

#include "CanSniffer.h"
#include <stdio.h>
#include <string.h>
#include "Hal.h"

CanSniffer::CanSniffer(IDataTransmitter* dataTransmitter)
	: CanDevice(0, 0, 3000)
{
	_DataTransmitter = dataTransmitter;
	_LastReceivingTimeTransmitted = 0;
	_Enable = true;
	_CanBuffWritePos = 0;
	_CanBuffReadPos = 0;
}

/*void CanSniffer::Tick()
{
	CanDevice::Transmit(false);

	if (!_Enable)
		return;

	if (_DataTransmitter == 0)
		return;

	if (_LastReceivingTime == _LastReceivingTimeTransmitted)
		return;


	_LastReceivingTimeTransmitted = _LastReceivingTime;

	char buff[64];

	if (_RxExtId)
		sprintf(buff, "%.4x%.4x : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n\r", (uint16_t)(_RxExtId >> 16), (uint16_t)_RxExtId, _RxData[0], _RxData[1], _RxData[2], _RxData[3], _RxData[4], _RxData[5], _RxData[6], _RxData[7]);
	else
		sprintf(buff, "%.4x : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n\r", (uint16_t)_RxStdId, _RxData[0], _RxData[1], _RxData[2], _RxData[3], _RxData[4], _RxData[5], _RxData[6], _RxData[7]);

	_DataTransmitter->Send((uint8_t*)buff, strlen(buff));

}*/

void CanSniffer::Enable(bool val)
{
	_Enable = val;
}

uint16_t tCounter = 0;

bool CanSniffer::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	if (_DataTransmitter == 0)
		return true;

	Hal::LedBlue(true);
	_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 0] = 1;

	if (rxHeader.StdId != 0)
	{
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 1] = rxHeader.StdId;
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 2] = rxHeader.StdId >> 8;
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 3] = 0;
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 4] = 0;
	}
	else
	{
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 0] |= 0x2;
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 1] = rxHeader.ExtId;
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 2] = rxHeader.ExtId >> 8;
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 3] = rxHeader.ExtId >> 16;
		_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 4] = rxHeader.ExtId >> 24;
	}

	_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 0] |= rxHeader.DLC << 4;


	memcpy(&_CanBuff[_CanBuffWritePos * CanBufferredMessageSize + 5], data, 8);

	tCounter++;
	//Hal::LedBlue(!Hal::LedBlue());
	Hal::LedBlue(false);

	if (++_CanBuffWritePos >= CanBufferredMessages)
		_CanBuffWritePos = 0;

	return true;
}

void CanSniffer::Tick()
{
	if (_DataTransmitter == 0)
		return ;

	if (_DataTransmitter->IsBusy())
		return ;

	char buff[64];
	uint8_t* buffDataPos = _CanBuff + _CanBuffReadPos * CanBufferredMessageSize;

	if (buffDataPos[0] == 0)
		return;

	uint8_t bCount = _CanBuff[_CanBuffReadPos * CanBufferredMessageSize + 0] >> 4;
	if (_CanBuff[_CanBuffReadPos * CanBufferredMessageSize + 0] & 0x2)
		sprintf(buff, "%.2x%.2x%.2x%.2x : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x (%i:%i)\n\r", buffDataPos[4], buffDataPos[3], buffDataPos[2], buffDataPos[1], buffDataPos[5], buffDataPos[6], buffDataPos[7], buffDataPos[8], buffDataPos[9], buffDataPos[10], buffDataPos[11], buffDataPos[12], bCount, tCounter);
	else
		sprintf(buff, "%.2x%.2x : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x (%i:%i)\n\r", buffDataPos[2], buffDataPos[1], buffDataPos[5], buffDataPos[6], buffDataPos[7], buffDataPos[8], buffDataPos[9], buffDataPos[10], buffDataPos[11], buffDataPos[12], bCount, tCounter);

	_DataTransmitter->Send((uint8_t*)buff, strlen(buff));
	buffDataPos[0] = 0;

	if (++_CanBuffReadPos >= CanBufferredMessages)
		_CanBuffReadPos = 0;

}
