#include "main_interface.h"

// 计算指针终点坐标
void CalculatePointerCoordinates(uint16_t centerX, uint16_t centerY, float angle, uint16_t length, uint16_t *x, uint16_t *y) {
    *x = centerX + length * cos(DEG_TO_RAD(angle - 90));
    *y = centerY + length * sin(DEG_TO_RAD(angle - 90));
}
// 绘制表针
void draw_clock() {
    uint16_t hour_x, hour_y, minute_x, minute_y;
    // 计算时针和分针的角度
    float angle_hour = (calendar.hour % 12 + calendar.min / 60.0) * 30.0;
    float angle_minute = calendar.min * 6.0;
    // 计算时针和分针终点坐标
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_hour, HOUR_HAND_LENGTH, &hour_x, &hour_y);
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_minute, MINUTE_HAND_LENGTH, &minute_x, &minute_y);
    // 绘制时针
    POINT_COLOR = RED;
    LCD_DrawLine(CENTER_X, CENTER_Y, hour_x, hour_y);
    // 绘制分针
    POINT_COLOR = BLUE;
    LCD_DrawLine(CENTER_X, CENTER_Y, minute_x, minute_y);
}
// 擦除表针
void earse_clock(uint16_t hour, uint16_t minute) {
    uint16_t hour_x, hour_y, minute_x, minute_y;
    // 计算时针和分针的角度
    float angle_hour = (hour % 12 + minute / 60.0) * 30.0;
    float angle_minute = minute * 6.0;
    // 计算时针和分针终点坐标
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_hour, HOUR_HAND_LENGTH, &hour_x, &hour_y);
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_minute, MINUTE_HAND_LENGTH, &minute_x, &minute_y);
    POINT_COLOR = WHITE;
    // 绘制时针
    LCD_DrawLine(CENTER_X, CENTER_Y, hour_x, hour_y);
    // 绘制分针
    LCD_DrawLine(CENTER_X, CENTER_Y, minute_x, minute_y);
}

u16 hour = 0;
u16 minute = 0;
u16 former_hour = 0;
u16 former_minute = 0;
u8 t = 0;
void draw_mainInterface(void) {
    former_hour = hour;
    former_minute = minute;
    hour = calendar.hour;
    minute = calendar.min;
    if (t != calendar.sec) {
        t = calendar.sec;
        // 显示时间
        POINT_COLOR = BLUE; // 设置字体为蓝色
        switch (calendar.week) {
        case 0:
            LCD_ShowString(145, 280, 200, 16, 16, "Sunday   ");
            break;
        case 1:
            LCD_ShowString(145, 280, 200, 16, 16, "Monday   ");
            break;
        case 2:
            LCD_ShowString(145, 280, 200, 16, 16, "Tuesday  ");
            break;
        case 3:
            LCD_ShowString(145, 280, 200, 16, 16, "Wednesday");
            break;
        case 4:
            LCD_ShowString(145, 280, 200, 16, 16, "Thursday ");
            break;
        case 5:
            LCD_ShowString(145, 280, 200, 16, 16, "Friday   ");
            break;
        case 6:
            LCD_ShowString(145, 280, 200, 16, 16, "Saturday ");
            break;
        }
        LCD_ShowString(60, 280, 200, 16, 16, "    -  -  ");
		//LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
        LCD_ShowxNum(60, 280, calendar.w_year, 4, 16,0x80);
        LCD_ShowxNum(100, 280, calendar.w_month, 2, 16,0x80);
        LCD_ShowxNum(124, 280, calendar.w_date, 2, 16,0x80);
        LCD_ShowString(60, 300, 200, 16, 16, "  :  :  ");
        LCD_ShowxNum(60, 300, calendar.hour, 2, 16,0x80);
		LCD_ShowxNum(84, 300, calendar.min, 2, 16,0x80);
        LCD_ShowxNum(108, 300, calendar.sec, 2, 16,0x80);
        LED0_Toggle;
    }
    if (hour != former_hour || minute != former_minute) {
        earse_clock(former_hour, former_minute);
        draw_clock();
    }
    delay_ms(10);
}
