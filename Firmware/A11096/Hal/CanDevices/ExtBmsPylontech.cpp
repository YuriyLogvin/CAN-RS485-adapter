/*
 * ExtBmsPylontech.cpp
 *
 *  Created on: 26 лип. 2022 р.
 *      Author: banz
 */

#include <ExtBmsPylontech.h>

#include "Hal.h"
#include <stdio.h>
#include <string.h>
#include "../../Protocol/CrcTool.h"

#ifdef BmsFuturesExtBmsPylontech

ExtBmsPylontech::ExtBmsPylontech() :
	CanDevice(0,0,1000,CanMode::Standart),
	IExtBms()
{
	_RequestsTicks = 0;
	_TransmitStage = 0;

	_PackNumber = 0;

}

ExtBmsPylontech::~ExtBmsPylontech() {
	// TODO Auto-generated destructor stub
}

void ExtBmsPylontech::OnTick()
{
	if (Hal::GetSpendTicks(_RequestsTicks) < Hal::GetTicksInMilliSecond() * 1000)
		return;

	memset(_TxData, 0, sizeof(_TxData));

	//bool needTransmit = true;
	bool needTransmit = false;

	switch (_TransmitStage)
	{
	case 1:
		_IdDest = 0x0305;
		break;
	//case 2:
	//	_IdDest = 0x18FF03CA;
	//	break;
	default:
		_TransmitStage = 1;
		needTransmit = false;
		break;
	}

	if (needTransmit)
	{
		if (CanDevice::Transmit())
		{
			_TransmitStage++;
			_RequestsTicks = Hal::GetTickCount();
		}
	}

}


/*	CAN ID Ц followed by 2 to 8 bytes of data:
	0x351 Ц 14 02 74 0E 74 0E CC 01 Ц Battery voltage + current limits | 0-1 voltage | 2-3 charge | 4-5 discharge | 6-7 ???(Seems min voltage)
	      - 14 02 64 00 fa 00 c2 01 - for US2000C
	      - 10 02 72 01 72 01 c2 01 - From founded on inet picture
	0x355 Ц 1A 00 64 00 Ц State of Health (SOH) / State of Charge (SOC) | 0-1 SOC | 2-3 SOH
	0x356 Ц 4e 13 02 03 04 05 Ц Voltage(0-1) / Current(2-3) / Temp(4-5)
	0x359 Ц 00 00 00 00 0A 50 4E Ц Protection & Alarm flags
	0x35C Ц C0 00 Ц Battery charge request flags
	0x35E Ц 50 59 4C 4F 4E 20 20 20 Ц Manufacturer name (УPYLON У)

	V in 0.01V, A in 0.1A, T in 0.1C

	BMS transmit 0x359 every 1500 ms
	after receiving 0x0305 start transmit
	*/

bool ExtBmsPylontech::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	int32_t val = 0;
	volatile int16_t valS = 0;

	switch (rxHeader.StdId)
	{
	case 0x351:
		val = data[0] | (data[1] << 8);
		_MaxChargeVoltage = (int32_t)val * 100;
		val = data[2] | (data[3] << 8);
		_ChargeCurrentLimit = (int32_t)val * 100;
		val = data[4] | (data[5] << 8);
		_DischargeCurrentLimit = (int32_t)val * 100;
		val = data[6] | (data[7] << 8);
		_MinDischargeVoltage = (int32_t)val * 100;
		break;
	case 0x355:
		val = data[0] | (data[1] << 8);
		_SOC = val;
		val = data[2] | (data[3] << 8);
		_SOH = val;
		break;
	case 0x356:
		val = data[0] | (data[1] << 8);
		_PackVoltage = (int32_t)val * 10;
		valS = data[2] | (data[3] << 8);
		_PackCurrent = (int32_t)valS * 100;
		valS = data[4] | (data[5] << 8);
		_BoardTemp = valS/10;
		break;
	case 0x359:
		_FaultCode = data[0] | (data[1] << 8);
		_WarningCode = data[2] | (data[3] << 8);
		_PackNumber = data[4];
		break;
	case 0x35C:
		_DriveFlags = (DriveFlags)(((data[0] & 0x80) ? (uint8_t)DriveFlags::Charge : 0) | ((data[0] & 0x40) ? (uint8_t)DriveFlags::Discharge : 0) | ((data[0] & 0x20) ? (uint8_t)DriveFlags::ChargeImmediately : 0));
		//_DriveFlags = DriveFlags::None;
		//_DriveFlags = (DriveFlags)((uint8_t)_DriveFlags | ((data[0] & 0x80) ? (uint8_t)DriveFlags::Charge : 0));
		//_DriveFlags = (DriveFlags)((uint8_t)_DriveFlags | ((data[0] & 0x40) ? (uint8_t)DriveFlags::Discharge : 0));
		//_DriveFlags = (DriveFlags)((uint8_t)_DriveFlags | ((data[0] & 0x20) ? (uint8_t)DriveFlags::ChargeImmediately : 0));
		break;
	case 0x35E:
		break;
	default:
		return false;
	}

	return true;
}


bool ExtBmsPylontech::IsOnline()
{
	return CanDevice::IsOnline();
}


#endif //BmsFuturesExtBmsPylontech
