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
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; 					//����1,���ͻ�����

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
    if (SkyTra_Cfg_Rate(5) != 0) // ���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ.
    {
        printf("SkyTraF8-BD Setting...\r\n");
        do {
            USART2_init(9600);           // ��ʼ������3������Ϊ9600
            SkyTra_Cfg_Prt(3);           // ��������ģ��Ĳ�����Ϊ38400
            USART2_init(38400);          // ��ʼ������3������Ϊ38400
            key = SkyTra_Cfg_Tp(100000); // ������Ϊ100ms
        } while (SkyTra_Cfg_Rate(5) != 0 && key != 0); // ����SkyTraF8-BD�ĸ�������Ϊ5Hz
        printf("SkyTraF8-BD Set Done!!\r\n");
        delay_ms(500);
    }
    while (1) {
        delay_ms(1);
		printf("TickNow:%u\r\n",time_counter);
        if (USART2_RX_STA & 0X8000) // ���յ�һ��������
        {
            rxlen = USART2_RX_STA & 0X7FFF; // �õ����ݳ���
            for (i = 0; i < rxlen; i++)
                USART1_TX_BUF[i] = USART2_RX_BUF[i];
            USART2_RX_STA = 0;                               // ������һ�ν���
            USART1_TX_BUF[i] = 0;                            // �Զ���ӽ�����
            GPS_Analysis(&gpsData, (u8 *)USART1_TX_BUF);        // �����ַ���
            Gps_Msg_Print();                                  // ��ʾ��Ϣ
            if (upload) printf("\r\n%s\r\n", USART1_TX_BUF); // ���ͽ��յ������ݵ�����1
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
