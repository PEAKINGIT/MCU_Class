#ifndef __UART4_H
#define __UART4_H

#include "stm32f10x.h"
#include "sys.h"

#include "led.h"	//for debug

#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"

#define USART2_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART2_RX_EN 			1					//0,不接收;1,接收.

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern vu16 USART2_RX_STA;   						//接收数据状态

void USART2_init(u32 bound);				//串口2初始化 
void u2_printf(char* fmt,...);

void TIM5_Int_Init(u16 arr,u16 psc);	//TIM5及其定时中断初始化
#endif
