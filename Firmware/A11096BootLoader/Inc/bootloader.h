#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include <stdint.h>

/*For attaching bootloader to new project implement next functions from xmodem.h:
 * XmLedEn
 * XmLedAlt
 * XmGetTickCount
 * XmReadCom
 * XmWriteCom
 * */

/* 03 - product type
 * 04 - product subtype*/
#define DevTypeNum 0x0304

/* Page size can be different for different chips
 * */
#define _FlashPageSize FLASH_PAGE_SIZE
#define _FlashPageCount 64
#define _MainProgramPage 16

void Jump2Prog();


typedef enum
{
	ffNone = 0,
	ffFlashed = 0x1,
	ffChecked = 0x2
} FlashFlags;

FlashFlags ReadFlags();

uint8_t Boot(void);

/*!!!!!!! Instruction !!!!!!!!!!!
 * 1. In original project fix VECT_TAB_OFFSET
 * 2. Add __enable_irq() after SystemClock_Config()
 * 3. Fix *_FLASH.ld for release configuration
 * */




#endif /*__BOOTLOADER_H__*/



