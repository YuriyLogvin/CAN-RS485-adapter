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
  File:          bootldr.c
                 main program code
  Version:       4.5

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en

  Date:          2008.8

  See readme.htm to get more information.

*/

#include "../Bootloader/xmodem.h"

//user's application start address
#define PROG_START         0x0000

#define BUFFERSIZE 128

//remove IVT
#define noIVT              0

#define ReadComTimeout 100

//password
#if LEVELMODE == 0
unsigned char ConnectKey[] = { 0x71, 0x23, 0x54, 0x01, 0x90, 0x71, 0x47, 0x58, 0x94, 0x12, 0x71, 0xA0 };
#endif

//define receive buffer
unsigned char buf[BUFFERSIZE];

#if (BUFSIZE > 255)
unsigned int bufptr, pagptr;
#else
unsigned char bufptr, pagptr;
#endif

unsigned char ch, cl;

//calculate CRC checksum
void crc16(unsigned char *buf)
{
#if (BUFSIZE > 255)
  unsigned int j;
#else
  unsigned char j;
#endif

#if (CRCMODE == 0)
  unsigned char i;
  unsigned int t;
#endif
  unsigned int crc;

  crc = 0;
  for(j = BUFFERSIZE; j > 0; j--)
  {
#if (CRCMODE == 0)
    //CRC1021 checksum
    crc = (crc ^ (((unsigned int) *buf) << 8));
    for(i = 8; i > 0; i--)
    {
      t = crc << 1;
      if(crc & 0x8000)
        t = t ^ 0x1021;
      crc = t;
    }
#elif (CRCMODE == 1)
    //word add up checksum
    crc += (unsigned int)(*buf);
#else
#error "Unknow CRC MODE!"
#endif
    buf++;
  }
  ch = crc / 256;
  cl = crc % 256;
}

char XmProccess(OnReceivevePortion rcvCallBack)
{
  unsigned char cnt;
  unsigned char packNO;
  unsigned char crch, crcl;
  unsigned char bi;
  unsigned char b;
  uint32_t tickCount = XmGetTickCount();

#if (BUFFERSIZE > 255)
  unsigned int li;
#else
  unsigned char li;
#endif


  XmClearBuffers();

  //set LED control port to output
  XmLedEn(1);

  cnt = TimeOutCnt;
  cl = 0;
  while(XmGetTickCount() < tickCount + 1000)
  {
#if WDG_En
    //clear watchdog
    wdt_reset();
#endif

	XmLedAlt();

    if(XmReadCom(&b, ReadComTimeout))             //receive connect key
    {
      if(b == ConnectKey[cl])  //compare ConnectKey
        cl++;
      else
        cl = 0;
    }
    if (cl >= sizeof(ConnectKey))
    	break;
  }

  if (cl < sizeof(ConnectKey))
	  return 1;

  tickCount = XmGetTickCount();

  //every interval send a "C",waiting XMODEM control command <soh>
  cnt = TimeOutCntC;
  while(1)
  {
	while(XmGetTickCount() < tickCount + 4);
	tickCount = XmGetTickCount();

    {

		XmWriteCom(XMODEM_RWC) ;    //send "C"

      XmLedAlt();                 //toggle LED

      cnt--;
      if(cnt == 0)              //timeout
      {
        return 2;
      }

#if WDG_En
    wdt_reset();                //clear watchdog
#endif

    }

#if WDG_En
    wdt_reset();                //clear watchdog
#endif

    if(XmReadCom(&b, ReadComTimeout))
    {
      if(b == XMODEM_SOH)  //XMODEM command <soh>
        break;
    }
  }

  //begin to receive data
  packNO = 0;
  bufptr = 0;
  cnt = 0;
  do
  {
    packNO++;
    XmReadCom(&ch, ReadComTimeout);                          //get package number
    XmReadCom(&cl, ReadComTimeout); cl = ~cl;
    if ((packNO == ch) && (packNO == cl))
    {
      for(li = BUFFERSIZE; li > 0; li--)      //receive a full data frame
      {
    	  XmReadCom(buf+bufptr, ReadComTimeout);
    	  bufptr++;
      }
      XmReadCom(&crch, ReadComTimeout);                       //get checksum
      XmReadCom(&crcl, ReadComTimeout);
      crc16(&buf[bufptr - BUFFERSIZE]);       //calculate checksum
      if((crch == ch) && (crcl == cl))
      {
          if(bufptr >= BUFFERSIZE)          //Flash page full, write flash page;otherwise receive next frame
          {                                   //receive multi frames, write one page
        	if (0 != rcvCallBack(buf, BUFFERSIZE))              //write data to Flash
        	{
        		XmWriteCom(XMODEM_CAN);
        		return 3;
        	}
            bufptr = 0;
          }


          XmWriteCom(XMODEM_ACK);                 //send ACK directly
          cnt = 0;

	#if WDG_En
			wdt_reset();                          //clear watchdog
	#endif

		XmLedAlt();                             //LED indicate update status
      }
      else //CRC
      {
    	  XmWriteCom(XMODEM_NAK);                 //require resend
    	  cnt++;
      }
    }
    else //PackNo
    {
    	XmWriteCom(XMODEM_NAK);                   //require resend
    	cnt++;
    }

    if(cnt > 3)                               //too many error, abort update
      break;
  }
  while(XmReadCom(&b, ReadComTimeout) && b != XMODEM_EOT);

  XmWriteCom(XMODEM_ACK);

  if(cnt > 0)
  {
#if WDG_En
    while(1);                                 //when update fail, use dead loop wait watchdog reset
#else
#if (BootStart > 0)
    (*((void(*)(void))(BootStart)))();
#endif
#endif
  }

  return 0;
}

//End of file: bootldr.c
