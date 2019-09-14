/*
 * Stm32UsartRecever.cpp
 *
 *  Created on: Feb 26, 2017
 *      Author: banz
 */

#include "Stm32UsartDma.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

extern "C"
{
	void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
	void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
}

struct Huart2Stm32UsartDma
{
	UART_HandleTypeDef* Huart;
	Stm32UsartDma * Stm32UsartDmaObj;
};

#define _huartsCount 3
Huart2Stm32UsartDma _huarts[_huartsCount] = {0};

Stm32UsartDma::Stm32UsartDma(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize)
{
	_Init(huart, rcvBufSize, sendBufSize, 0, 0);
}

Stm32UsartDma::Stm32UsartDma(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize, GPIO_TypeDef* txEnPort, uint16_t txEnPin)
{
	_Init(huart, rcvBufSize, sendBufSize, txEnPort, txEnPin);
}

#define __UsartRecvBuffSz 256
uint8_t __UsartRecvBuff[__UsartRecvBuffSz*2];

void Stm32UsartDma::_Init(UART_HandleTypeDef* huart, uint16_t rcvBufSize, uint16_t sendBufSize, GPIO_TypeDef* txEnPort, uint16_t txEnPin)
{
	_Huart = huart;
	_SendBufSize = sendBufSize;

	_TxEnPort = txEnPort;
	_TxEnPin = txEnPin;

	_InTxMode = false;

	_UsartSendBuff = (uint8_t*)malloc(_SendBufSize);
	memset(_UsartSendBuff, 0xff, _SendBufSize);

	_ErrorCount = 0;

	_UsartReceivePos = 0;
	if (huart->hdmatx->ChannelIndex != 12) //(rcvBufSize == 0x200)
	{
		_RcvBufSize = rcvBufSize;
		_UsartRecvBuff = (uint8_t*)malloc(_RcvBufSize*2);
	}
	else
	{
		_RcvBufSize = __UsartRecvBuffSz;
		_UsartRecvBuff = __UsartRecvBuff;
	}

	memset(_UsartRecvBuff, 0xff, _RcvBufSize*2);
	HAL_UART_Receive_DMA(_Huart, _UsartRecvBuff, _RcvBufSize);

	for (int8_t i = 0; i <_huartsCount; i++)
	{
		if (_huarts[i].Stm32UsartDmaObj != 0 )
			continue;

		_huarts[i].Huart = huart;
		_huarts[i].Stm32UsartDmaObj = this;
		break;
	}

}

void Stm32UsartDma::Restart()
{
	memset(_UsartSendBuff, 0xff, _SendBufSize);
	memset(_UsartRecvBuff, 0xff, _RcvBufSize*2);

	_ErrorCount = 0;
	_InTxMode = false;
	_UsartReceivePos = 0;

	HAL_UART_DMAStop(_Huart);

	HAL_UART_Receive_DMA(_Huart, _UsartRecvBuff, _RcvBufSize);

}

uint16_t Stm32UsartDma::Deb()
{
	return _UsartReceivePos;
}

uint8_t Stm32UsartDma::Deb1()
{
	return _UsartRecvBuff[0];
}

Stm32UsartDma::~Stm32UsartDma() {
	free(_UsartSendBuff);
	free(_UsartRecvBuff);
}

bool Stm32UsartDma::Send(const char* format, ...)
{
	if (_Huart->gState != HAL_UART_STATE_READY)
		return false;

	va_list args;
	va_start(args, format);
	vsnprintf((char*)_UsartSendBuff, _SendBufSize, format, args);
	va_end(args);

	if (_TxEnPort)
		HAL_GPIO_WritePin(_TxEnPort, _TxEnPin, GPIO_PIN_SET);

	/*for (int i = 0; i < 2000; i++)
	{
		i++;
	}*/

	HAL_UART_Transmit_DMA(_Huart, _UsartSendBuff, strlen((char*)_UsartSendBuff));

}

bool Stm32UsartDma::Redy4Send()
{
	return (_Huart->gState & HAL_UART_STATE_BUSY_TX & 0xf) ? false : true;
}

bool Stm32UsartDma::Send(uint8_t* data, uint16_t dataLen)
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

void Stm32UsartDma::_TxCmplt()
{
	if (_TxEnPort)
	{
		_InTxMode = false;
		HAL_GPIO_WritePin(_TxEnPort, _TxEnPin, GPIO_PIN_RESET);
		RxStart();
	}
}

void Stm32UsartDma::RxStop()
{
	//HAL_UART_DMAStop(_Huart);
	HAL_UART_AbortReceive(_Huart);
}

void Stm32UsartDma::RxStart()
{
	_UsartReceivePos = 0;
	//memset(_UsartRecvBuff, 0xff, _RcvBufSize*2);
	for (uint16_t i = 0; i < _RcvBufSize; i++)
	{
		if (_UsartRecvBuff[i*2+1] == 0xff)
			break;
		_UsartRecvBuff[i*2+1] = 0xff;
	}
	HAL_UART_Receive_DMA(_Huart, _UsartRecvBuff, _RcvBufSize);
}

uint16_t Stm32UsartDma::Receive(uint8_t* data, uint16_t dataBufSize)
{
	if (_TxEnPort && _InTxMode)
		return 0;

	uint8_t received = 0;
	for (;1;)
	{
		if (_UsartRecvBuff[_UsartReceivePos*2+1] == 0xff)
		{
			if (_Huart->RxState == HAL_UART_STATE_READY)
			{ //При получении ошибок останавливается прием. Толкнуть прием. Проблема аппаратная.
				if (_TxEnPort)
					HAL_GPIO_WritePin(_TxEnPort, _TxEnPin, GPIO_PIN_RESET);
				RxStart();
			}
			break;
		}

		data[received++] = _UsartRecvBuff[_UsartReceivePos*2];

		_UsartRecvBuff[_UsartReceivePos*2+1] = 0xff;

		_UsartReceivePos++;
		if (_UsartReceivePos >= _RcvBufSize )
			_UsartReceivePos = 0;

		if (received >= dataBufSize)
			break;

	}

	return received;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	for (int8_t i = 0; i <_huartsCount; i++)
	{
		if (_huarts[i].Huart != huart )
			continue;

		_huarts[i].Stm32UsartDmaObj->_TxCmplt();
		break;
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	for (int8_t i = 0; i <_huartsCount; i++)
	{
		if (_huarts[i].Huart != huart )
			continue;

		_huarts[i].Stm32UsartDmaObj->_ErrorCount++;
		break;
	}

}


