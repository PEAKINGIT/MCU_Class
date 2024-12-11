#include "main_interface.h"

uint8_t *week_days[7]={
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
u8 t = 0;

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

void draw_mainInterface(void) {
	u16 x_base = 10+LCD_XSTART;
	u16 y_base = 10+LCD_YSTART;
	uint8_t str_temp[20];
    if (t != calendar.sec) {
        t = calendar.sec;
        // 显示时间
        POINT_COLOR = BLUE; // 设置字体为蓝色
		strcpy((char *)str_temp,(char *)week_days[calendar.week]);
		LCD_ShowString(10+LCD_XSTART, 10+LCD_YSTART, 100, 16, 16, str_temp);	//星期
        LCD_ShowString(10+LCD_XSTART, 30+LCD_YSTART, 200, 16, 16, "    -  -  ");
        // // LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
        LCD_ShowxNum(10+LCD_XSTART, 30+LCD_YSTART, calendar.w_year, 4, 16, 0x80);
        LCD_ShowxNum(50+LCD_XSTART, 30+LCD_YSTART, calendar.w_month, 2, 16, 0x80);
        LCD_ShowxNum(74+LCD_XSTART, 30+LCD_YSTART, calendar.w_date, 2, 16, 0x80);
        LCD_ShowString(x_base, 46+y_base, 200, 16, 16, "  :  :  ");
        LCD_ShowxNum(x_base, 46+y_base, calendar.hour, 2, 16, 0x80);
        LCD_ShowxNum(x_base+24, 46+y_base, calendar.min, 2, 16, 0x80);
        LCD_ShowxNum(x_base+48, 46+y_base, calendar.sec, 2, 16, 0x80);
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

// 主页面的完整加载函数 包含初始化 循环 退出判断
void Load_MainInterface(void) {
    u16 former_hour = calendar.hour;
    u16 former_minute = calendar.min;
    u8 key;
	u32 no_input_last = 0;
	u8 wifi_isok = WIFI_OK;

	current_page = MAIN_INTERFACE;
    LCD_Clear(WHITE);
	//绘制边界
	POINT_COLOR = RED;
	LCD_DrawRectangle(15, 55, 224, 264);
	//绘制手表
	ai_load_picfile("0:/PICTURE/1-watch.jpg", 16, 56, 208, 208, 1);
    draw_clock();
	draw_mainInterface();
    while (1) {
		if (USART3_RX_STA & 0X8000) {
			no_input_last = globalTick_Get();
    	}else{
			if((globalTick_Get()-no_input_last)>=10000){
				//长时间无数据接收检查
				//wifi_isok = WIFI_ConnectCheck();	//测试时先注释了
				no_input_last = globalTick_Get();
			}
		}
        key = KEY_Scan(0);
		//WIFI_RcvHandle(wifi_isok);	//WIFI转发 测试时先注释了
        if (key == KEY0_PRES) {
			current_page = EMPTY_INTREFACE; // 主界面退出回到menu
			break;
		}
        if (key == KEY1_PRES) {
			current_page = MENU; // 主界面退出回到menu
            break;
        }
        delay_ms(100);
        draw_mainInterface();
    }
	LCD_Clear(WHITE);
}
