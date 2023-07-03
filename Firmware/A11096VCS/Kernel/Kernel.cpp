/*
 * Kernel.cpp
 *
 *  Created on: 6 вер. 2019 р.
 *      Author: banz
 */

#include "Kernel.h"
#include "Hal.h"
#include "CanSniffer.h"
#include "ProtocolHost.h"
#include "ReceiveMetodHost.h"
#include "SendMetodHost.h"
#include "EmkStandartMetods.h"
#include "InterfaceMetodsSpeedSens.h"
#include "InterfaceMetodsCurSens.h"
#include "InterfaceMetodsVoltSens.h"
#include "InterfaceMetodsLogicalInputs.h"
#include "InterfaceMetodsTempSens.h"

#include <CanCurrentSensor.h>
#include <CanVoltageSensor.h>
#include <CanHeartBeat.h>

/*VCS -> CAN adapter for Rs485 voltage + current sensors*/

Kernel* _Kernel = NULL;
ProtocolHost* _ProtocolHost;
ReceiveMetodHost* _ReceiveMetodHost;
SendMetodHost* _SendMetodHost;

uint8_t _DetectedDevices = 0;

void BmsKernelInit()
{

	Hal::Init();

	_Kernel = new Kernel();

	Kernel::Init();

}

void BmsKernelTick()
{
	Hal::Tick();
	Kernel::Tick();
}

CanSniffer* _CanSniffer = 0;

CanCurrentSensor* _CanCurrInterface1 = 0;
CanCurrentSensor* _CanCurrInterface2 = 0;
CanVoltageSensor* _CanVoltInterface1 = 0;
CanVoltageSensor* _CanVoltInterface2 = 0;
CanHeartBeat* _CanHeartBeat = 0;

void Kernel::Init()
{
	_ProtocolHost = new ProtocolHost(EmkAddr::Host);
	_ProtocolHost->DestAddr(EmkAddr::CurrentSensor);

	_ReceiveMetodHost = new ReceiveMetodHost();

	_SendMetodHost = new SendMetodHost();

	_CanCurrInterface1 = new CanCurrentSensor(101);
	_CanCurrInterface2 = new CanCurrentSensor(102);

	_CanVoltInterface1 = new CanVoltageSensor(111);
	_CanVoltInterface2 = new CanVoltageSensor(112);

	_CanHeartBeat = new CanHeartBeat(100);
}

int32_t _KernelTicks = 0;

enum class RequestStages
{
	None,
	CurrentSensor1,
	CurrentSensor2,
	VoltageSensor1,
	VoltageSensor2,
	Size
};

int32_t _LastDataGetTime[(int32_t)RequestStages::Size] = {0};

RequestStages& operator ++(RequestStages& a)
{
	int r = (int)a;
	if (++r >= (int)RequestStages::Size)
		r = (int)RequestStages::None;
	return a = (RequestStages)r;
}

RequestStages operator ++(RequestStages& a, int)
{
	RequestStages res = a;
	++a;
	return res;
}

RequestStages _RequestStage = RequestStages::None;

void Kernel::Tick()
{

	uint8_t b = 0;
	uint8_t* data = 0;
	uint8_t len = 0;
	for (;Hal::UsartExt->Receive(&b, 1) > 0;)
	{
		data = _ProtocolHost->ReceiveData(b, len);
		if (data)
			break;
	}


	if (data != 0)
	{
		_ReceiveMetodHost->InitNewMetod(data, len);
		_ProcessDataPacket();
	}


	if (Hal::GetSpendTicks(_KernelTicks) < Hal::GetTicksInMilliSecond() * 100)
		return;

	//Resetted value can be sended to CAN
	_CheckSensorConnections();

	_RequestStage++;
	switch (_RequestStage)
	{
	case RequestStages::CurrentSensor1:
		if (_CanHeartBeat)
			_CanHeartBeat->SetFlags(_DetectedDevices);
		_ProtocolHost->DestAddr(EmkAddr::CurrentSensor);
		_SendGetCurrent();
		break;
	case RequestStages::CurrentSensor2:
		_ProtocolHost->DestAddr(EmkAddr::CurrentSensor2);
		_SendGetCurrent();
		break;
	case RequestStages::VoltageSensor1:
		_ProtocolHost->DestAddr(EmkAddr::VoltageSensor);
		_SendGetVoltage();
		break;
	case RequestStages::VoltageSensor2:
		_ProtocolHost->DestAddr(EmkAddr::VoltageSensor2);
		_SendGetVoltage();
		break;
	default:
		break;
	}

	Hal::LedBlue(!Hal::LedBlue());

	_KernelTicks = Hal::GetTickCount();
}

void Kernel::_CheckSensorConnections()
{
	if (Hal::GetSpendTicks(_LastDataGetTime[(int16_t)_RequestStage]) < Hal::GetTicksInSecond())
		return;

	switch (_RequestStage)
	{
	case RequestStages::CurrentSensor1:
		_DetectedDevices &= ~0x1;
		if (_CanCurrInterface1)
		{
			_CanCurrInterface1->Enable(false);
			//_CanCurrInterface1->SetCurrent(0);
			//_CanCurrInterface1->SetTemp(0);
		}
		break;
	case RequestStages::CurrentSensor2:
		_DetectedDevices &= ~0x2;
		if (_CanCurrInterface2)
		{
			_CanCurrInterface2->Enable(false);
			//_CanCurrInterface2->SetCurrent(0);
			//_CanCurrInterface2->SetTemp(0);
		}
		break;
	case RequestStages::VoltageSensor1:
		_DetectedDevices &= ~0x10;
		if (_CanVoltInterface1)
		{
			_CanVoltInterface1->Enable(false);
			//_CanVoltInterface1->SetVoltage(0);
		}
		break;
	case RequestStages::VoltageSensor2:
		_DetectedDevices &= ~0x20;
		if (_CanVoltInterface2)
		{
			_CanVoltInterface2->Enable(false);
			//_CanVoltInterface2->SetVoltage(0);
		}
		break;
	default:
		break;
	}
}

void Kernel::_ProcessDataPacket()
{
	auto mNum = _ReceiveMetodHost->GetMetodNumber();

	switch (_RequestStage)
	{
	case RequestStages::CurrentSensor1:
	case RequestStages::CurrentSensor2:
		if ((InterfaceMetodsCurrSens)mNum == InterfaceMetodsCurrSens::CurrentGet)
		{
			int16_t sVal = 0;
			if (!_ReceiveMetodHost->GetArgumentShort(0, sVal))
				break;

			if (_RequestStage == RequestStages::CurrentSensor1)
			{
				_DetectedDevices |= 0x1;
				_CanCurrInterface1->Enable(true);
				_CanCurrInterface1->SetCurrent(sVal);
			}
			else
			{
				_DetectedDevices |= 0x2;
				_CanCurrInterface2->Enable(true);
				_CanCurrInterface2->SetCurrent(sVal);
			}

			if (!_ReceiveMetodHost->GetArgumentShort(1, sVal))
				break;

			if (_RequestStage == RequestStages::CurrentSensor1)
				_CanCurrInterface1->SetTemp(sVal);
			else
				_CanCurrInterface2->SetTemp(sVal);
		}
		break;
	case RequestStages::VoltageSensor1:
		if ((InterfaceMetodsVoltSens)mNum == InterfaceMetodsVoltSens::VoltageGet)
		{
			int16_t sVal = 0;
			if (_ReceiveMetodHost->GetArgumentShort(0, sVal))
			{
				_DetectedDevices |= 0x10;
				if (_CanVoltInterface1)
				{
					_CanVoltInterface1->Enable(true);
					_CanVoltInterface1->SetVoltage(sVal);
				}
			}
		}
		break;
	case RequestStages::VoltageSensor2:
		if ((InterfaceMetodsVoltSens)mNum == InterfaceMetodsVoltSens::VoltageGet)
		{
			int16_t sVal = 0;
			if (_ReceiveMetodHost->GetArgumentShort(0, sVal))
			{
				_DetectedDevices |= 0x20;
				if (_CanVoltInterface2)
				{
					_CanVoltInterface2->Enable(true);
					_CanVoltInterface2->SetVoltage(sVal);
				}
			}
		}
		break;
	default:
		break;
	}

	_LastDataGetTime[(int16_t)_RequestStage] = Hal::GetTickCount();

	if ((EmkMetods)mNum == EmkMetods::Ping)
	{
	};
}

void Kernel::_SendPing()
{
	_SendMetodHost->InitNewMetod((uint8_t)EmkMetods::Ping);

	_SendData();
}

void Kernel::_SendGetCurrent()
{
	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsCurrSens::CurrentGet);

	_SendData();
}

void Kernel::_SendGetVoltage()
{
	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsVoltSens::VoltageGet);

	_SendData();
}

void Kernel::_SendData()
{

	_ProtocolHost->InitSendData(*_SendMetodHost);

	uint8_t buff[32];

	uint8_t pos = 0;
	for (; pos < sizeof(buff); pos++)
	{
		uint8_t b;
		if (!_ProtocolHost->SendData(b))
			break;
		buff[pos] = b;
	}

	if (pos)
	{
		HAL_Delay(1); //ћежду приемом и отправкой должно быть не менее 100мк—ек
		Hal::UsartExt->Send(buff, pos);
	}

}


void Kernel::Send(uint8_t* data, uint16_t len)
{
	Hal::UsartExt->Send(data, len);
}

