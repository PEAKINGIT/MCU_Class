#ifndef __LED_H__
#define __LED_H__
#include "stm32f10x.h"

#define LED0(__SET__)  GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)(__SET__))
#define LED1(__SET__)	GPIO_WriteBit(GPIOE, GPIO_Pin_5, (BitAction)(__SET__))

#define LED0_Toggle GPIO_WriteBit(GPIOB, GPIO_Pin_5,(BitAction)!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)))
#define LED1_Toggle GPIO_WriteBit(GPIOE, GPIO_Pin_5,(BitAction)!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)))

#endif
