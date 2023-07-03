/*
 * CanCurtisAdapterSdo.h
 *
 *  Created on: 27 лют. 2020 р.
 *      Author: banz
 */

#ifndef CANDEVICES_CANCURTISADAPTERSDO_H_
#define CANDEVICES_CANCURTISADAPTERSDO_H_

#include "CanDevice.h"
#include "IMotorController.h"

class CanCurtisAdapterSdo : public CanDevice, public IMotorController  {
	int32_t _RequestsTicks;
	enum class RequestsStates
	{
		None = 0,
		Voltage = 0x324D,
		Rpm = 0x3207,
		TempController = 0x322A,
		CurrentRms = 0x3209,
		TempMotor = 0x320B,
		DeviceType = 0x1000,
		Switches = 0x3226,
		ErrorRegister = 0x1001,
		EmergencyObject = 0x1014,
		End = 0xffff
	};
	RequestsStates _RequestsState;
	uint8_t _NodeId;

	int16_t _Rpm;
	int16_t _Current;
	int16_t _Voltage;
	int16_t _TempConstroller;
	int16_t _TempMotor;
	uint16_t _Switches;
	bool _ErrorMessage;

	void _ProcessErr(uint8_t data[]);

public:
	CanCurtisAdapterSdo(uint8_t nodeId);
	virtual ~CanCurtisAdapterSdo() {};

	virtual int16_t Rpm();
	virtual int16_t Current();
	virtual int16_t Voltage();
	virtual int16_t TempConstroller();
	virtual int16_t TempMotor();
	virtual uint16_t Switches();

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();

};

#endif /* CANDEVICES_CANCURTISADAPTERSDO_H_ */
