#include "other_interface.h"
#include "key.h"
#include "lcd.h"
#include "delay.h"
#include "main_interface.h"
#include "dht11.h"

// 当前界面状态
extern InterfaceState current_state;
// 当前界面状态
InterfaceState current_state = MAIN_INTERFACE;

// 界面函数指针数组
void (*interface_functions[MAX_INTERFACE])(void) = {
    display_main_interface,
    display_sub_interface_1,
    display_sub_interface_2
};

// 按键事件处理函数
void handle_key_event(void) {
    u8 key = KEY_Scan(0); // 扫描按键状态

    if (current_state == MAIN_INTERFACE) {
        // 主界面按键逻辑
        if (key == KEY0_PRES) {
            current_state = SUB_INTERFACE_1; // 进入分界面1
					  display_sub_interface_1_init();  // 调用分界面1的初始化函数
        } else if (key == KEY1_PRES) {
            current_state = SUB_INTERFACE_2; // 进入分界面2
					 display_sub_interface_2_init();  // 调用分界面2的初始化函数
        }
    } else if (current_state == SUB_INTERFACE_1) {
        // 分界面1按键逻辑
        if (key == KEY0_PRES) {
            current_state = MAIN_INTERFACE; // 返回主界面
					//interface_init();//主界面初始化
        }
    } else if (current_state == SUB_INTERFACE_2) {
        // 分界面2按键逻辑
        if (key == KEY1_PRES) {
            current_state = MAIN_INTERFACE; // 返回主界面
					//interface_init();//主界面初始化
        }
    }
}
// 主界面显示函数
void display_main_interface(void) {
    draw_mainInterface(); // 调用绘制主界面的函数
    delay_ms(100);
    LED1_Toggle;
}
// 分界面1初始化函数
void display_sub_interface_1_init(void) {
    LCD_Clear(WHITE);  // 清屏操作，初始化界面背景
    // 其他初始化操作，如绘制固定的静态内容
    LCD_ShowString(10, 10, 200, 16, 16, "Temperature and Humidity");  // 显示标题
}
// 分界面1显示函数（温湿度显示）
void display_sub_interface_1(void) {
   DHT11_Update_Data_If_Expired(); // 周期性更新 DHT11 数据

    // 打印温湿度数据，每秒一次
    static uint32_t last_print_tick = 0;
    if (tick_expired(&last_print_tick, 1000)) {
        // 获取温湿度数据
        u8 temperature = DHT11_Get_Temperature();
        u8 humidity = DHT11_Get_Humidity();

        // 将温湿度数据转换为字符串
        char temp_str[32], humi_str[32];
        sprintf(temp_str, "Temp: %d C", temperature); // 温度字符串
        sprintf(humi_str, "Humi: %d %%", humidity); // 湿度字符串

        // 显示温度和湿度数据
			  
        LCD_ShowString(10, 50, 200, 16, 16, temp_str); // 显示温度
        LCD_ShowString(10, 70, 200, 16, 16, humi_str); // 显示湿度
    }

    delay_ms(100);  // 延时，模拟动态内容
}
// 分界面2初始化函数
void display_sub_interface_2_init(void) {
    LCD_Clear(WHITE);  // 清屏操作，初始化界面背景
    // 其他初始化操作，如绘制固定的静态内容
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 2");  // 显示标题
}

// 分界面2显示函数
void display_sub_interface_2(void) {
    
    delay_ms(100);  // 延时，模拟动态内容
}
// 界面模块初始化
void interface_init(void) {
    current_state = MAIN_INTERFACE; // 设置初始界面
    interface_functions[current_state](); // 显示初始界面
}