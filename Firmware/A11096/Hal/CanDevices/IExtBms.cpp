/*
 * IBms.cpp
 *
 *  Created on: 1 черв. 2022 р.
 *      Author: banz
 */

#include <CanDevices/IExtBms.h>

IExtBms::IExtBms() {
	_MaxCellVoltage = 0;
	_MinCellVoltage = 0;
	_AvgCellVoltage = 0;

	_MaxCellTemp = 0;
	_MinCellTemp = 0;
	_AvgCellTemp = 0;

	_BoardTemp = 0;
	_BusVoltage = 0;
	_PackVoltage = 0;

	_SOC = 0;
	_SOH = 0;
	_SOUE = 0;

	_PackCurrent = 0;
	_ChargeCurrentLimit = 0;
	_DischargeCurrentLimit = 0;
	_IsolationResistor = 0;

	_MaxChargeVoltage = 0;
	_MinDischargeVoltage = 0;
	_MaxCurrent = 0;

	_HwRevision = 0;
	_SwRevision = 0;

	_WarningCode = 0;
	_FaultCode = 0;

	_Status = IExtBms::Status::None;

	_NeedMode = Mode::None;

	_FanOn = false;
	_HeaterOn = false;
	_MeasuringIsolation = false;

	_CellsCount = 0;

}

IExtBms::Status IExtBms::GetBmsStatus()
{
	return _Status;
}

void IExtBms::GetMinCellVoltage(int16_t& v, uint8_t& num)
{
	v = _MinCellVoltage;
	num = 0;
}

void IExtBms::GetMaxCellVoltage(int16_t& v, uint8_t& num)
{
	v = _MaxCellVoltage;
	num = 0;
}

int16_t IExtBms::GetAvgCellVoltage()
{
	return _AvgCellVoltage;
}

void IExtBms::GetMinCellTemp(int8_t& t, uint8_t& num)
{
	t = _MinCellTemp;
	num = 0;
}

void IExtBms::GetMaxCellTemp(int8_t& t, uint8_t& num)
{
	t = _MaxCellTemp;
	num = 0;
}

int8_t IExtBms::GetAvgCellTemp()
{
	return _AvgCellTemp;
}

int8_t IExtBms::GetSoc()
{
	return _SOC;
}

int8_t IExtBms::GetSoh()
{
	return _SOH;
}

void IExtBms::SetMode(IExtBms::Mode mode)
{
	_NeedMode = mode;
}

bool operator& (const IExtBms::Mode& v1, const IExtBms::Mode& v2)
{
	if (((uint16_t)v1 & (uint16_t)v2) == 0)
		return false;
	return true;
}

bool IExtBms::GetExtParam(ExtIds paramId, int32_t& val)
{
	val = 0;
	switch (paramId)
	{
	case ExtIds::HwRevision:
		val = _HwRevision;
		break;
	case ExtIds::SwRevision:
		val = _SwRevision;
		break;
	case ExtIds::CellsCount:
		val = _CellsCount;
		break;
	case ExtIds::Status:
		val = (int32_t)_Status;
		break;
	case ExtIds::WarningCode:
		val = _WarningCode;
		break;
	case ExtIds::ErrorCode:
		val = _FaultCode;
		break;
	case ExtIds::Soc:
		val = _SOC;
		break;
	case ExtIds::Soh:
		val = _SOH;
		break;
	case ExtIds::Soue:
		val = _SOUE;
		break;
	case ExtIds::PackVoltage:
		val = _PackVoltage;
		break;
	case ExtIds::PackCurrent:
		val = _PackCurrent;
		break;
	case ExtIds::BoardTemp:
		val = _BoardTemp;
		break;
	case ExtIds::BusVoltage:
		val = _BusVoltage;
		break;
	case ExtIds::CellVoltageMin:
		val = _MinCellVoltage;
		break;
	case ExtIds::CellVoltageMax:
		val = _MaxCellVoltage;
		break;
	case ExtIds::CellVoltageAvg:
		val = _AvgCellVoltage;
		break;
	case ExtIds::CellVoltageNumMin:
		break;
	case ExtIds::CellVoltageNumMax:
		break;
	case ExtIds::CellTempMin:
		val = _MinCellTemp;
		break;
	case ExtIds::CellTempMax:
		val = _MaxCellTemp;
		break;
	case ExtIds::CellTempAvg:
		val = _AvgCellTemp;
		break;
	case ExtIds::CellTempNumMin:
		break;
	case ExtIds::CellTempNumMax:
		break;

	case ExtIds::ChargeCurrentLimit:
		val = _ChargeCurrentLimit;
		break;
	case ExtIds::DischargeCurrentLimit:
		val = _DischargeCurrentLimit;
		break;
	case ExtIds::MaxChargeVoltage:
		val = _MaxChargeVoltage;
		break;
	case ExtIds::MinDischargeVoltage:
		val = _MinDischargeVoltage;
		break;
	case ExtIds::MaxCurrent:
		val = _MaxCurrent;
		break;
	case ExtIds::IsolationResistor:
		val = _IsolationResistor;
		break;
	case ExtIds::DriveFlags:
		val = (uint8_t)_DriveFlags;
	}

	return true;
}
