/*
 * Kernel.cpp
 *
 *  Created on: 6 вер. 2019 р.
 *      Author: banz
 */

#include <sys/_stdint.h>
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
#include "InterfaceMetodsCharger.h"

#include "CanDevices/CanCurtisAdapter.h"
#include "CanDevices/CanTosAdapter.h"
#include "CanDevices/CanCurtisAdapterSdo.h"
#include "CanDevices/CanKellyAdapter.h"
#include "CanDevices/ChargerSaeJ1939.h"
#include "CanDevices/ChargerFlatPack2.h"


Kernel* _Kernel = NULL;
ProtocolHost* _ProtocolHost;
ReceiveMetodHost* _ReceiveMetodHost;
SendMetodHost* _SendMetodHost;

void BmsKernelInit()
{

#if (MODE == MODE_CURTIS_SDO || MODE == MODE_FLATPACK2)
#define canSilentMode false
#else
#define canSilentMode true
#endif

	Hal::Init(canSilentMode);

	_Kernel = new Kernel();

	Kernel::Init();

}

void BmsKernelTick()
{
	Hal::Tick();
	Kernel::Tick();
}

CanSniffer* _CanSniffer = 0;

IMotorController* _MotorControllerInterface = 0;

ICharger* _ChargerInterface = 0;

void Kernel::Init()
{

	bool interfaceSpeedSensor = false;
	bool interfaceLogicalInputs = false;
	bool interfaceTemperatureSensor = false;
	bool interfaceCharger = false;

#if (MODE == MODE_FLATPACK2)
	//This mode does not support ControllerInterface
	interfaceCharger = true;
	_ChargerInterface = new ChargerFlatPack2();
#else
	interfaceSpeedSensor = true;
	interfaceLogicalInputs = true;
	interfaceTemperatureSensor = true;
	interfaceCharger = true;
	_ChargerInterface = new ChargerSaeJ1939();
#if (MODE == MODE_CURTIS_SDO)
	_MotorControllerInterface = new CanCurtisAdapterSdo(38);
#else
#if (MODE == MODE_CURTIS_PDO)
	_MotorControllerInterface = new CanCurtisAdapter(38);
#else
#if (MODE == MODE_TOS)
	_MotorControllerInterface = new CanTosAdapter();
#else
#if (MODE == MODE_KELLY)
	_MotorControllerInterface = new CanKellyAdapter();
#else
#if	(MODE == MODE_SNIFFER)
	_MotorControllerInterface = 0;
	_CanSniffer = new CanSniffer(_Kernel);
#endif //MODE_SNIFFER
#endif //MODE_KELLY
#endif //MODE_TOS
#endif //MODE_CURTIS_PDO
#endif //MODE_CURTIS_SDO
#endif //MODE_FLATPACK2
	//

	if (interfaceSpeedSensor)
		_ProtocolHost = new ProtocolHost(EmkAddr::SpeedSensor);
	//_ProtocolHost->AddSelfAddr(EmkAddr::CurrentSensor);
	//_ProtocolHost->AddSelfAddr(EmkAddr::VoltageSensor);
	if (interfaceLogicalInputs)
		_ProtocolHost->AddSelfAddr(EmkAddr::LogicalInputs);
	if (interfaceTemperatureSensor)
		_ProtocolHost->AddSelfAddr(EmkAddr::TemperatureSensor);
	if (interfaceCharger)
		_ProtocolHost->AddSelfAddr(EmkAddr::Charger);

	_ProtocolHost->DestAddr(EmkAddr::Host);


	_ReceiveMetodHost = new ReceiveMetodHost();

	_SendMetodHost = new SendMetodHost();


}

int32_t _KernelTicks = 0;
int32_t _ChargerTicks = 0;

#if (MODE==MODE_SNIFFER)
void Kernel::Tick()
{
	uint8_t b = 0;
	for (;Hal::UsartExt->Receive(&b, 1) > 0;)
	{ //add data processing if you need
	}

	if (Hal::GetSpendTicks(_KernelTicks) < Hal::GetTicksInMilliSecond() * 1000)
		return;

	Hal::LedBlue(!Hal::LedBlue());

	Hal::UsartExt->Send("Hertbeat %i\n\r", Hal::GetTickCount());



	_KernelTicks = Hal::GetTickCount();
}
#else //MODE_SNIFFER
void Kernel::Tick()
{

	uint8_t b = 0;
	uint8_t* data = 0;
	uint8_t len = 0;
	//for (;Hal::UsartExt->Receive(&b, 1) > 0;)
	for (;Hal::UsartExt->Receive(b);)
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

	//Hal::LedBlue(!Hal::LedBlue());

	if (_MotorControllerInterface)
	{
		//Hal::UsartExt->Send("R:%i,C:%i,CT:%i,MT:%i\n\r", _MotorControllerInterface->Rpm(), _MotorControllerInterface->Current(), _MotorControllerInterface->TempConstroller(), _MotorControllerInterface->TempMotor());
	}

	if ( Hal::GetSpendTicks(_ChargerTicks) > Hal::GetTicksInSecond() * 2)
		if (_ChargerInterface)
		{
			_ChargerInterface->TurnCharger(false);
			_ChargerInterface->SetCurrent(0);
			_ChargerInterface->SetVoltage(0);
			_ChargerInterface->SetOverVoltage(0);
		}

	_KernelTicks = Hal::GetTickCount();
}
#endif //MODE_SNIFFER


void Kernel::_ProcessDataPacket()
{

	auto mNum = _ReceiveMetodHost->GetMetodNumber();

	switch (_ProtocolHost->PacketAddr())
	{
	case EmkAddr::SpeedSensor:
		if ((InterfaceMetodsSpeedSens)mNum == InterfaceMetodsSpeedSens::RpmGet)
		{
			_ResponseRpm();
			break;
		};
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			_ResponsePing();
			break;
		};
		break;
	case EmkAddr::VoltageSensor:
		if ((InterfaceMetodsVoltSens)mNum == InterfaceMetodsVoltSens::VoltageGet)
		{
			_ResponseVoltage();
			break;
		}
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			_ResponsePing();
			break;
		};
		break;
	case EmkAddr::CurrentSensor:
		if ((InterfaceMetodsCurrSens)mNum == InterfaceMetodsCurrSens::CurrentGet)
		{
			_ResponseCurrent();
			break;
		}
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			_ResponsePing();
			break;
		};

		break;
	case EmkAddr::LogicalInputs:
		if ((InterfaceMetodsLogicalInputs)mNum == InterfaceMetodsLogicalInputs::InputsStateGet)
		{
			_ResponseInputs();
			break;
		}
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			_ResponsePing();
			break;
		};
		break;

	case EmkAddr::TemperatureSensor:
		if ((InterfaceMetodsTempSens)mNum == InterfaceMetodsTempSens::TemperatureGet)
		{
			_ResponseTemperature();
			break;
		}
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			_ResponsePing();
			break;
		};
		break;

	case EmkAddr::Charger:
		if (_ChargerInterface == NULL)
			break;
		if ((InterfaceMetodsCharger)mNum == InterfaceMetodsCharger::ProcessCharging)
		{
			bool boolVal = false;
			unsigned short usVal = 0;
			if (!_ReceiveMetodHost->GetArgumentBool(0, boolVal))
				break;
			_ChargerInterface->TurnCharger(boolVal);
			if (!_ReceiveMetodHost->GetArgumentUshort(1, usVal))
				break;
			_ChargerInterface->SetCurrent(usVal);
			if (!_ReceiveMetodHost->GetArgumentUshort(2, usVal))
				break;
			_ChargerInterface->SetVoltage(usVal);

			if (_ReceiveMetodHost->GetArgumentUshort(3, usVal))
				_ChargerInterface->SetOverVoltage(usVal);

			_ChargerTicks = Hal::GetTickCount();
			Hal::LedBlue(boolVal);

			_ResponseProcessCharging();

			break;
		}
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			_ResponsePing();
			break;
		};
		break;

	default:
		break;
	};

}

volatile int _ReceivedPings = 0;

void Kernel::_ResponsePing()
{
	_SendMetodHost->InitNewMetod((uint8_t)EmkMetods::Ping);

	_SendMetodHost->AddArgumentBool(true);

	//version
	_SendMetodHost->AddArgumentByte(2);

	_ReceivedPings++;

	_SendData();
}

void Kernel::_ResponseRpm()
{
	if (_MotorControllerInterface == 0)
		return;

	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsSpeedSens::RpmGet);

	_SendMetodHost->AddArgumentUshort(_MotorControllerInterface->Rpm());

	_SendData();
}

void Kernel::_ResponseVoltage()
{
	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsVoltSens::VoltageGet);

	_SendMetodHost->AddArgumentShort(_MotorControllerInterface->Voltage());

	_SendData();
}

void Kernel::_ResponseCurrent()
{
	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsCurrSens::CurrentGet);

	_SendMetodHost->AddArgumentShort(_MotorControllerInterface->Current());

	_SendData();
}

void Kernel::_ResponseInputs()
{
	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsLogicalInputs::InputsStateGet);

	_SendMetodHost->AddArgumentUshort(_MotorControllerInterface->Switches());

	_SendData();
}

void Kernel::_ResponseTemperature()
{
	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsTempSens::TemperatureGet);

	_SendMetodHost->AddArgumentShort(_MotorControllerInterface->TempConstroller());

	_SendMetodHost->AddArgumentShort(_MotorControllerInterface->TempMotor());

	_SendData();
}

void Kernel::_ResponseProcessCharging()
{
	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsCharger::ProcessCharging);

	_SendMetodHost->AddArgumentByte(_ChargerInterface->GetChargerFlags());

	_SendMetodHost->AddArgumentShort(_ChargerInterface->GetCurrent());

	_SendMetodHost->AddArgumentShort(_ChargerInterface->GetVoltage());

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

