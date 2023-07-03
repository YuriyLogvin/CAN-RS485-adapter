/*
 * IChiller.h
 *
 *  Created on: 25 лист. 2021 р.
 *      Author: banz
 */

#ifndef CANDEVICES_ICHILLER_H_
#define CANDEVICES_ICHILLER_H_

#include <stdint.h>

class IChiller {
public:
	enum class ControlModes
	{
		Manual = 0,
		Auto = 1,
	};
	enum class ChillerModes
	{
		Waiting = 0,
		Cooling = 1,
		Heating = 2,
		Circulation = 3,
		FailureMode = 4,
		ExtremalMode = 5
	};
	enum class BatteryModes
	{
		Discharging = 0,
		Charging = 1
	};
	enum class HvModes
	{
		Discharge = 0,
		PwrSupply = 1
	};

protected:
	bool _ChillerEnable;
	int16_t _TempHaveMin;
    int16_t _TempHaveMax;
    int16_t _TempTarget;
    int16_t _VoltageHave;

    ControlModes _ControlModeNeed;
    ChillerModes _ChillerModeNeed;
    BatteryModes _BatteryModeNeed;
    HvModes _HvModeNeed;

    ControlModes _ControlModeHave;
    ChillerModes _ChillerModeHave;
    BatteryModes _BatteryModeHave;
    HvModes _HvModeHave;

	int16_t _TempIn;
	int16_t _TempOut;
	uint8_t _COP;
	uint32_t _ErrorCode;

public:
	IChiller();

	virtual void TurnOn(bool value);
	virtual bool IsOnline() = 0;

	///0.1 deg per bit
	virtual void TempHaveMinMaxp(int16_t min, int16_t max);
	///0.1 V per bit
	virtual void VoltageHave(int16_t value);
	///0.1 deg per bit
	virtual void TempTarget(int16_t value);

	virtual void ControlMode(ControlModes value);
	virtual void ChillerMode(ChillerModes value);
	virtual void BatteryMode(BatteryModes value);
	virtual void HvMode(HvModes value);

	virtual ControlModes ControlMode();
	virtual ChillerModes ChillerMode();
	virtual BatteryModes BatteryMode();
	virtual HvModes HvMode();

	///0.1 deg per bit
	virtual int16_t TempIn();
	///0.1 deg per bit
	virtual int16_t TempOut();
	virtual uint8_t COP();
	virtual uint32_t ErrorCode();
};

#endif /* CANDEVICES_ICHILLER_H_ */
