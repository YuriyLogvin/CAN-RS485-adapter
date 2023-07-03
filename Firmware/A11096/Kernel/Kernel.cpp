/*
 * Kernel.cpp
 *
 *  Created on: 6 вер. 2019 р.
 *      Author: banz
 */

#include <sys/_stdint.h>
#include "main.h"
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
#include "InterfaceMetodsChiller.h"
#include "InterfaceMetodsExtBms.h"
#include "InterfaceMetodsCanEmulator.h"

#include "CanDevices/CanCurtisAdapter.h"
#include "CanDevices/CanTosAdapter.h"
#include "CanDevices/CanCurtisAdapterSdo.h"
#include "CanDevices/CanKellyAdapter.h"
#include "CanDevices/ChargerSaeJ1939.h"
#include "CanDevices/ChargerFlatPack2.h"

#include "CanDevices/IChiller.h"
#include "CanDevices/ChillerSongZ.h"

#include "CanDevices/IExtBms.h"
#include "CanDevices/ExtBmsPylontech.h"

#include <CanDevices/CanEmulator.h>


Kernel* _Kernel = NULL;
ProtocolHost* _ProtocolHost;
ReceiveMetodHost* _ReceiveMetodHost;
SendMetodHost* _SendMetodHost;

void BmsKernelInit()
{

#if (MODE == MODE_CURTIS_SDO || MODE == MODE_FLATPACK2 || MODE == MODE_CHILLER || MODE == MODE_EXT_BMS || MODE == MODE_EMULATOR)
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

IChiller* _ChillerInterface = 0;

IExtBms* _ExtBms = 0;

CanEmulator* _CanEmulator = 0;

void Kernel::Init()
{

	bool interfaceSpeedSensor = false;
	bool interfaceLogicalInputs = false;
	bool interfaceTemperatureSensor = false;
	bool interfaceCharger = false;
	bool interfaceChiller = false;
	bool interfaceExtBms = false;
	bool interfaceCanEmulator = false;


#if (MODE == MODE_EXT_BMS)
	interfaceExtBms = true;
	_ExtBms = new ExtBmsPylontech();
#else //MODE_EXT_BMS
#if (MODE == MODE_FLATPACK2)
	//This mode does not support ControllerInterface
	interfaceCharger = true;
	_ChargerInterface = new ChargerFlatPack2();
#else //MODE_FLATPACK2
#if	(MODE == MODE_SNIFFER)
	_MotorControllerInterface = 0;
	_CanSniffer = new CanSniffer(_Kernel);
#else //MODE_SNIFFER
#if (MODE == MODE_CHILLER)
	_ChillerInterface = new ChillerSongZ();
	interfaceChiller = true;
#else
#if (MODE == MODE_EMULATOR)
	_CanEmulator = new CanEmulator();
	interfaceCanEmulator = true;
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
#endif //MODE_KELLY
#endif //MODE_TOS
#endif //MODE_CURTIS_PDO
#endif //MODE_CURTIS_SDO
#endif //MODE_EMULATOR
#endif //MODE_CHILLER
#endif //MODE_SNIFFER
#endif //MODE_FLATPACK2
#endif //MODE_EXT_BMS

	if (interfaceCanEmulator)
		_ProtocolHost = new ProtocolHost(EmkAddr::CanEmulator);

	if (interfaceExtBms)
		_ProtocolHost = new ProtocolHost(EmkAddr::ExtBms);

	if (interfaceChiller)
		_ProtocolHost = new ProtocolHost(EmkAddr::Chiller);

	if (interfaceSpeedSensor)
		_ProtocolHost = new ProtocolHost(EmkAddr::SpeedSensor);

	if (interfaceLogicalInputs)
	{
		if (_ProtocolHost)
			_ProtocolHost->AddSelfAddr(EmkAddr::LogicalInputs);
		else
			_ProtocolHost = new ProtocolHost(EmkAddr::LogicalInputs);
	}

	if (interfaceTemperatureSensor)
	{
		if (_ProtocolHost)
			_ProtocolHost->AddSelfAddr(EmkAddr::TemperatureSensor);
		else
			_ProtocolHost = new ProtocolHost(EmkAddr::TemperatureSensor);
	}

	if (interfaceCharger)
	{
		if (_ProtocolHost)
		{
			if (Hal::GetSwValue(1))
				_ProtocolHost->AddSelfAddr(EmkAddr::Charger2);
			else
				_ProtocolHost->AddSelfAddr(EmkAddr::Charger);
		}
		else
		{
			if (Hal::GetSwValue(1))
				_ProtocolHost = new ProtocolHost(EmkAddr::Charger2);
			else
				_ProtocolHost = new ProtocolHost(EmkAddr::Charger);
		}
	}

	if (_ProtocolHost)
	{
		_ProtocolHost->DestAddr(EmkAddr::Host);
		_ReceiveMetodHost = new ReceiveMetodHost();
		_SendMetodHost = new SendMetodHost();
	}
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

	if (_CanSniffer)
		_CanSniffer->Tick();

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
		//Hal::UsartExt->Send("%x", b);
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

	Hal::UsartExt->Send("123\n\r");
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

	if (_CanEmulator)
	{

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
	case EmkAddr::Charger2:
		if (_ChargerInterface == NULL)
			break;
		if (Hal::GetSwValue(1) && _ProtocolHost->PacketAddr() != EmkAddr::Charger2)
			break;
		if (!Hal::GetSwValue(1) && _ProtocolHost->PacketAddr() != EmkAddr::Charger)
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
			Hal::LedBlue(!Hal::LedBlue());
			_ResponsePing();
			break;
		};
		break;
	case EmkAddr::Chiller:
		if (_ChillerInterface == NULL)
			break;
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			Hal::LedBlue(!Hal::LedBlue());
			_ResponsePing();
			break;
		};

		if ((InterfaceMetodsChiller)mNum == InterfaceMetodsChiller::ProcessChilling)
		{
			if (!_ProcessChilling())
				break;
			_ResponseChilling();
			break;
		}
		break;
	case EmkAddr::ExtBms:
	case EmkAddr::ExtBms2:
	case EmkAddr::ExtBms3:
		if (_ExtBms == NULL)
			break;
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			Hal::LedBlue(!Hal::LedBlue());
			_ResponsePing();
			break;
		};

		if ((InterfaceMetodsExtBms)mNum == InterfaceMetodsExtBms::GetParameter)
		{
			if (!_ProcessRequestExtBmsGetParam())
				break;
			//_ResponseChilling();
			//break;
		}
		break;
	case EmkAddr::CanEmulator:
		if (_CanEmulator == NULL)
			break;
		if ((EmkMetods)mNum == EmkMetods::Ping)
		{
			Hal::LedBlue(!Hal::LedBlue());
			_ResponsePing();
			break;
		};

		_ProcessRequestCanEmulator();

		break;
	default:
		break;
	};

}

bool Kernel::_ProcessRequestCanEmulator()
{
	if (_CanEmulator == NULL)
		return false;
	auto mNum = (InterfaceMetodsCanEmulator)_ReceiveMetodHost->GetMetodNumber();
	bool respondRes = false;
	uint32_t canId = 0;

	switch (mNum)
	{
	case InterfaceMetodsCanEmulator::AddFilter:
		if (!_ReceiveMetodHost->GetArgumentUint(0, canId))
			break;;
		_CanEmulator->FilterAdd(canId);
		respondRes = true;
		break;
	case InterfaceMetodsCanEmulator::DelFilter:
		if (!_ReceiveMetodHost->GetArgumentUint(0, canId))
			break;;
		_CanEmulator->FilterDel(canId);
		respondRes = true;
		break;
	case InterfaceMetodsCanEmulator::GetFilteredData:
	{
		if (!_ReceiveMetodHost->GetArgumentUint(0, canId))
			break;
		int32_t lastInterval = 0;
		uint8_t dataBuff[8] = {0};
		int32_t lastPacket = _CanEmulator->GetData(canId, dataBuff, lastInterval);
		_SendMetodHost->InitNewMetod((uint8_t)mNum);
		_SendMetodHost->AddArgumentUint(canId);
		_SendMetodHost->AddArgumentInt(lastPacket);
		_SendMetodHost->AddArgumentInt(lastInterval);
		_SendMetodHost->AddArgumentData(dataBuff, sizeof(dataBuff));
		_SendData();
		return true;
	}
	case InterfaceMetodsCanEmulator::SetTransmittedData:
	{
		if (!_ReceiveMetodHost->GetArgumentUint(0, canId))
			break;
		int32_t transmitInterval = 0;
		if (!_ReceiveMetodHost->GetArgumentInt(1, transmitInterval))
			break;
		uint8_t dataBuff[8] = {0};
		uint8_t dataBuffLen = 8;
		if (!_ReceiveMetodHost->GetArgumentData(2, dataBuff, dataBuffLen))
			break;
		_CanEmulator->SetData(canId, transmitInterval, dataBuff);
		respondRes = true;
		break;
	}
	case InterfaceMetodsCanEmulator::DelTransmittedData:
		if (!_ReceiveMetodHost->GetArgumentUint(0, canId))
			break;;
		_CanEmulator->DelData(canId);
		respondRes = true;
		break;
	}

	_SendMetodHost->InitNewMetod((uint8_t)mNum);

	_SendMetodHost->AddArgumentBool(respondRes);

	_SendData();

	return true;

}

bool Kernel::_ProcessRequestExtBmsGetParam()
{
	if (_ExtBms == NULL)
		return false;
	uint8_t paramName = 0;
	if (!_ReceiveMetodHost->GetArgumentByte(0, paramName))
		return false;
	int32_t liVal = 0;
	if (!_ExtBms->GetExtParam((IExtBms::ExtIds)paramName, liVal))
		return false;

	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsExtBms::GetParameter);

	_SendMetodHost->AddArgumentByte(paramName);

	_SendMetodHost->AddArgumentInt(liVal);

	_SendData();

	return true;
}

bool Kernel::_ProcessChilling()
{
	if (_ChillerInterface == NULL)
		return false;
	bool boolVal = false;
	short sVal = 0, sVal2 = 0;
	uint8_t ucVal = 0;
	if (!_ReceiveMetodHost->GetArgumentBool(0, boolVal))
		return false;
	_ChillerInterface->TurnOn(boolVal);

	if (!_ReceiveMetodHost->GetArgumentByte(1, ucVal))
		return false;
	_ChillerInterface->ControlMode((IChiller::ControlModes)ucVal);

	if (!_ReceiveMetodHost->GetArgumentByte(2, ucVal))
		return false;
	_ChillerInterface->ChillerMode((IChiller::ChillerModes)ucVal);

	if (!_ReceiveMetodHost->GetArgumentByte(3, ucVal))
		return false;
	_ChillerInterface->BatteryMode((IChiller::BatteryModes)ucVal);

	if (!_ReceiveMetodHost->GetArgumentByte(4, ucVal))
		return false;
	_ChillerInterface->HvMode((IChiller::HvModes)ucVal);

	if (!_ReceiveMetodHost->GetArgumentShort(5, sVal))
		return false;
	if (!_ReceiveMetodHost->GetArgumentShort(6, sVal2))
		return false;
	_ChillerInterface->TempHaveMinMaxp(sVal, sVal2);

	if (!_ReceiveMetodHost->GetArgumentShort(7, sVal))
		return false;
	_ChillerInterface->VoltageHave(sVal);

	if (!_ReceiveMetodHost->GetArgumentShort(8, sVal))
		return false;
	_ChillerInterface->TempTarget(sVal);

	return true;
}

void Kernel::_ResponseChilling()
{
	if (_ChillerInterface == NULL)
		return;

	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsChiller::ProcessChilling);

	_SendMetodHost->AddArgumentByte((uint8_t)_ChillerInterface->ControlMode());

	_SendMetodHost->AddArgumentByte((uint8_t)_ChillerInterface->ChillerMode());

	_SendMetodHost->AddArgumentByte((uint8_t)_ChillerInterface->BatteryMode());

	_SendMetodHost->AddArgumentByte((uint8_t)_ChillerInterface->HvMode());

	_SendMetodHost->AddArgumentShort(_ChillerInterface->TempIn());

	_SendMetodHost->AddArgumentShort(_ChillerInterface->TempOut());

	_SendMetodHost->AddArgumentByte(_ChillerInterface->COP());

	_SendMetodHost->AddArgumentUint(_ChillerInterface->ErrorCode());

	_SendData();
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


bool Kernel::Send(uint8_t* data, uint16_t len)
{
	return Hal::UsartExt->Send(data, len);
}

bool Kernel::IsBusy()
{
	return !Hal::UsartExt->Redy4Send();
}


