#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"

//���尴������
#define KEY0_PIN        GPIO_Pin_4
#define KEY1_PIN        GPIO_Pin_3
#define WKUP_PIN        GPIO_Pin_0
#define KEY0_PORT       GPIOE
#define KEY1_PORT       GPIOE
#define WKUP_PORT       GPIOA

//���尴���¼�ֵ
#define KEY0_PRES       1
#define KEY1_PRES       2
#define WKUP_PRES       3

//��������
void KEY_Init(void);          //������ʼ��
void EXTI3_IRQHandler(void);   // ����KEY1�жϷ�����
void EXTI4_IRQHandler(void);   // KEY0
void EXTI0_IRQHandler(void);   // WK_UP

#endif
