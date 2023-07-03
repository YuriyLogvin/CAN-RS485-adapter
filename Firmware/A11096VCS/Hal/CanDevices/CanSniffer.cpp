/*
 * CanSniffer.cpp
 *
 *  Created on: Dec 2, 2017
 *      Author: banz
 */

#include "CanSniffer.h"
#include <stdio.h>
#include <string.h>

CanSniffer::CanSniffer(IDataTransmitter* dataTransmitter)
	: CanDevice(0, 0, 3000)
{
	_DataTransmitter = dataTransmitter;
	_LastReceivingTimeTransmitted = 0;
	_Enable = true;
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

bool CanSniffer::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	if (_DataTransmitter == 0)
		return true;

	char buff[64];

	if (rxHeader.StdId == 0)
		sprintf(buff, "%.4x%.4x : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n\r", (uint16_t)(rxHeader.ExtId >> 16), (uint16_t)rxHeader.ExtId, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	else
		sprintf(buff, "%.4x : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n\r", (uint16_t)rxHeader.StdId, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

	_DataTransmitter->Send((uint8_t*)buff, strlen(buff));

	return true;
}
