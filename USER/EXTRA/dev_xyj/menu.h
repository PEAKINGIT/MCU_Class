/*
 菜单页面
*/
#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include "lcd.h"

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

#define SELECT_NUM (MAX_INTERFACE+1)	//最大页面数 加其它选项

extern InterfaceState current_page; // 当前界面状态
extern void (*interface_functions[MAX_INTERFACE])(void); // 声明界面函数指针数组

void Menu_Init(void);
void Menu_Load(void);

void display_empty(void);

#endif
