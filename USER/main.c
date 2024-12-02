#include "stm32f10x.h"

#include "sys.h"
#include "delay.h"

#include "led.h"
#include "usart.h"

#include "./gps/usart2.h"
#include "./gps/gps.h"
#include "./gps/gps_app.h"

// HZY
#include "key.h" //2024/11/10 hzy
//

void Delay(u32 count);

extern volatile uint32_t time_counter;

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    delay_init();
    KEY_Init();
	Gps_Init();
    
    while (1) {
        delay_ms(1);
		Gps_Receive_Handle();
    }
}

void Delay(u32 count) {
    u32 i = 0;
    for (; i < count; i++)
        ;
}
