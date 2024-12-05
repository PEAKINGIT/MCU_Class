#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f10x.h"
	 
extern volatile u32 globalTick;
	 
void delay_init(void);
void delay_ms(u32 nms);
void delay_us(u32 nus);

void globalTick_Inc(void);
u32 globalTick_Get(void);

// ����Ƿ񵽴�ָ��ʱ����
static inline int tick_expired(uint32_t *last_tick, uint32_t interval) {
    uint32_t now = globalTick_Get();
    if ((now - *last_tick) >= interval) {
        *last_tick = now;
        return 1; // ʱ�䵽��
    }
    return 0; // ʱ��δ��
}
#endif

