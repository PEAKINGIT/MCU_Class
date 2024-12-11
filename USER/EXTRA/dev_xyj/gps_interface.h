#ifndef __GPS_IF_H_
#define __GPS_IF_H

#include "math.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "lcd.h"
#include "gps.h"
#include "gps_app.h"
#include "key.h"
#include "menu.h"

#define SCALING (2) // 0.00001degree/(SCALING):1 Pixel
#define PI_S (3.14) // short pi

#define G_RAD 80 //��Բ�뾶
#define G_WIDTH 240
#define G_HEIGHT 320

#define DFT_LAT (3026434) 	//default latitude
#define DFT_LON (12011965)	//default lontitude

void location_Rst(void);
void location_Get(void);
void gpsGui_Init(void);
void gpsGui_Load(void);

int32_t my_pow(int32_t x,uint32_t n);
uint32_t draw_Direct(uint16_t x1, uint16_t y1,
                     uint16_t x2, uint16_t y2,
                     uint8_t r);

void DrawArrow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t arrow_size);
#endif
