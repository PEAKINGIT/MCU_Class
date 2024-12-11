#include "gps_interface.h"

uint8_t ns_set = 'N';	//半球设置
uint8_t ew_set = 'E';

uint32_t lat_set = (DFT_LAT); // 目标点的坐标
uint32_t lon_set = (DFT_LON);
uint32_t lat_cen = DFT_LAT; // 中心点经纬度 默认设在教学楼
uint32_t lon_cen = DFT_LON;

uint32_t page_tick = 0;
uint32_t t_last1, t_last2;
// 测试经纬 人的坐标
uint32_t test_lat = DFT_LAT, test_lon = DFT_LON, test_i = 0;
int x_last = 30, y_last = 30;
uint32_t cur_lat,cur_lon;
static uint8_t *title = (uint8_t *)"GPS Locating Map";

//复位当前位置
void location_Rst(void){
	cur_lat = lat_cen;
	cur_lon = lon_cen;
}

// 通过GPS模块数据更新当前位置
// GPS本身数据的更新需要在界面主循环里调用gps_app.c/Gps_Receive_Handle()
void location_Get(void){
	cur_lat = gpsData.latitude;
	cur_lon = gpsData.longitude;
}

void gpsGui_Init(void) {
    page_tick = globalTick_Get();
    t_last1 = page_tick;
    t_last2 = page_tick;
    LCD_Clear(WHITE);
    POINT_COLOR = BLACK;
    LCD_ShowString(10, 10, 150, 20, 12, title);
    DrawArrow(10, 30, 40, 30, 5); // 向东箭头
    LCD_ShowChar(40, 30, 'E', 12, 0);
    DrawArrow(10, 30, 10, 60, 5);
    LCD_ShowChar(10, 50, 'N', 12, 0);
    POINT_COLOR = GRAY;
    LCD_Draw_Circle(G_WIDTH / 2, G_HEIGHT / 2, G_RAD); // Middle of gui
}

void gpsGui_Load(void) {
	int32_t dlat, dlon;
    uint32_t cxy; // cross xy
	dlat = test_lat - lat_cen;
    dlon = test_lon - lon_cen;
	gpsGui_Init();
    while (1) {
        /* use t_last2 as time cnt*/
        if ((globalTick_Get() - t_last2) > 50) {
            // LCD_Clear(WHITE);
            LCD_Fill(1, 70, 240, 260, WHITE); // 局部清空
            POINT_COLOR = (GRAY);
            LCD_Draw_Circle(G_WIDTH / 2, G_HEIGHT / 2, G_RAD); // Middle of gui
            POINT_COLOR = (RED);
            cxy = draw_Direct(G_WIDTH / 2, G_HEIGHT / 2,
                              G_WIDTH / 2 + dlon / (SCALING),
                              G_HEIGHT / 2 + dlat / (SCALING), G_RAD);

            t_last2 = globalTick_Get();
        }
        /* use t_last1 as time cnt*/
        if ((globalTick_Get() - t_last1) > 100) {
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
            t_last1 = globalTick_Get();
        }
        delay_ms(100);
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
// 画方向指示线 bresenham法 过圆圈范围不画线 并画一个点
// 返回交点坐标
uint32_t draw_Direct(uint16_t x1, uint16_t y1,
                     uint16_t x2, uint16_t y2,
                     uint8_t r) {
    uint16_t tx = x1, ty = y1;
    uint32_t ret;
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
    return 0xFFFFFFFF;
}

int32_t my_pow(int32_t x,uint32_t n){
	uint32_t i = 0;
	int32_t ret = 1;
	for(i=0;i<n;i++){
		ret*=x;
	}
	return ret;
}

// 画箭头函数，箭头的起点 (x1, y1)，终点 (x2, y2)，箭头头部大小 arrow_size
void DrawArrow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t arrow_size) {
    LCD_DrawLine(x1, y1, x2, y2);
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    float angle = atan2f(dy, dx); // 计算箭头线与水平线的夹角
    uint16_t arrow_angle = 30;    // 箭头头部的夹角大小
    // 计算箭头两侧的角度 逆时针顺序
    float fst_ang = angle + PI_S / 180.0 * arrow_angle;
    float sec_ang = angle - PI_S / 180.0 * arrow_angle;
    // 计算箭头头部的两个点
    uint16_t x_left = x2 - arrow_size * cosf(fst_ang);
    uint16_t y_left = y2 - arrow_size * sinf(fst_ang);
    uint16_t x_right = x2 - arrow_size * cosf(sec_ang);
    uint16_t y_right = y2 - arrow_size * sinf(sec_ang);
    // 画箭头两侧的线
    LCD_DrawLine(x2, y2, x_left, y_left);
    LCD_DrawLine(x2, y2, x_right, y_right);
}
