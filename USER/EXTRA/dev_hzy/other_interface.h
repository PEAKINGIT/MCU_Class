#ifndef __INTERFACE_H
#define __INTERFACE_H

// 界面状态枚举
typedef enum {
    MAIN_INTERFACE,  // 主界面
    SUB_INTERFACE_1, // 分界面1
    SUB_INTERFACE_2, // 分界面2
    MAX_INTERFACE    // 界面数量
} InterfaceState;

// 界面管理相关变量和接口
extern InterfaceState current_state; // 当前界面状态
void interface_init(void);           // 界面模块初始化

// 声明界面函数指针数组
extern void (*interface_functions[MAX_INTERFACE])(void); // 使用 extern 声明

void display_main_interface(void);
void display_sub_interface_1(void);
void display_sub_interface_2(void);
void handle_key_event(void); // 按键事件处理函数
void display_sub_interface_1_init(void); //分界面1初始化函数
void display_sub_interface_2_init(void); //分界面2初始化函数
#endif // __OTHER_INTERFACE_H
