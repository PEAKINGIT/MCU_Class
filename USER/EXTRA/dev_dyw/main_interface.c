#include "main_interface.h"

// u16 hour = 0;
// u16 minute = 0;
u16 former_hour = 0;
u16 former_minute = 0;
u8 t = 0;

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
    float angle_minute = calendar.min * 6.0;
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

void draw_mainInterface(void) {
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
        // LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
        LCD_ShowxNum(60, 280, calendar.w_year, 4, 16, 0x80);
        LCD_ShowxNum(100, 280, calendar.w_month, 2, 16, 0x80);
        LCD_ShowxNum(124, 280, calendar.w_date, 2, 16, 0x80);
        LCD_ShowString(60, 300, 200, 16, 16, "  :  :  ");
        LCD_ShowxNum(60, 300, calendar.hour, 2, 16, 0x80);
        LCD_ShowxNum(84, 300, calendar.min, 2, 16, 0x80);
        LCD_ShowxNum(108, 300, calendar.sec, 2, 16, 0x80);
        LED0_Toggle;
    }
    if (calendar.hour != former_hour || calendar.min != former_minute) {
        earse_clock(former_hour, former_minute);
        draw_clock();
        former_hour = calendar.hour;
        former_minute = calendar.min;
    }
    delay_ms(10);
}

// ��ҳ����������غ��� ������ʼ�� ѭ�� �˳��ж�
void Load_MainInterface(void) {
    u16 former_hour = calendar.hour;
    u16 former_minute = calendar.min;
    u8 key;

    LCD_Clear(WHITE);
    if (Draw_Picture(0) != PIC_OK) {
        // ��ͼƬʧ�� ��ĳЩ����...
    }
    draw_clock();
    while (1) {
        key = KEY_Scan(0);
        if (key == KEY0_PRES) break;
        if (key == KEY1_PRES) {
            LED1_Toggle;
        }
        delay_ms(100);
        draw_mainInterface();
    }
    current_page = EMPTY_INTREFACE; // �������˳��ص�menu
	LCD_Clear(WHITE);
}
