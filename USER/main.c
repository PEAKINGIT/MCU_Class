#include "stm32f10x.h"

#include "delay.h"
#include "dht11.h"
#include "key.h"
#include "led.h"
#include "main_interface.h"
#include "menu.h"
#include "picture_app.h"
#include "gps_app.h"
#include "wifi_app.h"

#include "exfuns.h"      //extra Funcs for Fatfs
#include "ff.h"          //Fatfs
#include "lcd.h"         //LCD TFT Screen Drive
#include "lcd_defines.h"
#include "malloc.h"      //hand-made malloc
#include "piclib.h"      //lib for picture display
#include "rtc.h"         //realtime clk drive
#include "sdio_sdcard.h" //as its name told
#include "sys.h"
#include "text.h" //CN text display
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
    //u8 res; // ��ʱ����ֵ
    //u8 key;              // ��ֵ
    // u8 pause = 0;        // ��ͣ���
	u8 wifi_state;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�

    delay_init();         // ��ʱ������ʼ��
    uart_init(115200);    // ���ڳ�ʼ��Ϊ115200
    my_mem_init(SRAMIN);  // ��ʼ���ڲ��ڴ��
    led_init();           // LED�˿ڳ�ʼ��
    KEY_Init();           // ������ʼ��
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

	// Gps_Init();          // GPS Initialize
	wifi_state = WIFI_App_Init();
	if((wifi_state|0x01)!=0){
		printf("wifi_state:%02x\r\n",wifi_state);
	}

	current_page = MENU;
	//RTC_Force_Init(2024, 12, 11, 16, 41, 0);
	/**
	 * @brief ��ѭ��������!!!
	 */
    while (1) {

		//������ʾ �������ں����������ѭ��
		interface_functions[current_page]();

		// �ǽ���ѭ�������������������
		if(KEY_Scan(0)!=0){
			current_page = MAIN_INTERFACE;
		}
		LED1_Toggle;
		delay_ms(500);

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
