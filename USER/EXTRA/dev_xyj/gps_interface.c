#include "gps_interface.h"

uint8_t ns_set = 'N'; // ��������
uint8_t ew_set = 'E';

uint32_t lat_set = (DFT_LAT); // Ŀ��������
uint32_t lon_set = (DFT_LON);
uint32_t lat_cen = DFT_LAT; // ���ĵ㾭γ�� Ĭ�����ڽ�ѧ¥
uint32_t lon_cen = DFT_LON;

uint32_t page_tick = 0;
uint32_t t_last1, t_last2;
// ���Ծ�γ �˵�����
uint32_t test_lat = DFT_LAT, test_lon = DFT_LON, test_i = 0;
int x_last = 30, y_last = 30;
uint32_t cur_lat, cur_lon;
static uint8_t *title = (uint8_t *)"GPS Locating Map";
uint8_t str_p[40] = {0};

// ����λ�ø�λ
void center_Rst(void) {
    lat_cen = DFT_LAT; // ���ĵ㾭γ�� Ĭ�����ڽ�ѧ¥
    lon_cen = DFT_LON;
}

// �赱ǰλ��Ϊ����λ��
void center_Cur(void) {
    lat_cen = cur_lat;
    lon_cen = cur_lon;
}

// ��λ��ǰλ��
void location_Rst(void) {
    cur_lat = lat_cen;
    cur_lon = lon_cen;
}

// ͨ��GPSģ�����ݸ��µ�ǰλ��
// GPS�������ݵĸ�����Ҫ�ڽ�����ѭ�������gps_app.c/Gps_Receive_Handle()
void location_Get(void) {
    int32_t dlat, dlon;
    dlat = gpsData.latitude - lat_cen;
    dlon = gpsData.longitude - lon_cen;

    // ���������ų�
    if (abs(dlat) > (1 << 15) || abs(dlon) > (1 << 15)) {
        return;
    } else {
        cur_lat = gpsData.latitude;
        cur_lon = gpsData.longitude;
    }
}

void gpsGui_Init(void) {
    ai_load_picfile("0:/PICTURE/4-location.jpg", 16, 56, 208, 208, 1);
    delay_ms(800);
    page_tick = globalTick_Get();
    t_last1 = page_tick;
    t_last2 = page_tick;
    location_Rst();

    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    LCD_DrawRectangle(LCD_XSTART - 1, LCD_YSTART - 1, LCD_XEND + 1, LCD_YEND + 1);
    POINT_COLOR = BLACK;
    LCD_ShowString(LCD_XSTART, LCD_YSTART + 12, 120, 20, 12, title);
    LCD_ShowString(LCD_XSTART + 120, LCD_YSTART + 1, 100, 20, 12, "0.005deg");
    LCD_DrawLine(LCD_XSTART + 120, LCD_YSTART + 14, LCD_XSTART + 170, LCD_YSTART + 14);

    DrawArrow(LCD_XSTART, LCD_YSTART + 24, LCD_XSTART + 20, LCD_YSTART + 24, 5); // �򶫼�ͷ
    LCD_ShowChar(LCD_XSTART + 20, LCD_YSTART + 24, 'E', 12, 0);
    DrawArrow(LCD_XSTART, LCD_YSTART + 24, LCD_XSTART, LCD_YSTART + 44, 5);
    LCD_ShowChar(LCD_XSTART, LCD_YSTART + 44, 'N', 12, 0);
    POINT_COLOR = GRAY;
    LCD_Draw_Circle(G_WIDTH / 2, G_HEIGHT / 2, G_RAD); // Middle of gui
}

void gpsGui_Load(void) {
    int32_t dlat, dlon;
    // uint32_t cxy; // cross xy
    u8 key;
    gpsGui_Init();
    while (1) {
        /* use t_last2 as time cnt*/
        if (!GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_0)) {
            // debug io �͵�ƽ Ĭ��
            Gps_Receive_Handle();
            location_Get();
        } else {
            cur_lat = test_lat;
            cur_lon = test_lon;
            /* use t_last1 as time cnt*/
            GPS_Test(&t_last1);
        }
        dlat = cur_lat - lat_cen;
        dlon = cur_lon - lon_cen;
        // printf("dlat:%d dlon:%d \r\n", dlat, dlon);

        key = KEY_Scan(0);
        if (key == KEY0_PRES) break;
        if (key == KEY1_LONGP) {
            // �赱ǰλ��Ϊ����
            center_Cur();
            Show_Str_Mid(LCD_XSTART + 10, LCD_YSTART + 80, "Center to Cur Done!", 12, 168);
            delay_ms(800);
        }
        if (key == KEY1_PRES) {
            // ���ĸ�λ
            center_Rst();
            Show_Str_Mid(LCD_XSTART + 10, LCD_YSTART + 80, "Reset Center Done!", 12, 168);
            delay_ms(800);
        }
        if ((globalTick_Get() - t_last2) > 20) {
            // LCD_Clear(WHITE);
            LCD_Fill(G_WIDTH / 2 - G_RAD, G_HEIGHT / 2 - G_RAD, G_WIDTH / 2 + G_RAD, G_HEIGHT / 2 + G_RAD, WHITE); // �ֲ����
            POINT_COLOR = (BLACK);
            LCD_Draw_Circle(G_WIDTH / 2, G_HEIGHT / 2, G_RAD + 2);  // Middle of gui
            LCD_Draw_Circle(G_WIDTH / 2, G_HEIGHT / 2, G_RAD);      // Middle of gui
            LCD_Draw_Circle(G_WIDTH / 2, G_HEIGHT / 2, G_RAD / 20); // Middle of gui
            POINT_COLOR = (RED);

            draw_Direct(G_WIDTH / 2, G_HEIGHT / 2,
                        G_WIDTH / 2 + dlon / (SCALING),
                        G_HEIGHT / 2 + dlat / (SCALING), G_RAD);

            t_last2 = globalTick_Get();
        }
        sprintf((char *)str_p, "current:%.5f%c,%.5f%c", cur_lat / (100000.0), ns_set, cur_lon / (100000.0), ew_set);
        LCD_ShowString(LCD_XSTART + 2, LCD_YEND - 14, 200, 16, 12, str_p);
        sprintf((char *)str_p, "center:%.5f%c,%.5f%c", lat_cen / (100000.0), ns_set, lon_cen / (100000.0), ew_set);
        LCD_ShowString(LCD_XSTART + 2, LCD_YEND - 26, 200, 16, 12, str_p);
        // delay_ms(100);
    }
    current_page = MENU;
    LCD_Clear(WHITE);
}

void GPS_Test(uint32_t *tp) {
    if ((globalTick_Get() - *tp) > 100) {
        test_i++;
        if (test_i < 80) {
            test_lon += 2;
        } else if (test_i >= 80 && test_i < 160) {
            test_lat += 2;
        } else if (test_i >= 160 && test_i < 240) {
            test_lon -= 2;
        } else {
            test_lat -= 2;
            if (test_i == 320) {
                test_i = 0;
            }
        }
        *tp = globalTick_Get();
    }
}
/**
 * @brief funcs assist the gui display
 */

// get the cross point of a line & a circle
// line x1,y1->x2,y2
// circle x1,y1 r
// ret[31:16]=tx [15:0]=ty
// is there is no cross return 0xFFFF FFFF
// ������ָʾ�� bresenham�� ��ԲȦ��Χ������ ����һ����
// ���ؽ�������
uint32_t draw_Direct(uint16_t x1, uint16_t y1,
                     uint16_t x2, uint16_t y2,
                     uint8_t r) {
    uint16_t tx = x1, ty = y1;
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1;
    else if (delta_x == 0)
        incx = 0;
    else {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0;
    else {
        incy = -1;
        delta_y = -delta_y;
    }
    distance = (delta_x > delta_y) ? delta_x : delta_y;
    for (t = 0; t <= distance + 1; t++) {
        tx = uRow;
        ty = uCol;
        POINT_COLOR = RED;
        if ((my_pow(tx - x1, 2) + my_pow(ty - y1, 2)) >= my_pow(r * 9 / 10, 2)) {
            LCD_Draw_Circle(tx, ty, 10);
            return (tx << 16) + ty;
        }
        LCD_DrawPoint(uRow, uCol);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
    POINT_COLOR = MAGENTA;
    LCD_Draw_Circle(tx, ty, 5);
    return 0xFFFFFFFF;
}

int32_t my_pow(int32_t x, uint32_t n) {
    uint32_t i = 0;
    int32_t ret = 1;
    for (i = 0; i < n; i++) {
        ret *= x;
    }
    return ret;
}

// ����ͷ��������ͷ����� (x1, y1)���յ� (x2, y2)����ͷͷ����С arrow_size
void DrawArrow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t arrow_size) {
    LCD_DrawLine(x1, y1, x2, y2);
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    float angle = atan2f(dy, dx); // �����ͷ����ˮƽ�ߵļн�
    uint16_t arrow_angle = 30;    // ��ͷͷ���ļнǴ�С
    // �����ͷ����ĽǶ� ��ʱ��˳��
    float fst_ang = angle + PI_S / 180.0 * arrow_angle;
    float sec_ang = angle - PI_S / 180.0 * arrow_angle;
    // �����ͷͷ����������
    uint16_t x_left = x2 - arrow_size * cosf(fst_ang);
    uint16_t y_left = y2 - arrow_size * sinf(fst_ang);
    uint16_t x_right = x2 - arrow_size * cosf(sec_ang);
    uint16_t y_right = y2 - arrow_size * sinf(sec_ang);
    // ����ͷ�������
    LCD_DrawLine(x2, y2, x_left, y_left);
    LCD_DrawLine(x2, y2, x_right, y_right);
}

// debug io PF0
void GPS_DebugIO_Init(void) {
    GPIO_InitTypeDef GPIO_Initure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    GPIO_Initure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Initure.GPIO_Mode = GPIO_Mode_IPD;
    // ���� Ĭ�ϵ͵�ƽ

    GPIO_Init(GPIOF, &GPIO_Initure);
}
