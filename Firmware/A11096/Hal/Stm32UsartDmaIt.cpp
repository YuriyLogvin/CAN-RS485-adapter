/*
 * Stm32UsartRecever.cpp
 *
 *  Created on: Feb 26, 2017
 *      Author: banz
 */

#include "Stm32UsartDmaIt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

extern "C"
{
	void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
	void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
}

struct Huart2Stm32UsartDmaIt
{
	UART_HandleTypeDef* Huart;
	Stm32UsartDmaIt * Stm32UsartDmaItObj;
};

#define _huartsCount 3
Huart2Stm32UsartDmaIt _huarts[_huartsCount] = {0};

Stm32UsartDmaIt::Stm32UsartDmaIt(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize)
{
	_Init(huart, rcvBufSize, sendBufSize, 0, 0);
}

Stm32UsartDmaIt::Stm32UsartDmaIt(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize, GPIO_TypeDef* txEnPort, uint16_t txEnPin)
{
	_Init(huart, rcvBufSize, sendBufSize, txEnPort, txEnPin);
}

#define __UsartRecvBuffSz 256
uint8_t __UsartRecvBuff[__UsartRecvBuffSz*2];

void Stm32UsartDmaIt::_Init(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize, GPIO_TypeDef* txEnPort, uint16_t txEnPin)
{
	_Huart = huart;
	_SendBufSize = sendBufSize;

	_TxEnPort = txEnPort;
	_TxEnPin = txEnPin;

	_InTxMode = false;

	_UsartSendBuff = (uint8_t*)malloc(_SendBufSize);
	memset(_UsartSendBuff, 0xff, _SendBufSize);

	_ReceiveBuffer = new ReceiveBuffer(64);

	_ErrorCount = 0;

	for (int8_t i = 0; i <_huartsCount; i++)
	{
		if (_huarts[i].Stm32UsartDmaItObj != 0 )
			continue;

		_huarts[i].Huart = huart;
		_huarts[i].Stm32UsartDmaItObj = this;
		break;
	}

	HAL_UART_Receive_IT(_Huart, (uint8_t*)&_Rbyte, 1);

}

void Stm32UsartDmaIt::Restart()
{
	memset(_UsartSendBuff, 0xff, _SendBufSize);

	_ErrorCount = 0;
	_InTxMode = false;

	HAL_UART_DMAStop(_Huart);

	HAL_UART_Receive_IT(_Huart, (uint8_t*)&_Rbyte, 1);
}

Stm32UsartDmaIt::~Stm32UsartDmaIt() {
	free(_UsartSendBuff);
	delete _ReceiveBuffer;
}

bool Stm32UsartDmaIt::Send(const char* format, ...)
{
	if (_Huart->gState != HAL_UART_STATE_READY)
		return false;

	va_list args;
	va_start(args, format);
	vsnprintf((char*)_UsartSendBuff, _SendBufSize, format, args);
	va_end(args);

	if (_TxEnPort)
		HAL_GPIO_WritePin(_TxEnPort, _TxEnPin, GPIO_PIN_SET);

	HAL_UART_Transmit_DMA(_Huart, _UsartSendBuff, strlen((char*)_UsartSendBuff));

	return true;
}

bool Stm32UsartDmaIt::Redy4Send()
{
	return (_Huart->gState & HAL_UART_STATE_BUSY_TX & 0xf) ? false : true;
}

bool Stm32UsartDmaIt::Send(uint8_t* data, uint16_t dataLen)
{
	if (_Huart->gState != HAL_UART_STATE_READY)
		return false;

	if (dataLen > _SendBufSize)
		return false;

	memcpy(_UsartSendBuff, data, dataLen);

	if (_TxEnPort)
	{ //Если не останавливать прием - прилетает левый бит, портящий данные
		RxStop();
		_InTxMode = true;
		HAL_GPIO_WritePin(_TxEnPort, _TxEnPin, GPIO_PIN_SET);
	}

	HAL_UART_Transmit_DMA(_Huart, _UsartSendBuff, dataLen);
	return true;
}

void Stm32UsartDmaIt::_TxCmplt()
{
	if (_TxEnPort && _InTxMode)
	{
		_InTxMode = false;
		HAL_GPIO_WritePin(_TxEnPort, _TxEnPin, GPIO_PIN_RESET);
		RxStart();
	}
}

void Stm32UsartDmaIt::_RxCmplt()
{
	_ReceiveBuffer->Push(_Rbyte);
	HAL_UART_Receive_IT(_Huart, (uint8_t*)&_Rbyte, 1);
}

void Stm32UsartDmaIt::RxStop()
{
	//HAL_UART_AbortReceive(_Huart);
}

void Stm32UsartDmaIt::RxStart()
{
	//HAL_UART_Receive_DMA(_Huart, _UsartRecvBuff, _RcvBufSize);
}

bool Stm32UsartDmaIt::Receive(uint8_t& data)
{
	bool res;
	__disable_irq();
	res = _ReceiveBuffer->Get(data);
	__enable_irq();

	if (_Huart->RxState == HAL_UART_STATE_READY)
		HAL_UART_Receive_IT(_Huart, (uint8_t*)&_Rbyte, 1);
	return res;
}

uint16_t Stm32UsartDmaIt::Receive(uint8_t* data, uint16_t dataBufSize)
{
	//if (_TxEnPort && _InTxMode)
	//	return 0;

	uint16_t i = 0;

	for (; i < dataBufSize;i++)
	{
		if (!Receive(data[i]))
			break;
	}

	return i;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	for (int8_t i = 0; i <_huartsCount; i++)
	{
		if (_huarts[i].Huart != huart )
			continue;

		_huarts[i].Stm32UsartDmaItObj->_TxCmplt();
		break;
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	for (int8_t i = 0; i <_huartsCount; i++)
	{
		if (_huarts[i].Huart != huart )
			continue;

		_huarts[i].Stm32UsartDmaItObj->_ErrorCount++;

		if (_huarts[i].Huart->ErrorCode)
			continue;

		break;
	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	for (int8_t i = 0; i < _huartsCount; i++)
	{
		if (_huarts[i].Huart != huart )
			continue;

		_huarts[i].Stm32UsartDmaItObj->_RxCmplt();
	}
}



