#include "bootloader.h"
#include <stdio.h>
#include <string.h>

#include "main.h"

#include "../Bootloader/xmodem.h"

char _StateFlashing = 0;

char ReceivePortion(unsigned char* portionData, short portionDataLen);

void _EraseFlash();

void _WriteFlashFinish();
void _WriteFlash(uint8_t* buf, uint8_t len);
char _WriteFlashStart(uint8_t* buf, uint8_t len);

void Jump2Prog();

uint8_t Boot(void)
{
	  char res;
	  _StateFlashing = 0;

	  res = XmProccess(ReceivePortion);
	  if (_StateFlashing && res == 0)
	  {
		  _WriteFlashFinish();
		  return 1;
	  }

	  return 0;
	  /*FlashFlags f = ReadFlags();
	  if (	f == ffFlashed ||
			f == ffChecked )
	  {
		  Jump2Prog();
	  }*/
}

#define _SendBufSize 256
#define _ReceiveBuffSize 256

#define _MainProgramPageAddress (FLASH_BASE | (_MainProgramPage * _FlashPageSize))
#define _ProrgrammFlashSettingAddress ((FLASH_BASE | (_FlashPageCount * _FlashPageSize)) - 16)

char ReceivePortion(unsigned char* portionData, short portionDataLen)
{
	if (!_StateFlashing)
	{
		_StateFlashing = 1;
		return _WriteFlashStart(portionData, portionDataLen);
	}
	else
		_WriteFlash(portionData, portionDataLen);

	return 0;
}

/*#include "..\Decrypt\CrcTool.c"*/



/* Содержимое файла:
 * Сигнатура - 5 байт <.em2.>
 * устройство - 2 байта. Смотри таблицу "номер - устройство"
 * версия - 4 байт <x.yy>
 * контрольная сумма - 4 байт
 * размер - 4 байт
 * данные
 * */

/* флэш:
 * 0-3 байт - байты флагов
 * 4-7 байты - версия прошивки
 * 8й байт - начало блока кода
 *
 * Если номер устройства не валиден - во флэш не пишем
 * данные расшифровываются прямо во флэш
 * Если после дешифрования контрольная сумма совпала - в ?первый? байт записывается флаг валидной прошивки
 * */


static volatile int _ErasingPageNumber;
static uint32_t _WriteFlashAddress;

char _WriteFlashStart(uint8_t* buf, uint8_t len)
{

	if (len < 20)
		return -1;

	_WriteFlashAddress = _MainProgramPageAddress;

	HAL_FLASH_Unlock();

	uint32_t t = HAL_GetTick();

	_ErasingPageNumber = _MainProgramPage;

	_WriteFlash(buf, len);

	t = HAL_GetTick() - t;
	t++;

	return 0;

}



void _WriteFlash(uint8_t* buf, uint8_t len)
{
	/*чистим по одной странице за раз*/
	if (_ErasingPageNumber < _FlashPageCount)
	{
		FLASH_PageErase(FLASH_BASE + _ErasingPageNumber * _FlashPageSize);
		/*The problem was that PER bit in FLASH->CR register which is set when FLASH_PageErase() is called isn't cleared at the end of it. Clearing this bit while flash is still unlocked allows other operations on flash to be run after that.
		STM documentation has nothing to say about this.*/
		CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
		_ErasingPageNumber++;
	}

	uint8_t writeIterations = len / 4;
	if (len % 4)
		writeIterations++;
	for (uint8_t i = 0; i < writeIterations; i++)
	{
		uint32_t data = 0;
		memcpy(&data, buf + i*4, 4);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _WriteFlashAddress, data);
		_WriteFlashAddress += 4;
	};

}

void _WriteFlashFinish()
{
	HAL_FLASH_Lock();
}

void _EraseFlash()
{
	HAL_FLASH_Unlock();

	for (uint16_t i = _MainProgramPage; i < _FlashPageCount; i++)
	{
		FLASH_PageErase(FLASH_BASE + i * _FlashPageSize);
	}

	/*The problem was that PER bit in FLASH->CR register which is set when FLASH_PageErase() is called isn't cleared at the end of it. Clearing this bit while flash is still unlocked allows other operations on flash to be run after that.
	STM documentation has nothing to say about this.*/
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);

	HAL_FLASH_Lock();
}

void LoadStackAndGo( void* sp, void* entry)
{
	typedef void (*pFunction)(void);
	pFunction Jump_To_Application;
	uint32_t jumpAddress = *(__IO uint32_t*) (_MainProgramPageAddress + 4);
	Jump_To_Application = (pFunction) jumpAddress;

	__set_MSP(*(__IO uint32_t*)_MainProgramPageAddress);
	Jump_To_Application();
}

void Jump2Prog()
{
	XmLedEn(0);

	__disable_irq();

	SCB->VTOR = _MainProgramPageAddress;

	LoadStackAndGo(*(__IO uint32_t*)_MainProgramPageAddress, *(__IO uint32_t*)(_MainProgramPageAddress + 4));

}
