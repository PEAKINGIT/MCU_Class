#ifndef __MAIN_INTERFACE
#define __MAIN_INTERFACE
#include "delay.h"
#include "key.h"
#include "lcd.h"
#include "led.h"
#include "math.h"
#include "menu.h"
#include "picture_app.h"
#include "wifi_app.h"
#include "rtc.h"
#include "stm32f10x.h"
#include "string.h"
#include "lcd_defines.h"

// 屏幕中心坐标
#define CENTER_X 120
#define CENTER_Y 160
// 指针长度
#define HOUR_HAND_LENGTH 50
#define MINUTE_HAND_LENGTH 70

// 将角度转换为弧度
#define DEG_TO_RAD(angle) ((angle) * 3.1415 / 180.0)

void CalculatePointerCoordinates(uint16_t centerX, uint16_t centerY, float angle, uint16_t length, uint16_t *x, uint16_t *y);
void draw_clock(void);
void earse_clock(uint16_t hour, uint16_t minute);
void draw_mainInterface(void);

void Load_MainInterface(void);

#endif
