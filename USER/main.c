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
    // u8 res; // 临时返回值
    // u8 key;              // 键值
    //  u8 pause = 0;        // 暂停标记
    u8 wifi_state;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级分组为组2：2位抢占优先级，2位响应优先级

    delay_init();        // 延时函数初始化
    uart_init(115200);   // 串口初始化为115200
    my_mem_init(SRAMIN); // 初始化内部内存池
    led_init();          // LED端口初始化
    KEY_Init();          // 按键初始化
    tp_dev.init();
    BEEP_Init();
    LCD_Init();           // 初始化LCD
    RTC_Init();           // RTC初始化
    DHT11_Init_Wrapper(); // DHT11初始化

    // f_mount()挂载的时候fat系统会通过用户定义的diskio里的函数对存储设备(SD,FLASH)进行初始化
    // 不用再手动调用SD_Init(); W25QXX_Init();
    exfuns_init();           // 为fatfs相关变量申请内存
    f_mount(fs[0], "0:", 1); // 挂载SD卡
    f_mount(fs[1], "1:", 1); // 挂载FLASH.

    Font_Init_Check();    // 检查字库初始化
    Picture_Init_Check(); // 检查图片文件初始化
    PicDebug_ListPics();  // 串口打印出图片列表
    Draw_Picture_Init();  // 图片绘制初始化

    Gps_Init(); // GPS Initialize
    GPS_DebugIO_Init();
//    wifi_state = WIFI_App_Init(); // WIFI初始化有点slow 调试可以先关掉
//    wifi_state = 0x01;
//    if ((wifi_state | 0x01) != 0) {
//        printf("wifi_state:%02x\r\n", wifi_state);
//    }

    current_page = MAIN_INTERFACE;
    LED0(1);
    LED1(1);
//  RTC_Force_Init(2024, 12, 18, 21, 2, 0);	//强制初始化RTC
    /**
     * @brief 主循环在这里!!!
     */
    while (1) {
        // 界面显示 正常是在函数里面进行循环
        // interface_functions[current_page]();
        interface_functions[current_page]();

        // 非界面循环里任意键返回主界面
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
