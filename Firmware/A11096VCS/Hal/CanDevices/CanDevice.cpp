/*
 * CanDevice.cpp
 *
 *  Created on: Sep 15, 2017
 *      Author: banz
 */

#include "CanDevice.h"
#include "Hal.h"
#include <string.h>

CAN_HandleTypeDef* CanDevice::_hCan = 0;

#define CanDeviceMaxCount 5

static CanDevice* _CanDevices[CanDeviceMaxCount] = {0};
static int16_t _CanBusErrorCount = 0;
static uint32_t _CanBusErrorBits = 0;
static uint32_t _UsedFilterBanks = 0;
#define _FilterFIFOAssignment 0

extern "C"
{
	void Error_Handler(void);
}

CanDevice::CanDevice(uint32_t extIdFilter, uint32_t idDest, uint16_t recieveTimeoutMs, CanMode canMode)
{
	int8_t i = 0;
	for (; i < CanDeviceMaxCount; i++)
		if (_CanDevices[i] == 0)
		{
			_CanDevices[i] = this;
			break;
		}

	_ExtIdFilter = extIdFilter;
	_IdDest = idDest;
	_RecieveTimeoutMs = recieveTimeoutMs;
	_Ticks = Hal::GetTickCount();
	_LastReceivingTime = 0;
	_CanDeviceState = cdsNone;

	_CanMode = canMode;

	_RxStdId = 0;
	_RxExtId = 0;

	_Enable = true;

	memset(_TxData, 0, sizeof(_TxData));

	_CanBusErrorCount = 0;
	_CanBusErrorBits = 0;

	HAL_CAN_Stop(CanDevice::_hCan);

	CAN_FilterTypeDef sFilterConfig = {0};
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = (uint16_t)(extIdFilter << 16);
	sFilterConfig.FilterIdLow = (uint16_t)extIdFilter;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = _FilterFIFOAssignment;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.FilterBank = 32;
	for (int8_t i = 0; i < 32; i++)
		if ((_UsedFilterBanks & (1 << i)) == 0)
		{
			_UsedFilterBanks |= (1 << i);
			sFilterConfig.FilterBank = i;
			break;
		}
	sFilterConfig.SlaveStartFilterBank = 0;
	HAL_CAN_ConfigFilter(CanDevice::_hCan, &sFilterConfig);

	HAL_CAN_Start(CanDevice::_hCan);


}

CanDevice::~CanDevice()
{
	int8_t i = 0;
	for (; i < CanDeviceMaxCount; i++)
		if (_CanDevices[i] == this)
		{
			_CanDevices[i] = 0;
			break;
		}
}

bool CanDevice::_CanSilentMode = true;

void CanDevice::Init(CAN_HandleTypeDef* hCan, Speeds speed, bool silentMode)
{
	_CanSilentMode = silentMode;

	CanDevice::_hCan = hCan;
	hCan->Instance = CAN1;
	hCan->Init.Prescaler = 18;
	hCan->Init.Mode = _CanSilentMode ? CAN_MODE_SILENT : CAN_MODE_NORMAL;
	hCan->Init.SyncJumpWidth = CAN_SJW_1TQ;
	hCan->Init.TimeSeg1 = CAN_BS1_13TQ;  /*BS1*/
	hCan->Init.TimeSeg2 = CAN_BS2_2TQ; /*BS2*/
	hCan->Init.TimeTriggeredMode = DISABLE; /*TTCM*/
	hCan->Init.AutoBusOff = ENABLE; /*ABOM*/
	hCan->Init.AutoWakeUp = DISABLE; /*AWUM*/
	hCan->Init.AutoRetransmission = DISABLE; /*NART*/
	hCan->Init.ReceiveFifoLocked = DISABLE; /*RFLM*/
	hCan->Init.TransmitFifoPriority = DISABLE; /*TXFP*/

	switch(speed)
	{
	case Speeds::s125k:
		hCan->Init.Prescaler = 18; //125kbs
		break;
	case Speeds::s250k:
		hCan->Init.Prescaler = 9; //250kbs
		break;
	case Speeds::s500k:
		hCan->Init.Prescaler = 4; //500kbs
		hCan->Init.TimeSeg1 = CAN_BS1_15TQ;  /*BS1*/
		break;
	default:
		hCan->Init.Prescaler = 18; //125kbs
		break;
	}

	if (HAL_CAN_Init(hCan) != HAL_OK)
	{
		Error_Handler();
	}


}

void CanDevice::Tick()
{
	auto messNum = HAL_CAN_GetRxFifoFillLevel(CanDevice::_hCan, _FilterFIFOAssignment);
	if (messNum)
	{
		CAN_RxHeaderTypeDef rxHeader = {0};
		uint8_t rxData[8] = {0};
		HAL_StatusTypeDef rxRes =
				HAL_CAN_GetRxMessage(CanDevice::_hCan, _FilterFIFOAssignment, &rxHeader, rxData);

		if (rxRes != HAL_OK )
			return;

		for (int8_t i = 0; i < CanDeviceMaxCount; i++)
		{
			if (_CanDevices[i] == 0)
				continue;
			_CanDevices[i]->ProcessMess(rxHeader, rxData);
		};

		return;
	}

	for (int8_t i = 0; i < CanDeviceMaxCount; i++)
	{
		if (_CanDevices[i] == 0)
			continue;
		_CanDevices[i]->OnTick();
	}

}



bool CanDevice::Transmit()
{

	/*if (!sendImmediately)
		if (_SendIntervalMs == 0 || Hal::GetSpendTicks(_Ticks) < Hal::GetTicksInMilliSecond() * _SendIntervalMs)
		{
			if (HAL_CAN_GetState(_hCan) == HAL_CAN_STATE_READY)
				HAL_CAN_RxFifo0FullCallback(_hCan);
			return false;
		}

	if (HAL_CAN_GetState(_hCan) != HAL_CAN_STATE_READY &&
		HAL_CAN_GetState(_hCan) != HAL_CAN_STATE_)
		return false;*/

	if (HAL_CAN_GetTxMailboxesFreeLevel(_hCan) < 1)
		return false;

	CAN_TxHeaderTypeDef txHndl;
	txHndl.TransmitGlobalTime = DISABLE;
	txHndl.RTR = CAN_RTR_DATA;
	txHndl.DLC = 8;

	if (_CanMode == CanMode::Extended)
	{
		txHndl.ExtId = _IdDest;
		txHndl.StdId = 0;
		txHndl.IDE = CAN_ID_EXT;
	}
	else if (_CanMode == CanMode::Standart)
	{
		txHndl.StdId = _IdDest;
		txHndl.ExtId = 0;
		txHndl.IDE = CAN_ID_STD;
	}
	else
		return false;

	uint32_t txMailbox = 0;

	if (HAL_OK != HAL_CAN_AddTxMessage(_hCan, &txHndl, _TxData, &txMailbox))
		return false;

	return true;
}

CanDeviceStates CanDevice::_GetState()
{
	return _CanDeviceState;
}

int32_t _ReceivedPackets = 0;

int32_t CanDevice::ReceivedPackets()
{
	return _ReceivedPackets;
}

bool CanDevice::ProcessMess(const CAN_RxHeaderTypeDef& rxHeader, uint8_t data[])
{
	return false;
}

void CanDevice::OnTick()
{
}

void CanDevice::_Reinit()
{
	HAL_CAN_MspDeInit(_hCan);
	for (int8_t i = 0; i < CanDeviceMaxCount; i++)
	{
		if (_CanDevices[i] == 0)
			continue;
		_CanDevices[i]->_CanDeviceState = cdsNone;
	}
	HAL_CAN_MspInit(_hCan);
}

int16_t CanDevice::Errors(uint32_t& errBits)
{
	errBits = _CanBusErrorBits;
	return _CanBusErrorCount;
}

void CanDevice::ResetErrors()
{
	_CanBusErrorBits = 0;
	_CanBusErrorCount = 0;
}


/*void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{

	//if (_hCan->ErrorCode != 0)
	//{
	for (int8_t i = 0; i < CanDeviceMaxCount; i++)
	{
		if (_CanDevices[i] == 0)
			continue;
		_CanDevices[i]->_CanDeviceState = cdsBusError;
	}

	_CanBusErrorCount++;

	_CanBusErrorBits |= hcan->ErrorCode;

}


void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{

	_ReceivedPackets++;

	for (int8_t i = 0; i < CanDeviceMaxCount; i++)
	{
		if (_CanDevices[i] == 0)
			continue;
		if (_CanDevices[i]->_ExtIdFilter != 0)
		{ //Если девайс ловит один пакет
			if (hcan->pRxMsg->ExtId)
			{
				if (_CanDevices[i]->_ExtIdFilter != hcan->pRxMsg->ExtId)
					continue;
			}
			else
				if (_CanDevices[i]->_ExtIdFilter != hcan->pRxMsg->StdId)
					continue;
		}
		else
		{ //Если девайс ловит много пакетов
			uint32_t pId = hcan->pRxMsg->ExtId ? hcan->pRxMsg->ExtId : hcan->pRxMsg->StdId;
			if (!_CanDevices[i]->ProcessMessId(pId, hcan->pRxMsg->Data))
				continue;

			if (hcan->pRxMsg->ExtId)
				_CanDevices[i]->_RxExtId = pId;
			else
				_CanDevices[i]->_RxStdId = pId;
		}

		if (!_CanDevices[i]->_RxDataLock)
		{
			_CanDevices[i]->_RxDataLock = true;
			memcpy(_CanDevices[i]->_RxData, hcan->pRxMsg->Data, sizeof(_CanDevices[i]->_RxData));
			_CanDevices[i]->_RxDataLock = false;
		}

		_CanDevices[i]->_LastReceivingTime = Hal::GetTickCount();
		_CanDevices[i]->_CanDeviceState = cdsWorking;
	}


	HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
}*/
