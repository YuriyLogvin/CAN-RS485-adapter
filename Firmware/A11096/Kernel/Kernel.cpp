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

#include "CanDevices/CanCurtisAdapter.h"
#include "CanDevices/CanTosAdapter.h"
#include "CanDevices/CanCurtisAdapterSdo.h"
#include "CanDevices/CanKellyAdapter.h"


Kernel* _Kernel = NULL;
ProtocolHost* _ProtocolHost;
ReceiveMetodHost* _ReceiveMetodHost;
SendMetodHost* _SendMetodHost;

void BmsKernelInit()
{

#if (MODE == MODE_CURTIS_SDO)
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

void Kernel::Init()
{
	_ProtocolHost = new ProtocolHost(EmkAddr::SpeedSensor);
	//_ProtocolHost->AddSelfAddr(EmkAddr::CurrentSensor);
	//_ProtocolHost->AddSelfAddr(EmkAddr::VoltageSensor);
	_ProtocolHost->AddSelfAddr(EmkAddr::LogicalInputs);
	_ProtocolHost->AddSelfAddr(EmkAddr::TemperatureSensor);
	_ProtocolHost->DestAddr(EmkAddr::Host);


	_ReceiveMetodHost = new ReceiveMetodHost();

	_SendMetodHost = new SendMetodHost();

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
	//

}

int32_t _KernelTicks = 0;

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

	Hal::LedBlue(!Hal::LedBlue());

	if (_MotorControllerInterface)
	{
		//Hal::UsartExt->Send("R:%i,C:%i,CT:%i,MT:%i\n\r", _MotorControllerInterface->Rpm(), _MotorControllerInterface->Current(), _MotorControllerInterface->TempConstroller(), _MotorControllerInterface->TempMotor());
	}

	Hal::UsartExt->Send("Hertbeat1 %i\n\r", Hal::GetTickCount());

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

