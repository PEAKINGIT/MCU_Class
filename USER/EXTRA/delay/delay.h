#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f10x.h"
	 
extern volatile u32 globalTick;
	 
void delay_init(void);
void delay_ms(u32 nms);
void delay_us(u32 nus);

void globalTick_Inc(void);
u32 globalTick_Get(void);

#endif





























