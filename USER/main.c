#include "stm32f10x.h"

#include "delay.h"
#include "key.h"
#include "lcd_display.h"
#include "led.h"
#include "main_interface.h"

#include "exfuns.h"	//extra Funcs for Fatfs
#include "ff.h"		//Fatfs 
#include "lcd.h"	//LCD TFT Screen Drive
#include "malloc.h"	//hand-made malloc
#include "piclib.h"	//lib for picture display
#include "rtc.h"	//realtime clk drive
#include "sdio_sdcard.h"	//as its name told
#include "sys.h"
#include "text.h"	//CN text display
#include "usart.h"
#include "w25qxx.h"	//Flash on board drive

// arm C libs
#include "math.h"
#include "string.h"

#include "./gps/usart2.h"
#include "./gps/gps.h"
#include "./gps/gps_app.h"
/**
 * @brief state for functions [called in main()] & [defined in main.c]
 */
void Delay(u32 count);

int main(void) {
	u8 res;				//��ʱ����ֵ
    u8 key;              // ��ֵ
    u8 pause = 0;        // ��ͣ���
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    delay_init();                                   // ��ʱ������ʼ��
    uart_init(115200);                              // ���ڳ�ʼ��Ϊ115200
    led_init();                                     // LED�˿ڳ�ʼ��
    KEY_Init();                                     // ������ʼ��
    LCD_Init();                                     // ��ʼ��LCD
    RTC_Init();                                     // RTC��ʼ��
	Gps_Init();
    // ���������洢���ĳ�ʼ��ΪʲôҪע�͵����·� --note (Ctrl+F to find)
    // SD_Init();
    // W25QXX_Init();				//��ʼ��W25Q128 �ⲿflash

    // �ֶ�ʵ��malloc�ĺ���
    my_mem_init(SRAMIN); // ��ʼ���ڲ��ڴ��

    exfuns_init(); // Ϊfatfs��ر��������ڴ�

    f_mount(fs[0], "0:", 1); // ����SD��
    f_mount(fs[1], "1:", 1); // ����FLASH.
    // --note
    // ���ص�ʱ��fatϵͳ��ͨ���û������diskio��ĺ����Դ洢�豸(SD,FLASH)���г�ʼ��
    // �������ֶ�����SD_Init()

    Font_Init_Check(); // ����ֿ��ʼ��
	
    Picture_Init_Check();	//���ͼƬ�ļ���ʼ��
	Draw_Picture_Init();

    while (1) {
		res = Draw_Picture(0);
		if(res != PIC_OK) break;
        draw_clock();
        while (1) {
            draw_mainInterface();
			delay_ms(100);
			LED1_Toggle;
        }
    }
	Picture_Free();
	
	draw_clock();
	while (1) {
		Gps_Receive_Handle();
		draw_mainInterface();
		delay_ms(100);
		LED0_Toggle;
    }
}

void Delay(u32 count) {
    u32 i = 0;
    for (; i < count; i++)
        ;
    for (; i < count; i++)
        ;
}

