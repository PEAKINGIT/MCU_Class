#ifndef __GPS_APP_H
#define __GPS_APP_H

#include "stm32f10x.h"
#include "delay.h"

#include "gps.h"
#include "lcd.h"

#include "math.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define GPS_InitOK (0x00)
#define GPS_TimeOut (0x01)
#define GPS_INIT_TIMEOUT (200) // timeout*10ms = realtime; 200->2s

#define DEBUG_IO PFout(0)	// BEEP,·äÃùÆ÷½Ó¿Ú

extern u8 USART1_TX_BUF[USART2_MAX_RECV_LEN];

u8 Gps_Init(void);
void Gps_Receive_Handle(void);
void Gps_Msg_Print(void);
void utc_to_local_time(nmea_utc_time *utc_time, int8_t timezone, nmea_utc_time *local_time);

#endif
