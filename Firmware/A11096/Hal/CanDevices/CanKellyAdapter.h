/*
 * CanKellyAdapter.h
 *
 *  Created on: 1 черв. 2020 р.
 *      Author: banz
 */

#ifndef CANDEVICES_CANKELLYADAPTER_H_
#define CANDEVICES_CANKELLYADAPTER_H_

#include "CanDevice.h"
#include "IMotorController.h"

class CanKellyAdapter : public CanDevice, public IMotorController  {
	int16_t _MotorRpm;
	int16_t _CurrentRms;
	int16_t _MotorTemperature;
	int16_t _ControllerTemperature;
	uint16_t _ErrorCode;
	uint8_t _Switches;
public:
	CanKellyAdapter();

	virtual ~CanKellyAdapter() {};

	virtual int16_t Rpm();
	virtual int16_t Current();
	virtual int16_t Voltage() {return 0;};
	virtual int16_t TempConstroller();
	virtual int16_t TempMotor();
	virtual uint16_t Switches();


protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);

};

#endif /* CANDEVICES_CANKELLYADAPTER_H_ */
