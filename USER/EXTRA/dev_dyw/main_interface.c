#include "main_interface.h"

uint8_t *week_days[7] = {
    "Sunday   ",
    "Monday   ",
    "Tuesday  ",
    "Wednesday",
    "Thursday ",
    "Friday   ",
    "Saturday ",
};

// u16 hour = 0;
// u16 minute = 0;
u16 former_hour = 0;
u16 former_minute = 0;

const u8 touch_h = 12; // ����ʾ������Χ �ֱ����������½�
const u8 touch_w = 80;

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
    u16 x_base = 4 + LCD_XSTART;
    u16 y_base = 6 + LCD_YSTART;

    uint8_t str_temp[20];
    if (t != calendar.sec) {
        t = calendar.sec;
        // ��ʾʱ��
        POINT_COLOR = BLUE; // ��������Ϊ��ɫ
        strcpy((char *)str_temp, (char *)week_days[calendar.week]);
        LCD_ShowString(x_base, y_base, 100, 16, 16, str_temp); // ����
        LCD_ShowString(x_base, y_base + 20, 200, 16, 16, "    -  -  ");
        // // LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
        LCD_ShowxNum(x_base, y_base + 20, calendar.w_year, 4, 16, 0x80);
        LCD_ShowxNum(x_base + 40, y_base + 20, calendar.w_month, 2, 16, 0x80);
        LCD_ShowxNum(x_base + 64, y_base + 20, calendar.w_date, 2, 16, 0x80);
        LCD_ShowString(x_base, y_base + 40, 200, 16, 16, "  :  :  ");
        LCD_ShowxNum(x_base, 40 + y_base, calendar.hour, 2, 16, 0x80);
        LCD_ShowxNum(x_base + 24, 40 + y_base, calendar.min, 2, 16, 0x80);
        LCD_ShowxNum(x_base + 48, 40 + y_base, calendar.sec, 2, 16, 0x80);
        // LED0_Toggle;
        LCD_DrawRectangle(LCD_XSTART, LCD_YSTART, LCD_XSTART + 86, LCD_YSTART + 64);

        // center point
        POINT_COLOR = BLUE;
        LCD_Draw_Circle(120, 160, 5);
        POINT_COLOR = RED;
        LCD_Draw_Circle(120, 160, 3);

        LCD_Fill(LCD_XSTART, LCD_YEND - touch_h,
                 LCD_XSTART + touch_w, LCD_YEND, GREEN);
        LCD_Fill(LCD_XEND - touch_w, LCD_YEND - touch_h,
                 LCD_XEND, LCD_YEND, BLUE);
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

    current_page = MAIN_INTERFACE;
    LCD_Clear(WHITE);
    // ���Ʊ߽�
    POINT_COLOR = RED;
    LCD_DrawRectangle(15, 55, 224, 264);
    // �����ֱ�
    ai_load_picfile("0:/PICTURE/1-watch.jpg", 16, 56, 208, 208, 1);
    draw_clock();
    draw_mainInterface();
    while (1) {
        key = KEY_Scan(0);
        if (key == KEY0_PRES) {
            current_page = EMPTY_INTREFACE;
            break;
        }
        if (key == KEY1_PRES) {
            current_page = MENU; // �������˳��ص�menu
            break;
        }
        tp_dev.scan(0);
        if (tp_dev.sta & TP_PRES_DOWN) {
            // ������������
            if (touch_inRange(LCD_XSTART, LCD_YEND - touch_h,
                              LCD_XSTART + touch_w, LCD_YEND,
                              tp_dev.x[0], tp_dev.y[0])) {
                // ��ఴ��
                current_page = EMPTY_INTREFACE;
				tp_dev.sta = 0;
                break;
            } else if (touch_inRange(LCD_XEND - touch_w, LCD_YEND - touch_h,
                                     LCD_XEND, LCD_YEND,
                                     tp_dev.x[0], tp_dev.y[0])) {
                // �Ҳఴ��
                current_page = MENU; // �������˳��ص�menu
				tp_dev.sta = 0;
                break;
            }
        } else
            delay_ms(10); // û�а������µ�ʱ��
        delay_ms(100);
        draw_mainInterface();
    }
    LCD_Clear(WHITE);
}
