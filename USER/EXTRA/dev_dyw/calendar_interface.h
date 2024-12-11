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

// ����һ���ṹ�����洢�������ƺ�����
typedef struct _solar_term {
    u8 name[10];
    u32 month;
    u32 day;
}SolarTerm;

u32 isBetween(u32 month, u32 day, u32 startMonth, u32 startDay, u32 endMonth, u32 endDay);// �ж�һ�������Ƿ�����������֮�䣨����ǰһ�����ڣ�
u32 getSolarTerm(u32 year, u32 month, u32 day);	// ����һ���������жϸ��������������ĸ�����
void draw_calendarInterface(void);
void Load_calendarInterface(void);


#endif
