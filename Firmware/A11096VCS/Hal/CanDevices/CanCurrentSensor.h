/*
 * CanCurrentSensor.h
 *
 *  Created on: 26 черв. 2020 р.
 *      Author: banz
 */

#ifndef CANDEVICES_CANCURRENTSENSOR_H_
#define CANDEVICES_CANCURRENTSENSOR_H_

#include "CanDevice.h"

class CanCurrentSensor : public CanDevice {
	int32_t _RequestsTicks;
	int16_t _Current;
	int16_t _Temp;

public:
	CanCurrentSensor(uint32_t destNodeId);
	virtual ~CanCurrentSensor();

	void SetCurrent(int16_t);
	void SetTemp(int16_t);

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();

};

#endif /* CANDEVICES_CANCURRENTSENSOR_H_ */
