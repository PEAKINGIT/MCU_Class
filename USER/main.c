#include "stm32f10x.h"

#include "beep.h"
#include "delay.h"
#include "dht11.h"
#include "gps_app.h"
#include "key.h"
#include "led.h"
#include "main_interface.h"
#include "menu.h"
#include "picture_app.h"
#include "wifi_app.h"

#include "exfuns.h" //extra Funcs for Fatfs
#include "ff.h"     //Fatfs
#include "lcd.h"    //LCD TFT Screen Drive
#include "lcd_defines.h"
#include "malloc.h"      //hand-made malloc
#include "piclib.h"      //lib for picture display
#include "rtc.h"         //realtime clk drive
#include "sdio_sdcard.h" //as its name told
#include "sys.h"
#include "text.h" //CN text display
#include "touch.h"
#include "usart.h"
#include "w25qxx.h" //Flash on board drive

// arm C libs
#include "math.h"
#include "stdio.h"
#include "string.h"

/**
 * @brief state for functions [called in main()] & [defined in main.c]
 */
void Delay(u32 count);

/**
 * @brief main Function
 */
int main(void) {
    // u8 res; // ��ʱ����ֵ
    // u8 key;              // ��ֵ
    //  u8 pause = 0;        // ��ͣ���
    u8 wifi_state;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�

    delay_init();        // ��ʱ������ʼ��
    uart_init(115200);   // ���ڳ�ʼ��Ϊ115200
    my_mem_init(SRAMIN); // ��ʼ���ڲ��ڴ��
    led_init();          // LED�˿ڳ�ʼ��
    KEY_Init();          // ������ʼ��
    tp_dev.init();
    BEEP_Init();
    LCD_Init();           // ��ʼ��LCD
    RTC_Init();           // RTC��ʼ��
    DHT11_Init_Wrapper(); // DHT11��ʼ��

    // f_mount()���ص�ʱ��fatϵͳ��ͨ���û������diskio��ĺ����Դ洢�豸(SD,FLASH)���г�ʼ��
    // �������ֶ�����SD_Init(); W25QXX_Init();
    exfuns_init();           // Ϊfatfs��ر��������ڴ�
    f_mount(fs[0], "0:", 1); // ����SD��
    f_mount(fs[1], "1:", 1); // ����FLASH.

    Font_Init_Check();    // ����ֿ��ʼ��
    Picture_Init_Check(); // ���ͼƬ�ļ���ʼ��
    PicDebug_ListPics();  // ���ڴ�ӡ��ͼƬ�б�
    Draw_Picture_Init();  // ͼƬ���Ƴ�ʼ��

    Gps_Init(); // GPS Initialize
    GPS_DebugIO_Init();
   wifi_state = WIFI_App_Init(); // WIFI��ʼ���е�slow ���Կ����ȹص�
   wifi_state = 0x01;
   if ((wifi_state | 0x01) != 0) {
       printf("wifi_state:%02x\r\n", wifi_state);
   }

    current_page = MAIN_INTERFACE;
    LED0(1);
    LED1(1);
	//RTC_Force_Init(2024, 12, 18, 21, 2, 0);	//ǿ�Ƴ�ʼ��RTC
    /**
     * @brief ��ѭ��������!!!
     */
    while (1) {
        // ������ʾ �������ں����������ѭ��
        interface_functions[current_page]();

		
        // �ǽ���ѭ�������������������
        if (current_page == EMPTY_INTREFACE) {
            delay_ms(800);
            tp_dev.scan(0);
            if (KEY_Scan(0) != 0 || (tp_dev.sta & TP_PRES_DOWN)) {
                current_page = MAIN_INTERFACE;
            }
            LED0_Toggle;
            LED1_Toggle;
            delay_ms(200);
        }
		if(USART_RX_STA & 0x8000){
			printf("recv:%s\r\n",USART_RX_BUF);
			if((u8 *)strstr((char *)USART_RX_BUF,"RTC")!= NULL){
				RTC_Force_Init(2024, 12, 23, 21, 56, 0);
			}
			USART_RX_STA = 0;
		}
    }
}

/**
 * @brief Function Defines
 */
void Delay(u32 count) {
    u32 i = 0;
    for (; i < count; i++)
        ;
}

/* END OF FILE */
