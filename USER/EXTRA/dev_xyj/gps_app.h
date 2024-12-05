#ifndef __GPS_APP_H
#define __GPS_APP_H

#include "delay.h" 	

#include "gps.h"
#include "gps.h" 
#include "lcd.h"

#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h"

#define GPS_InitOK (0x00)
#define GPS_TimeOut (0x01)

#define GPS_INIT_TIMEOUT (200)	//timeout*10ms = realtime; 200->2s

u8 Gps_Init(void);
void Gps_Receive_Handle(void);
void Gps_Msg_Print(void);

#endif
