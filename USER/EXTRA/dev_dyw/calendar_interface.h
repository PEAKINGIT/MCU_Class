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

// ����һ���ṹ�����洢�������ƺ�����
typedef struct {
    u8 name[10];
    int month;
    int day;
} SolarTerm;

// ����������ݣ���������˳������
const SolarTerm solarTerms[] = {
    {"С��", 1, 5}, {"��", 1, 20}, {"����", 2, 4}, {"��ˮ", 2, 19},
    {"����", 3, 6}, {"����", 3, 21}, {"����", 4, 5}, {"����", 4, 20},
    {"����", 5, 6}, {"С��", 5, 21}, {"â��", 6, 6}, {"����", 6, 21},
    {"С��", 7, 7}, {"����", 7, 23}, {"����", 8, 8}, {"����", 8, 23},
    {"��¶", 9, 8}, {"���", 9, 23}, {"��¶", 10, 8}, {"˪��", 10, 23},
    {"����", 11, 7}, {"Сѩ", 11, 22}, {"��ѩ", 12, 6}, {"����", 12, 21}
};

int isBetween(int month, int day, int startMonth, int startDay, int endMonth, int endDay);// �ж�һ�������Ƿ�����������֮�䣨����ǰһ�����ڣ�
int getSolarTerm(int year, int month, int day);	// ����һ���������жϸ��������������ĸ�����
void draw_calendarInterface(void);
void Load_calendarInterface(void);


#endif
