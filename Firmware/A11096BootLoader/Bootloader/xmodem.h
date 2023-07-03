/*

                           e Y8b    Y8b YV4.08P888 88e
                          d8b Y8b    Y8b Y888P 888 888D
                         d888b Y8b    Y8b Y8P  888 88"
                        d888WuHan8b    Y8b Y   888 b,
                       d8888888b Y8b    Y8P    888 88b,
           8888 8888       ,e,                                  888
           8888 888820088e  " Y8b Y888P ,e e, 888,8, dP"Y ,"Y88b888
           8888 8888888 88b888 Y8b Y8P d88 88b888 " C88b "8" 888888
           8888 8888888 888888  Y8b "  888   ,888    Y88D,ee 888888
           'Y88 88P'888 888888   Y8P    "YeeP"888   d,dP "88 888888
   888 88b,                    d8  888                     888
   888 88P' e88 88e  e88 88e  d88  888 e88 88e  ,"Y88b e88 888 ,e e, 888,8,
   888 8K  d888 888bd888 8Shaoziyang88d888 888b"8" 888d888 888d88 88b888 "
   888 88b,Y888 888PY888 888P 888  888Y888 888P,ee 888Y888 888888   ,888
   888 88P' "88 88"  "88 88"  888  888 "88 88" "88 888 "88 888 "YeeP"888


  Project:       AVR Universal BootLoader
  File:          bootldr.h
                 necessary macro declare
  Version:       4.5

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en

  Date:          2008.8

  See readme.htm to get more information.

*/

#ifndef _BOOTMODEM_H_
#define _BOOTMODEM_H_        1

///////////////////////////////////////////////////////////////
//Don't modify code below, unless your really konw what to do//
///////////////////////////////////////////////////////////////

//two buffer size must be multiple or submultiple relation
/*#if BUFFERSIZE >= SPM_PAGESIZE
#if (BUFFERSIZE / SPM_PAGESIZE * SPM_PAGESIZE) != BUFFERSIZE
#error "Result of (BUFFERSIZE / SPM_PAGESIZE) is not a Integer!"
#error "Please check and set 'BUFFERSIZE/SPM_PAGESIZE' Macro again!"
#endif
#else
#if (SPM_PAGESIZE /BUFFERSIZE * BUFFERSIZE) != SPM_PAGESIZE
#error "Result of (BUFFERSIZE / SPM_PAGESIZE) is not a Integer!"
#error "Please check and set 'BUFFERSIZE/SPM_PAGESIZE' Macro again!"
#endif
#endif*/

#include <stdint.h>

//internal use macro
#define CONCAT(a, b)       a ## b
#define CONCAT3(a, b, c)   a ## b ## c

#define TimeOutCnt         30

//max wait data time = TimeOutCntC * timeclk
//send 'C' command count
#define TimeOutCntC        50

//password length
#define CONNECTCNT         12

//Bootloader launch  0:comport password  1:port level
#define LEVELMODE          0

//toggle LED output
#define LEDAlt()           PORTREG(LEDPORT) ^= (1 << LEDPORTNo)

#define WDG_En 1

//timer1: prescale 1024, CTC mode 4, interval unit is millisecond
//Xmoden control command
#define XMODEM_NUL         0x00
#define XMODEM_SOH         0x01
#define XMODEM_STX         0x02
#define XMODEM_EOT         0x04
#define XMODEM_ACK         0x06
#define XMODEM_NAK         0x15
#define XMODEM_CAN         0x18
#define XMODEM_EOF         0x1A
#define XMODEM_RWC         'C'

#if RS485
#define RS485Enable()      PORTREG(RS485PORT) |= (1 << RS485TXEn); PORTREG(RS485PORTRx) |= (1 << RS485RXEn)
#define RS485Disable()     PORTREG(RS485PORT) &= ~(1 << RS485TXEn); PORTREG(RS485PORTRx) &= ~(1 << RS485RXEn)
#endif

char XmReadCom(unsigned char* b, unsigned short readComTimeout);
void XmWriteCom(unsigned char b);
void XmClearBuffers();

void wdt_reset();

typedef char (*OnReceivevePortion)(unsigned char* portionData, short portionDataLen);
char XmProccess(OnReceivevePortion rcvCallBack);

/*#define XmLedEn(val)
#define XmLedAlt()*/
void XmLedEn(char val);
void XmLedAlt(void);

/*1 tick == 1 ms*/
uint32_t XmGetTickCount();


#endif

//End of file: bootldr.h
