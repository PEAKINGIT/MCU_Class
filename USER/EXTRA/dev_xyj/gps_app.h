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

void Gps_Init(void);
void Gps_Receive_Handle(void);
void Gps_Msg_Print(void);

#endif
