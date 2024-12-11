#ifndef __CALENDAR_IF_H_
#define __CALENDAR_IF_H

#include "gui_g9.h"
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
typedef struct {
    u8 name[10];
    int month;
    int day;
} SolarTerm;

// 定义节气数据，并按日期顺序排序
const SolarTerm solarTerms[] = {
    {"小寒", 1, 5}, {"大寒", 1, 20}, {"立春", 2, 4}, {"雨水", 2, 19},
    {"惊蛰", 3, 6}, {"春分", 3, 21}, {"清明", 4, 5}, {"谷雨", 4, 20},
    {"立夏", 5, 6}, {"小满", 5, 21}, {"芒种", 6, 6}, {"夏至", 6, 21},
    {"小暑", 7, 7}, {"大暑", 7, 23}, {"立秋", 8, 8}, {"处暑", 8, 23},
    {"白露", 9, 8}, {"秋分", 9, 23}, {"寒露", 10, 8}, {"霜降", 10, 23},
    {"立冬", 11, 7}, {"小雪", 11, 22}, {"大雪", 12, 6}, {"冬至", 12, 21}
};

int isBetween(int month, int day, int startMonth, int startDay, int endMonth, int endDay);// 判断一个日期是否在两个日期之间（包含前一个日期）
int getSolarTerm(int year, int month, int day);	// 定义一个函数来判断给定的日期属于哪个节气
void draw_calendarInterface(void);
void Load_calendarInterface(void);


#endif
