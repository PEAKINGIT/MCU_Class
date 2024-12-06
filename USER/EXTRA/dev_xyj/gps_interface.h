#ifndef __GPS_IF_H_
#define __GPS_IF_H

#include "gui_g9.h"
#include "math.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "lcd.h"

#define SCALING (1) // 0.00001degree/(SCALING):1 Pixel
#define PI_S (3.14) // short pi

/**
 * @brief data that might be used by others
 */
typedef struct gps_inter_face_data {
    uint32_t page_tick; // page init tick
    uint32_t lat_set;   // latitude need to /100000
    uint8_t ns_set;     // 'N'/'S'
    uint32_t lon_set;   // longitude need to /100000
    uint8_t ew_set;     // 'E'/'W'
    // �?心锚定点坐标
    uint32_t lat_cen; // latitude need to /100000
    uint32_t lon_cen; // longitude need to /100000
} GpsInterFaceData;

extern InterFaceObj gpsInterface;

void gpsGui_Init(InterFaceObj *obj_ptr);
void gpsGui_Refresh(InterFaceObj *obj_ptr);
IE_JUDGE gpsGui_ExitJudge(InterFaceObj *obj_ptr);
void gpsGui_Exit(InterFaceObj *obj_ptr);

uint32_t draw_Direct(uint16_t x1, uint16_t y1,
                     uint16_t x2, uint16_t y2,
                     uint8_t r);

void DrawArrow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t arrow_size);
#endif