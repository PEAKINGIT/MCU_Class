#ifndef __MAIN_INTERFACE
#define	__MAIN_INTERFACE
#include "stm32f10x.h"
#include "lcd.h"
#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "math.h"

// ��Ļ��������
#define CENTER_X 120
#define CENTER_Y 160
// ָ�볤��
#define HOUR_HAND_LENGTH 50
#define MINUTE_HAND_LENGTH 70

// ���Ƕ�ת��Ϊ����
#define DEG_TO_RAD(angle)((angle) * 3.1415 / 180.0)

void CalculatePointerCoordinates(uint16_t centerX, uint16_t centerY, float angle, uint16_t length, uint16_t *x, uint16_t *y);
void draw_clock(void);
void earse_clock(uint16_t hour, uint16_t minute);
void draw_mainInterface(void);

#endif
