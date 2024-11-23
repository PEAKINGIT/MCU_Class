#include "stm32f10x.h"
#include "usart.h"
#include "led.h"

void Delay(u32 count);

// HZY
#include "key.h" //2024/11/10 hzy
//

int main(void) {

    uart_init(115200);
    SysTick_Config(SystemCoreClock / 1000); // 初始化SysTick每1ms触发一次

    // HZY
    KEY_Init(); // 2024/11/10 hzy
    //

    while (1) {
    }
}

void Delay(u32 count) {
    u32 i = 0;
    for (; i < count; i++);
}
