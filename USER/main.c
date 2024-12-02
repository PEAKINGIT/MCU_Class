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
	u8 res;				//临时返回值
    u8 key;              // 键值
    u8 pause = 0;        // 暂停标记
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    delay_init();                                   // 延时函数初始化
    uart_init(115200);                              // 串口初始化为115200
    led_init();                                     // LED端口初始化
    KEY_Init();                                     // 按键初始化
    LCD_Init();                                     // 初始化LCD
    RTC_Init();                                     // RTC初始化
	Gps_Init();										//GPS Initialize

    // 下面两个存储器的初始化为什么要注释掉见下方 --note (Ctrl+F to find)
    // SD_Init();
    // W25QXX_Init();				//初始化W25Q128 外部flash
    // 手动实现malloc函数的初始化
    my_mem_init(SRAMIN); // 初始化内部内存池
    exfuns_init(); // 为fatfs相关变量申请内存
    f_mount(fs[0], "0:", 1); // 挂载SD卡
    f_mount(fs[1], "1:", 1); // 挂载FLASH.
    // --note
    // f_mount()挂载的时候fat系统会通过用户定义的diskio里的函数对存储设备(SD,FLASH)进行初始化
    // 不用再手动调用SD_Init()

    Font_Init_Check(); // 检查字库初始化
    Picture_Init_Check();	//检查图片文件初始化
	Draw_Picture_Init();	//图片绘制初始化 可以改进成显示指定图片

    while (1) {
		res = Draw_Picture(0);
		if(res != PIC_OK) break;
        draw_clock();
        while (1) {
			Gps_Receive_Handle();
            draw_mainInterface();
			delay_ms(100);
			LED1_Toggle;
        }
    }
	Picture_Free();	//释放绘图有关的申请的内存
	
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

