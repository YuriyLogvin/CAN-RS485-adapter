/*
 * ChargerFlatPack2.h
 *
 *  Created on: 7 ρεπο. 2021 π.
 *      Author: banz
 */

#ifndef CANDEVICES_CHARGERFLATPACK2_H_
#define CANDEVICES_CHARGERFLATPACK2_H_

#include "CanDevice.h"
#include "ICharger.h"

#define MaxContChargerFlatPack 3

class ChargerFlatPack2 : public CanDevice, public ICharger {
	uint16_t _VoltageNeed;
	uint16_t _CurrentNeed;
	uint16_t _VoltageHave;
	uint16_t _CurrentHave;
	uint16_t _VoltageOver;
	uint8_t _ChargerFlags;
	uint8_t _ChargerEnable;

	enum class CanTransmitStates
	{
		None,
		Losted,
		SendLogin,
		SetDefaultVoltage,
		SendVClimits
	};
	CanTransmitStates _TransmitState;

	int32_t _LastReceiving;
	int32_t _TransmitTicker;

	int32_t _TicksLogin;
	int32_t _TicksVClimits;

	struct DetectedFlatPack
	{
		uint64_t Id;
		uint8_t IntakeTemp;
		uint8_t ExhaustTemp;
		uint16_t CurrentOut;
		uint16_t VoltageOut;
		uint16_t VoltageInp;
		bool CheckedDefaultVoltage;
		bool LoggedIn;
	};

	DetectedFlatPack _DetectedFlatPack2[MaxContChargerFlatPack];
	int8_t _ActiveCharger;

public:
	ChargerFlatPack2();
	virtual ~ChargerFlatPack2();

	virtual void SetVoltage(uint16_t) override;
	virtual uint16_t GetVoltage() override;

	virtual void SetOverVoltage(uint16_t val) override;

	virtual void SetCurrent(uint16_t) override;
	virtual uint16_t GetCurrent() override;

	virtual uint8_t GetChargerFlags() override;

	virtual void TurnCharger(bool value) override;

protected:
	virtual bool ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[]);
	virtual void OnTick();

};

#endif /* CANDEVICES_CHARGERFLATPACK2_H_ */
