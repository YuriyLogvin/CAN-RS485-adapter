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

class CanSniffer: public CanDevice
{
	IDataTransmitter* _DataTransmitter;
	int32_t _LastReceivingTimeTransmitted;
	bool _Enable;

	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);

public:
	CanSniffer(IDataTransmitter* dataTransmitter);

	void Enable(bool val);
};

#endif /* CANSNIFFER_H_ */
