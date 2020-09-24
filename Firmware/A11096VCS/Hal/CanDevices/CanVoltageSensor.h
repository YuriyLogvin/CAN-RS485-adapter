/*
 * CanVoltageSensor.h
 *
 *  Created on: 23 вер. 2020 р.
 *      Author: banz
 */

#ifndef CANDEVICES_CANVOLTAGESENSOR_H_
#define CANDEVICES_CANVOLTAGESENSOR_H_

#include "CanDevice.h"

class CanVoltageSensor : public CanDevice {
	int32_t _RequestsTicks;
	int16_t _Voltage;
public:
	CanVoltageSensor(uint32_t destNodeId);
	virtual ~CanVoltageSensor();

	void SetVoltage(int16_t);

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();
};

#endif /* CANDEVICES_CANVOLTAGESENSOR_H_ */
