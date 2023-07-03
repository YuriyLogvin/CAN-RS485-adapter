/*
 * CanSniffer.h
 *
 *  Created on: Dec 2, 2017
 *      Author: banz
 */

#ifndef CANSNIFFER_H_
#define CANSNIFFER_H_

#include "CanDevice.h"
#include "IDataTransmitter.h"

#define CanBufferredMessages	10
#define CanBufferredMessageSize	13

class CanSniffer: public CanDevice
{
	IDataTransmitter* _DataTransmitter;
	int32_t _LastReceivingTimeTransmitted;
	bool _Enable;
	/* 0 :
	 * 		0 bit - 1/0 - filled/notfilled
	 * 		1 bit:  0 - std Id, 1 Ext Id
	 * 1-4 - id
	 * 5-12 - data
	 * */
	uint8_t	_CanBuff[CanBufferredMessages * CanBufferredMessageSize];
	uint8_t	_CanBuffWritePos;
	uint8_t	_CanBuffReadPos;

	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);

public:
	CanSniffer(IDataTransmitter* dataTransmitter);

	void Enable(bool val);
	void Tick();
};

#endif /* CANSNIFFER_H_ */
