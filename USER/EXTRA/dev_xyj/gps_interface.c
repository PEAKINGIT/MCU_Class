#include "gps_interface.h"

/**
 * @brief data that might be used by others
 */
GpsInterFaceData gpsIfData = {
    .page_tick = 0,
    .lat_set = (3026434),	//目标点的坐标
    .ns_set = 'N',
    .lon_set = (12011965),
    .ew_set = 'E',
	.lat_cen = 3026434,	//中心点经纬度
	.lon_cen = 12011965,
};

InterFaceObj gpsInterface = {
    .init = gpsGui_Init,
    .looping = gpsGui_Refresh,
    .exit_judge = gpsGui_ExitJudge,
    .exit = gpsGui_Exit,
    .data_ptr = &gpsIfData,
};

/**
 * @brief data that is private for this file
 */
uint32_t t_last1, t_last2;
//测试经纬 人的坐标
uint32_t test_lat = 3026434, test_lon = 12011965,test_i=0;	
int x_last = 30, y_last = 30;
uint8_t *str = (uint8_t *)"GPS Locating Map";

void gpsGui_Init(InterFaceObj *obj_ptr) {
    GpsInterFaceData *p = (GpsInterFaceData *)obj_ptr->data_ptr;
    p->page_tick = globalTick_Get();
    t_last1 = p->page_tick;
    t_last2 = p->page_tick;
    LCD_Clear(WHITE);
	LCD_SetPointColor(BLACK);
	LCD_ShowString(150,120,150,20,16,str);
	DrawArrow(100,100,130,100,5);
	LCD_ShowChar(130,100,'E',12,0);
	DrawArrow(100,100,100,130,5);
	LCD_ShowChar(100,130,'N',12,0);
    LCD_SetPointColor(GRAY);
    LCD_Draw_Circle(G_WIDTH / 2, G_HEIGHT / 2, 100); // Middle of gui
}

void gpsGui_Refresh(InterFaceObj *obj_ptr) {
    GpsInterFaceData *p = (GpsInterFaceData *)obj_ptr->data_ptr;
	int32_t dlat,dlon;
	dlat = test_lat-p->lat_cen;
	dlon = test_lon-p->lon_cen;
    uint32_t cxy; // cross xy
    /* use t_last2 as time cnt*/
    if ((globalTick_Get() - t_last2) > 50) {
        //LCD_Clear(WHITE);
		LCD_Fill(140,140,361,361,WHITE);	//局部清空
        LCD_SetPointColor(GRAY);
        LCD_Draw_Circle(G_WIDTH / 2, G_HEIGHT / 2, 100); // Middle of gui
        LCD_SetPointColor(RED);
        cxy = draw_Direct(G_WIDTH / 2, G_HEIGHT / 2,
                    G_WIDTH / 2 + dlon/(SCALING),
                    G_HEIGHT / 2 + dlat/(SCALING), 100);

        t_last2 = globalTick_Get();
    }
    /* use t_last1 as time cnt*/
    if ((globalTick_Get() - t_last1) > 100) {
        test_i++;
		if(test_i<80){
			test_lon+=2;
		}else if(test_i>=80&&test_i<160){
			test_lat+=2;
		}else if(test_i>=160&&test_i<240){
			test_lon-=2;
		}else{
			test_lat-=2;
			if(test_i==320){
				test_i=0;
			}
		}
        t_last1 = globalTick_Get();
    }
    delay_ms(100);
}

IE_JUDGE gpsGui_ExitJudge(InterFaceObj *obj_ptr) {
    return I_CONTINUE;
}

void gpsGui_Exit(InterFaceObj *obj_ptr) {
    printf("gpsExit\r\n");
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
        if ((my_pow(tx - x1, 2) + my_pow(ty - y1, 2)) >= my_pow(r, 2)) {
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

// 画箭头函数，箭头的起点 (x1, y1)，终点 (x2, y2)，箭头头部大小 arrow_size
void DrawArrow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t arrow_size) {
    LCD_DrawLine(x1, y1, x2, y2);
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    float angle = atan2f(dy, dx);  // 计算箭头线与水平线的夹角
    uint16_t arrow_angle = 30;  // 箭头头部的夹角大小
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
