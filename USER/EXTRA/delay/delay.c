#include "delay.h"

volatile u32 globalTick = 0;
static u8 fac_us = 0; // us延时倍乘数

void delay_init() {
    SysTick_Config(SystemCoreClock / 1000); // 1ms触发一次
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); // 选择外部时钟
    fac_us = SystemCoreClock / 1000000; // 为系统时钟的
}
void delay_us(u32 nus) {
//    u32 ticks;
//    u32 told, tnow, tcnt = 0;
//    u32 reload = SysTick->LOAD; // LOAD的值
//    ticks = nus * fac_us; // 需要的节拍数
//    told = SysTick->VAL; // 刚进入时的计数器值
//    while (1) {
//        tnow = SysTick->VAL;
//        if (tnow != told) {
//            if (tnow < told)
//                tcnt += told - tnow; // 这里注意一下SYSTICK是一个递减的计数器就可以了.
//            else
//                tcnt += reload - tnow + told;
//            told = tnow;
//            if (tcnt >= ticks) break; // 时间超过/等于要延迟的时间,则退出.
//        }
//    };
	for(u32 i =0;i<nus*72;i++){
		
	}
}

// 延时nms
// nms:要延时的ms数
void delay_ms(u32 nms) {
    u32 i;
    for (i = 0; i < nms; i++)
        delay_us(1000);
}

void globalTick_Inc(void){
	globalTick++;
}
u32 globalTick_Get(void){
	return globalTick;
}
