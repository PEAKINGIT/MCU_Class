#ifndef __WIFI_APP_H
#define __WIFI_APP_H

#include "stm32f10x.h"
#include "common.h"
#include "usart3.h"

#define WIFI_MODE_AP (0x01)
#define WIFI_MODE_STA (0x02)
#define WIFI_MODE_APSTA (0x03)

#define STA_TCP_SERVER (0x00)
#define STA_TCP_CLIENT (0x01)
#define STA_UDP (0x02)

extern u8 test_wifi;

void WIFI_App_Init(u8 mode);
void WIFI_STA_Set(void);
u16 WIFI_RcvHandle(u8 con_state,u8 *p);

#endif
