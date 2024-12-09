#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"
#include "delay.h"

//定义按键引脚
#define KEY0_PIN        GPIO_Pin_4
#define KEY1_PIN        GPIO_Pin_3
#define WKUP_PIN        GPIO_Pin_0
#define KEY0_PORT       GPIOE
#define KEY1_PORT       GPIOE
#define WKUP_PORT       GPIOA

//定义按键事件值
#define KEY0_PRES       (0x01)
#define KEY1_PRES       (0x02)
#define WKUP_PRES       (0x03)
// long press
#define KEY0_LONGP		(0x0A)
#define KEY1_LONGP		(0x0B)
#define WKUP_LONGP		(0x0C)

#define WKUP_Msk (0x80)
#define WKUP_Long_Msk (0x40)
#define KEY0_Msk (0x20)
#define KEY0_Long_Msk (0x10)
#define KEY1_Msk (0x08)
#define KEY1_Long_Msk (0x04)
extern u8 key_state;
//key_state
//bit 7:6 WKUP:WKUP_Long
//bit 5:4 KEY0:KEY0_Long
//bit 3:2 KEY1:KEY1_Long

//函数声明
void KEY_Init(void);          //按键初始化
void EXTI3_IRQHandler(void);   // 按键KEY1中断服务函数
void EXTI4_IRQHandler(void);   // KEY0
void EXTI0_IRQHandler(void);   // WK_UP

u8 KEY_Scan(u8 mode);
void globalKeyEvent(void);

#endif
