#include "calendar_interface.h"

int isBetween(int month, int day, int startMonth, int startDay, int endMonth, int endDay) {
    if (startMonth == endMonth) return (month == startMonth && day >= startDay && day < endDay);
    if (month == startMonth && day >= startDay) return 1;
    if (month == endMonth && day < endDay) return 1;
    if (month > startMonth && month < endMonth) return 1;

    return 0;
}

int getSolarTerm(int year, int month, int day) {
    for (int i = 0; i < sizeof(solarTerms) / sizeof(SolarTerm); i++) {
        int nextIndex = (i + 1) % (sizeof(solarTerms) / sizeof(SolarTerm));
        if (isBetween(month, day, solarTerms[i].month, solarTerms[i].day, solarTerms[nextIndex].month, solarTerms[nextIndex].day)) {
            return nextIndex;//(u8*)solarTerms[nextIndex].name
        }
    }
	return 0;
}

void draw_calendarInterface(){
	int year = calendar.w_year;
	int month = calendar.w_month;
	int date = calendar.w_date;
	int solarTerm;
	int day, day1;
	int i, j ,x ,y;
	//显示日期
	POINT_COLOR = BLUE;
	switch (calendar.week) {
		case 0:
			LCD_ShowString(100, 57, 200, 16, 16, "Sunday   ");
			break;
		case 1:
			LCD_ShowString(100, 57, 200, 16, 16, "Monday   ");
			break;
		case 2:
			LCD_ShowString(100, 57, 200, 16, 16, "Tuesday  ");
			break;
		case 3:
			LCD_ShowString(100, 57, 200, 16, 16, "Wednesday");
			break;
		case 4:
			LCD_ShowString(100, 57, 200, 16, 16, "Thursday ");
			break;
		case 5:
			LCD_ShowString(100, 57, 200, 16, 16, "Friday   ");
			break;
		case 6:
			LCD_ShowString(100, 57, 200, 16, 16, "Saturday ");
			break;
	}
	// LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
	LCD_ShowString(17, 57, 200, 16, 16, "    -  -  ");
	LCD_ShowxNum(17, 57, year, 4, 16, 0x80);
	LCD_ShowxNum(57, 57, month, 2, 16, 0x80);
	LCD_ShowxNum(81, 57, date, 2, 16, 0x80);
	
	//显示节气
	solarTerm = getSolarTerm(year, month, date);
	Show_Str(80, 224, 200, 16, "上一节气:" ,16,0);
	LCD_ShowString(150, 224, 200, 16, 16, "  -  ");
	LCD_ShowxNum(150, 224, solarTerms[(solarTerm-1)>=0? solarTerm-1:23].month, 2, 16, 0x80);
	LCD_ShowxNum(174, 224, solarTerms[(solarTerm-1)>=0? solarTerm-1:23].day, 2, 16, 0x80);
	Show_Str(190, 224, 200, 16, (u8*)solarTerms[(solarTerm-1)>=0? solarTerm-1:23].name ,16,0);
	Show_Str(80, 244, 200, 16, "下一节气:" ,16,0);
	LCD_ShowString(150, 244, 200, 16, 16, "  -  ");
	LCD_ShowxNum(150, 244, solarTerms[solarTerm].month, 2, 16, 0x80);
	LCD_ShowxNum(174, 244, solarTerms[solarTerm].day, 2, 16, 0x80);
	Show_Str(190, 244, 200, 16, (u8*)solarTerms[solarTerm].name ,16,0);
	//绘制日历
	POINT_COLOR = BLACK;
	LCD_ShowString(17, 83, 200, 16, 16, "MON");
	LCD_ShowString(47, 83, 200, 16, 16, "TUE");
	LCD_ShowString(77, 83, 200, 16, 16, "WED");
	LCD_ShowString(107, 83, 200, 16, 16, "THU");
	LCD_ShowString(137, 83, 200, 16, 16, "FRI");
	LCD_ShowString(167, 83, 200, 16, 16, "SAT");
	LCD_ShowString(197, 83, 200, 16, 16, "SUN");
	
	//判断年月
	day = 0;
	for (i = 1900; i < year; i++){	//将输入年之前的年份天数求和
		if ((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0))	day += 366;
		else day += 365;
	}
	day1 = 0;
	for (j = 1;j <= month; j++){ 	//将输入年中的每月统计到总天数中
		switch (j) {
			case 4:
			case 6:
			case 9:
			case 11:
				day1 = 30;
				break;
			case 2:
				if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) day1 =29;
				else day1 = 28;
				break;
			default:
				day1 = 31;
				break;
		}
		if (j < month) day += day1;
	}
	//绘制日历
	x = 19; y = 103;
	j = day % 7 ;
	for( i = 0; i < j; i++) {
		x += 30;
	}
	for (i = 1; i <= day1; i++) {
		if ( i == date) POINT_COLOR = BLUE;
		else POINT_COLOR = BLACK;
		LCD_ShowxNum( x, y, i, 2, 16, 0x80);
		x += 30;
		if ((i + day) % 7  == 0){ x = 19; y += 20; }
	}
}

void Load_calendarInterface(void) {
	u8 key;
	
	LCD_Clear(WHITE);
	//绘制边界
	POINT_COLOR = RED;
	LCD_DrawRectangle(15, 55, 224, 264);
	//绘制日历
	draw_calendarInterface();

    while (1) {
        key = KEY_Scan(0);
        if (key == KEY0_PRES) break;
        if (key == KEY1_PRES) {
            LED1_Toggle;
        }
        delay_ms(100);
    }
    current_page = EMPTY_INTREFACE; // 主界面退出回到menu
	LCD_Clear(WHITE);
}
