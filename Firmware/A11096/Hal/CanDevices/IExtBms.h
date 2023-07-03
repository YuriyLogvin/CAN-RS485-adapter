/*
 * IBms.h
 *
 *  Created on: 1 черв. 2022 р.
 *      Author: banz
 */

#ifndef CANDEVICES_IEXTBMS_H_
#define CANDEVICES_IEXTBMS_H_

#include <stdint.h>

class IExtBms {
public:
	enum class DriveFlags
	{
		None = 0,
		Charge = 0x1,
		Discharge = 0x2,
		ChargeImmediately = 0x4,
	};
	enum class Status
	{
		None = 0,
		Initializing = 0x1,
		Idle = 0x2,
		Wait = 0x3,
		Drive = 0x4,
		Shutdown = 0x5,
		Error = 0x6,
		Heating = 0x7
	};
	enum class Mode
	{
		None = 0,
		Idle = 0x1,
		Drive = 0x2,
		Charge = 0x4,
		Fault = 0x10
	};

	enum class ExtIds
	{ /* 0 - 64 base ids range*/
		None = 0,
		HwRevision = 1,
		SwRevision = 2,
		CellsCount = 3,
		Status = 4,
		WarningCode = 5,
		ErrorCode = 6,
		Soc = 7,
		Soh = 8,
		Soue = 9,
		PackVoltage = 10,
		PackCurrent = 11,
		BoardTemp = 12,
		BusVoltage = 13,
		CellVoltageMin = 14,
		CellVoltageMax = 15,
		CellVoltageAvg = 16,
		CellVoltageNumMin = 17,
		CellVoltageNumMax = 18,
		CellTempMin = 19,
		CellTempMax = 20,
		CellTempAvg = 21,
		CellTempNumMin = 22,
		CellTempNumMax = 23,

		ChargeCurrentLimit = 24,
		DischargeCurrentLimit = 25,
		MaxChargeVoltage = 26,
		MinDischargeVoltage = 27,
		MaxCurrent = 28,
		IsolationResistor = 29,
		DriveFlags = 30

	};

protected:
	/* Voltages - in mV
	 * Temperatures - in degrees
	 * */
	int16_t _MaxCellVoltage;
	int16_t _MinCellVoltage;
	int16_t _AvgCellVoltage;

	int8_t _MaxCellTemp;
	int8_t _MinCellTemp;
	int8_t _AvgCellTemp;

	int8_t _BoardTemp;
	int16_t _BusVoltage;
	int32_t _PackVoltage;

	int8_t _SOC;
	int8_t _SOH;
	int8_t _SOUE;

	uint16_t _CellsCount;

	int32_t _PackCurrent;
	int32_t _ChargeCurrentLimit;
	int32_t _DischargeCurrentLimit;
	int32_t _IsolationResistor;

	int32_t _MaxChargeVoltage;
	int32_t _MinDischargeVoltage;
	int32_t _MaxCurrent;

	uint16_t _HwRevision;
	uint16_t _SwRevision;

	uint32_t _WarningCode;
	uint32_t _FaultCode;

	Status _Status;
	Mode _NeedMode;
	DriveFlags _DriveFlags;

	bool _FanOn;
	bool _HeaterOn;
	bool _MeasuringIsolation;
public:
	IExtBms();

	Status GetBmsStatus();
	void SetMode(Mode mode);

	void GetMinCellVoltage(int16_t& v, uint8_t& num);
	void GetMaxCellVoltage(int16_t& v, uint8_t& num);
	int16_t GetAvgCellVoltage();

	void GetMinCellTemp(int8_t& t, uint8_t& num);
	void GetMaxCellTemp(int8_t& t, uint8_t& num);
	int8_t GetAvgCellTemp();

	int8_t GetSoc();
	int8_t GetSoh();

	virtual bool GetExtParam(ExtIds paramId, int32_t& val);

};

bool operator& (const IExtBms::Mode& v1, const IExtBms::Mode& v2);


#endif /* CANDEVICES_IEXTBMS_H_ */
