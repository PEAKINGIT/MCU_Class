#ifndef __UART4_H
#define __UART4_H

#include "stm32f10x.h"
#include "sys.h"

#include "led.h"	//for debug

#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"

#define USART2_MAX_RECV_LEN		600					//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		600					//����ͻ����ֽ���
#define USART2_RX_EN 			1					//0,������;1,����.

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern vu16 USART2_RX_STA;   						//��������״̬

void USART2_init(u32 bound);				//����2��ʼ�� 
void u2_printf(char* fmt,...);

void TIM5_Int_Init(u16 arr,u16 psc);	//TIM5���䶨ʱ�жϳ�ʼ��
#endif
