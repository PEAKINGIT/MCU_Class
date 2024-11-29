#include "stm32f10x.h"

#include "sys.h"
#include "delay.h"

#include "led.h"
#include "usart.h"

#include "./gps/usart2.h"
#include "./gps/gps.h"

// HZY
#include "key.h" //2024/11/10 hzy
//

void Delay(u32 count);

extern volatile uint32_t time_counter;
//for GPS Test
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; 					//串口1,发送缓存区

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    delay_init();

    // HZY
    KEY_Init(); // 2024/11/10 hzy
                //

    // GPS Test
    u16 i,rxlen;
	u16 lenx;
	u8 key=0XFF;
	u8 upload=1;
    if (SkyTra_Cfg_Rate(5) != 0) // 设置定位信息更新速度为5Hz,顺便判断GPS模块是否在位.
    {
        printf("SkyTraF8-BD Setting...\r\n");
        do {
            USART2_init(9600);           // 初始化串口3波特率为9600
            SkyTra_Cfg_Prt(3);           // 重新设置模块的波特率为38400
            USART2_init(38400);          // 初始化串口3波特率为38400
            key = SkyTra_Cfg_Tp(100000); // 脉冲宽度为100ms
        } while (SkyTra_Cfg_Rate(5) != 0 && key != 0); // 配置SkyTraF8-BD的更新速率为5Hz
        printf("SkyTraF8-BD Set Done!!\r\n");
        delay_ms(500);
    }
    while (1) {
        delay_ms(1);
		printf("TickNow:%u\r\n",time_counter);
        if (USART2_RX_STA & 0X8000) // 接收到一次数据了
        {
            rxlen = USART2_RX_STA & 0X7FFF; // 得到数据长度
            for (i = 0; i < rxlen; i++)
                USART1_TX_BUF[i] = USART2_RX_BUF[i];
            USART2_RX_STA = 0;                               // 启动下一次接收
            USART1_TX_BUF[i] = 0;                            // 自动添加结束符
            GPS_Analysis(&gpsData, (u8 *)USART1_TX_BUF);        // 分析字符串
            Gps_Msg_Print();                                  // 显示信息
            if (upload) printf("\r\n%s\r\n", USART1_TX_BUF); // 发送接收到的数据到串口1
        }
        if ((lenx % 500) == 0)
            LED0_Toggle;
        lenx++;
    }
}

void Delay(u32 count) {
    u32 i = 0;
    for (; i < count; i++)
        ;
}
