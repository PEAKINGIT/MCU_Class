#ifndef __WIFI_APP_H
#define __WIFI_APP_H

#include "stm32f10x.h"
#include "common.h"
#include "usart3.h"
#include "string.h"
#include "lcd_defines.h"
#include "menu.h"

//WIFI状态位量
// u8 WIFI_state
// b0 OK/ERR
// b1 module on/module off 模块是否连接
// b7 get connect/no connect

#define WIFI_OK (0x00)
#define WIFI_ERR (0x01)

#define WIFIDEV_ON (0x00)
#define WIFIDEV_OFF (0x02)

//服务器连接状态
#define NO_CNNCT (0x80)	//未建立服务器连接
#define GET_CNNCT (0x00)	// 1000 0000

#define WIFI_MODE_AP (0x01)
#define WIFI_MODE_STA (0x02)
#define WIFI_MODE_APSTA (0x03)

#define STA_TCP_SERVER (0x00)
#define STA_TCP_CLIENT (0x01)
#define STA_UDP (0x02)

extern u8 *default_ip;

u8 WIFI_App_Init(void);
u8 WIFI_STA_Set(void);
void WIFI_RcvHandle(u8 wifi_isok, void (*cmdExec)(u8 *));

void WIFI_StartSendtoSever(void);
uint8_t WIFI_ConnectCheck(void);
void WIFI_StateDisp(void);

#endif
