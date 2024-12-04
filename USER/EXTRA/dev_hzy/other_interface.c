#include "other_interface.h"
#include "key.h"
#include "lcd.h"
#include "delay.h"
#include "main_interface.h"

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
        } else if (key == KEY1_PRES) {
            current_state = SUB_INTERFACE_2; // 进入分界面2
        }
    } else if (current_state == SUB_INTERFACE_1) {
        // 分界面1按键逻辑
        if (key == KEY0_PRES) {
            current_state = MAIN_INTERFACE; // 返回主界面
        }
    } else if (current_state == SUB_INTERFACE_2) {
        // 分界面2按键逻辑
        if (key == KEY1_PRES) {
            current_state = MAIN_INTERFACE; // 返回主界面
        }
    }
}
// 主界面显示函数
void display_main_interface(void) {
    draw_mainInterface(); // 调用绘制主界面的函数
    delay_ms(100);
    LED1_Toggle;
}
// 分界面1显示函数
void display_sub_interface_1(void) {
    LCD_Clear(WHITE);
    // 在 (10, 10) 位置，显示宽度200，高度16，字体大小16的字符串 "This is Sub Interface 1"
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 1");
    delay_ms(100);  // 延时，模拟动态内容
}

// 分界面2显示函数
void display_sub_interface_2(void) {
    LCD_Clear(WHITE);
    // 在 (10, 10) 位置，显示宽度200，高度16，字体大小16的字符串 "This is Sub Interface 2"
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 2");
    delay_ms(100);  // 延时，模拟动态内容
}
// 界面模块初始化
void interface_init(void) {
    current_state = MAIN_INTERFACE; // 设置初始界面
    interface_functions[current_state](); // 显示初始界面
}