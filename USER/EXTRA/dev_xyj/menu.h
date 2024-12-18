/*
 菜单页面
*/
#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include "lcd.h"
#include "lcd_defines.h"

#include "main_interface.h"
#include "gps_interface.h"
#include "dht11_interface.h"
#include "calendar_interface.h"
#include "rtc.h"
#include "wifi_app.h"
#include "beep.h"
#include "key.h"

//interfaces
// 界面状态枚举
typedef enum {
	EMPTY_INTREFACE = 0,	//空界面
    MAIN_INTERFACE,  // 主界面
    DHT_INTREFACE, // 分界面1
    GPS_GUI, // 分界面2
	CALENDAR,
	MENU,
    MAX_INTERFACE,    // 界面数量
} InterfaceState;

typedef struct _select_opt{
	uint8_t *text;
	uint8_t type;	// 选项类型 功能型还是页面切换
	void (*select_event)(void);
}Select;

#define EMPTY	(0x00)	//默认 无功能选项
#define INTERF_S (0x01)	//页面切换
#define FUNC (0x02)		//菜单界面小功能

#define PAGE_INUM	(6)	//页面项目数
#define PAGE_NUM	(2)	//page数
#define SELECT_NUM (12)	// 
#define M_PAGE1 (1)
#define M_PAGE2 (2)
#define M_PAGE3 (3)

#define SELECT_H (22)

extern InterfaceState current_page; // 当前界面状态
extern void (*interface_functions[MAX_INTERFACE])(void); // 声明界面函数指针数组

void Menu_Init(void);
void Menu_Load(void);
void add_Select(uint16_t index, uint8_t *text, void (*select_event)(void),uint8_t type) ;
void disp_selects(u16 index1, u16 index2) ;
void disp_MenuPage(uint16_t page) ;


void display_empty(void);

// 选项函数
void s2Main(void);
void s2DHT(void);
void s2GPS(void);
void s2Calendar(void);
void rstWIFI(void);
void adjTime(void);
void FuncClear(void);
void CallingHelp(void);
void setBackCount(void);

#endif
