/*
 * CanTosAdapter.h
 *
 *  Created on: 2 лют. 2020 р.
 *      Author: banz
 */

#ifndef CANDEVICES_CANTOSADAPTER_H_
#define CANDEVICES_CANTOSADAPTER_H_

#include "CanDevice.h"
#include "IMotorController.h"

class CanTosAdapter : public CanDevice, public IMotorController {
	int16_t _MotorRpm;
	int16_t _CurrentDc;
	int16_t _CurrentRms;
	uint16_t _Voltage;
	uint8_t _Voltage12V;
	int8_t _MotorTemperature;
	int8_t _ControllerTemperature;
	bool _OvercurrentBit;
	uint8_t _OperationalMode;
	uint8_t _GearMode;
public:
	CanTosAdapter();
	virtual ~CanTosAdapter();

	virtual int16_t Rpm();
	virtual int16_t Current();
	virtual int16_t TempConstroller();
	virtual int16_t TempMotor();

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);

};

#endif /* CANDEVICES_CANTOSADAPTER_H_ */
