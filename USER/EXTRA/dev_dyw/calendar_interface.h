#ifndef __CALENDAR_IF_H_
#define __CALENDAR_IF_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "lcd.h"
#include "rtc.h"
#include "led.h"
#include "math.h"
#include "picture_app.h"
#include "key.h"
#include "menu.h"

#include <stdio.h>
#include <string.h>

// 定义一个结构体来存储节气名称和日期
typedef struct _solar_term {
    u8 name[10];
    u32 month;
    u32 day;
}SolarTerm;

extern u32 isBetween(u32 month, u32 day, u32 startMonth, u32 startDay, u32 endMonth, u32 endDay);// 判断一个日期是否在两个日期之间（包含前一个日期）
extern int calculate_days(int year); // 计算从 1900 年到输入年份之前所有年份的总天数
u32 getSolarTerm(u32 year, u32 month, u32 day);	// 定义一个函数来判断给定的日期属于哪个节气
void draw_calendarInterface(void);
void Load_calendarInterface(void);


#endif
