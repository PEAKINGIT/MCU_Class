#include "main_interface.h"

// ����ָ���յ�����
void CalculatePointerCoordinates(uint16_t centerX, uint16_t centerY, float angle, uint16_t length, uint16_t *x, uint16_t *y) {
    *x = centerX + length * cos(DEG_TO_RAD(angle - 90));
    *y = centerY + length * sin(DEG_TO_RAD(angle - 90));
}
// ���Ʊ���
void draw_clock() {
    uint16_t hour_x, hour_y, minute_x, minute_y;
    // ����ʱ��ͷ���ĽǶ�
    float angle_hour = (calendar.hour % 12 + calendar.min / 60.0) * 30.0;
    float angle_minute = minute * 6.0;
    // ����ʱ��ͷ����յ�����
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_hour, HOUR_HAND_LENGTH, &hour_x, &hour_y);
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_minute, MINUTE_HAND_LENGTH, &minute_x, &minute_y);
    // ����ʱ��
    POINT_COLOR = RED;
    LCD_DrawLine(CENTER_X, CENTER_Y, hour_x, hour_y);
    // ���Ʒ���
    POINT_COLOR = BLUE;
    LCD_DrawLine(CENTER_X, CENTER_Y, minute_x, minute_y);
}
// ��������
void earse_clock(uint16_t hour, uint16_t minute) {
    uint16_t hour_x, hour_y, minute_x, minute_y;
    // ����ʱ��ͷ���ĽǶ�
    float angle_hour = (hour % 12 + minute / 60.0) * 30.0;
    float angle_minute = minute * 6.0;
    // ����ʱ��ͷ����յ�����
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_hour, HOUR_HAND_LENGTH, &hour_x, &hour_y);
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_minute, MINUTE_HAND_LENGTH, &minute_x, &minute_y);
    POINT_COLOR = WHITE;
    // ����ʱ��
    LCD_DrawLine(CENTER_X, CENTER_Y, hour_x, hour_y);
    // ���Ʒ���
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
        // ��ʾʱ��
        POINT_COLOR = BLUE; // ��������Ϊ��ɫ
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
        LCD_ShowNum(60, 280, calendar.w_year, 4, 16);
        LCD_ShowNum(100, 280, calendar.w_month, 2, 16);
        LCD_ShowNum(124, 280, calendar.w_date, 2, 16);
        LCD_ShowString(60, 300, 200, 16, 16, "  :  :  ");
        LCD_ShowNum(60, 300, calendar.hour, 2, 16);
        LCD_ShowNum(84, 300, calendar.min, 2, 16);
        LCD_ShowNum(108, 300, calendar.sec, 2, 16);
        LED0_Toggle;
    }
    if (hour != former_hour || minute != former_minute) {
        earse_clock(former_hour, former_minute);
        draw_clock();
    }
    delay_ms(10);
}