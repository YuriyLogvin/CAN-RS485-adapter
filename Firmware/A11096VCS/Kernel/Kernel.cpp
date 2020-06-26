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

Kernel* _Kernel = NULL;
ProtocolHost* _ProtocolHost;
ReceiveMetodHost* _ReceiveMetodHost;
SendMetodHost* _SendMetodHost;

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

CanCurrentSensor* _CanInterface = 0;

void Kernel::Init()
{
	_ProtocolHost = new ProtocolHost(EmkAddr::Host);
	_ProtocolHost->DestAddr(EmkAddr::CurrentSensor);

	_ReceiveMetodHost = new ReceiveMetodHost();

	_SendMetodHost = new SendMetodHost();

	_CanInterface = new CanCurrentSensor(101);

}

int32_t _KernelTicks = 0;

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


	if (Hal::GetSpendTicks(_KernelTicks) < Hal::GetTicksInMilliSecond() * 1000)
		return;

	_SendGetCurrent();

	Hal::LedBlue(!Hal::LedBlue());

	_KernelTicks = Hal::GetTickCount();
}

void Kernel::_ProcessDataPacket()
{
	auto mNum = _ReceiveMetodHost->GetMetodNumber();

	if ((InterfaceMetodsCurrSens)mNum == InterfaceMetodsCurrSens::CurrentGet)
	{
		int16_t sVal = 0;
		if (_ReceiveMetodHost->GetArgumentShort(0, sVal))
			_CanInterface->SetCurrent(sVal);
		if (_ReceiveMetodHost->GetArgumentShort(1, sVal))
			_CanInterface->SetTemp(sVal);
	}
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

