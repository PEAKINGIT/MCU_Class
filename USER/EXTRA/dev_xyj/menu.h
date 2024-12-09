/*
 菜单页面
*/
#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include "lcd.h"
#include "gui_g9.h"

#include "main_interface.h"
#include "gps_interface.h"
#include "dht11_interface.h"

//interfaces
// 界面状态枚举
typedef enum {
	EMPTY_INTREFACE = 0,	//空界面
    MAIN_INTERFACE,  // 主界面
    SUB_INTERFACE_1, // 分界面1
    SUB_INTERFACE_2, // 分界面2
	MENU,
    MAX_INTERFACE,    // 界面数量
} InterfaceState;

typedef struct _select_opt{
	uint8_t *text;
	void (*select_event)(void);
}Select;

typedef struct _menu_data{
	uint8_t *title;		//页面标题
	Select *selects;	//选项
}MenuData;

extern InterfaceState current_page; // 当前界面状态
extern InterFaceObj Menu_Obj;

void interface_init(void);           // 界面模块初始化
// 声明界面函数指针数组
extern void (*interface_functions[MAX_INTERFACE])(void); // 使用 extern 声明

void display_sub_interface_2(void);
void handle_key_event(void); // 按键事件处理函数
void display_sub_interface_2_init(void); //分界面2初始化函数
void display_empty(void);

#endif
