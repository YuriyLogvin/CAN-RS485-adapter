/*
 * CanCurtisAdapter.h
 *
 *  Created on: 5 ñ³÷. 2020 ð.
 *      Author: banz
 */

#ifndef CANDEVICES_CANCURTISADAPTER_H_
#define CANDEVICES_CANCURTISADAPTER_H_

#include "CanDevice.h"
#include "IMotorController.h"

class CanCurtisAdapter : public CanDevice, public IMotorController {
	uint8_t _NodeId;
	int16_t _MotorRpm;
	int16_t _CurrentRms;
	int16_t _MotorTemperature;
	int16_t _ControllerTemperature;
public:
	CanCurtisAdapter(uint8_t nodeId);
	virtual ~CanCurtisAdapter() {};

	virtual int16_t Rpm();
	virtual int16_t Current();
	virtual int16_t Voltage() {return 0;};
	virtual int16_t TempConstroller();
	virtual int16_t TempMotor();


protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);

};

#endif /* CANDEVICES_CANCURTISADAPTER_H_ */
