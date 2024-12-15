#include "menu.h"

static uint8_t *menu_title = "Menu"; // 页面标题
static Select selects[SELECT_NUM];   // 选项 结构数组
static uint16_t cur_index = 0;       // 当前选项下标
static uint16_t effect_index_num = 0;

const uint16_t funcx = 2 + LCD_XSTART; // 功能区相对起始坐标
const uint16_t funcy = (PAGE_INUM + 1) * (SELECT_H) + 2 + LCD_YSTART;

const uint8_t *empty_str = "Nothing Here~"; // 空界面字符串显示
InterfaceState current_page = MENU;         // 当前界面状态
// 界面函数指针数组
// 顺序注意和界面枚举保持相同
void (*interface_functions[MAX_INTERFACE])(void) = {
    display_empty,          // 空界面
    Load_MainInterface,     // 主界面
    DHT_interface,          // 温湿度曲线显示
    gpsGui_Load,            // GPS界面
    Load_calendarInterface, // 日历
    Menu_Load,              // 菜单
};

// 为select的特定下标项设置功能
void add_Select(uint16_t index, uint8_t *text, void (*select_event)(void), uint8_t type) {
    selects[index].select_event = select_event;
    selects[index].text = text;
    selects[index].type = type;
}

// 显示index1和index2之间的选项
void disp_selects(u16 index1, u16 index2) {
    for (u16 i = index1; i <= index2; i++) {
        if (i == cur_index) {
            // 选中高亮
            LCD_DrawRectangle(LCD_XSTART + 2, LCD_YSTART + SELECT_H * (i - index1 + 1) + 2, LCD_XEND - 2, LCD_YSTART + SELECT_H * (i - index1 + 1) + SELECT_H - 2);
        }
        LCD_DrawRectangle(LCD_XSTART, LCD_YSTART + SELECT_H * (i - index1 + 1), LCD_XEND, LCD_YSTART + SELECT_H * (i - index1 + 1) + SELECT_H);
        LCD_ShowNum(LCD_XSTART + 2, LCD_YSTART + SELECT_H * (i - index1 + 1) + 4, i, 2, 16);
        if (selects[i].text != NULL) {
            LCD_ShowString(LCD_XSTART + SELECT_H, LCD_YSTART + SELECT_H * (i - index1 + 1) + 4, 160, SELECT_H, 16, selects[i].text);
        }
    }
}

// 1 select 1-3
// 2 select 4-6
// 3 ...
void disp_MenuPage(uint16_t page) {
    LCD_Fill(LCD_XSTART, LCD_YSTART + 20, LCD_XEND, LCD_YEND, WHITE);
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 2, menu_title, 16, LCD_WIDTH);
    disp_selects((page - 1) * PAGE_INUM + 1, page * PAGE_INUM);
}

// 菜单初始化
void Menu_Init(void) {
    effect_index_num = 0;
    add_Select(1, "Main Interface", s2Main, INTERF_S);
    effect_index_num++;
    add_Select(2, "TemperatureHumidity", s2DHT, INTERF_S);
    effect_index_num++;
    add_Select(3, "GPS", s2GPS, INTERF_S);
    effect_index_num++;
    add_Select(4, "Calendar", s2Calendar, INTERF_S);
    effect_index_num++;
    // FUNCS
    add_Select(5, "WIFI Reset", rstWIFI, FUNC);
    effect_index_num++;
    add_Select(6, "!!!Help!!!", CallingHelp, FUNC);
    effect_index_num++;
    add_Select(7, "Time Adjust", adjTime, FUNC);
    effect_index_num++;
	add_Select(8, "Back Count", setBackCount, FUNC);
    effect_index_num++;

    cur_index = 1;

    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    LCD_DrawRectangle(LCD_XSTART - 1, LCD_YSTART - 1, LCD_XEND + 1, LCD_YEND + 1);
    POINT_COLOR = BLUE;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 2, menu_title, 16, LCD_WIDTH);
    disp_MenuPage(1);
}

void Menu_Load(void) {
    u8 key;
    u8 page = 1;       // 当前页面 选项放不下 use多个页面
    u8 break_flag = 0; // 跳出循环标志位

    Menu_Init();
    while (1) {
        key = KEY_Scan(0);
        switch (key) {
        case WKUP_PRES:
            // confirm select
            if (selects[cur_index].type != EMPTY) {
                // 类型不为空
                selects[cur_index].select_event();
                // 如果为界面切换型选项 退出循环
                if (selects[cur_index].type == INTERF_S) break_flag = 1;
            }
            disp_MenuPage(page);
            break;
        case KEY0_PRES:
            // change cur index
            cur_index++;
            if (cur_index > PAGE_INUM * page) {
                // 超页面回第一项
                cur_index = (page - 1) * PAGE_INUM + 1;
            }
            disp_MenuPage(page);
            // 页面刷新
            break;
        case KEY0_LONGP:
            // 长按KEY0 翻页
            page++;
            if (page > PAGE_NUM) {
                page = 1;
            }
            cur_index = (page - 1) * PAGE_INUM + 1;
            disp_MenuPage(page);
        default:
            break;
        }

        // 切换界面则退出loop
        if (break_flag) break;
    }
    /* do before exit */
    if (current_page == MENU) {
        current_page = MAIN_INTERFACE;
    }
    LCD_Clear(WHITE);
}

// 空界面 显示完一句话直接退出
void display_empty(void) {
    LCD_ShowString(60, 160, 120, 20, 12, (u8 *)empty_str);
}

/**
 * @brief 选项函数
 */
// s2 == switch to
void s2Main(void) {
    current_page = MAIN_INTERFACE;
}

//
void s2DHT(void) {
    current_page = DHT_INTREFACE;
}

void s2GPS(void) {
    current_page = GPS_GUI;
}

void s2Calendar(void) {
    current_page = CALENDAR;
}

// FUNC

// 重新设置WIFI的STA连接
void rstWIFI(void) {
    atk_8266_send_cmd("AT+CIPCLOSE", "OK", 100);  // 关闭连接
    atk_8266_quit_trans();                        // 退出透传
    atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); // 关闭透传模式
    WIFI_STA_Set();
}

// 校正RTC时间
//  从GPS模块获取时间进行校正
//  or 手动校正
void adjTime(void) {
    nmea_utc_time temptime;
    u8 key;
    u8 mode = 0;

    // 当前要设置的对象
    u8 cur_subj = 0; // 0:sec 1:min 2:hour

    FuncClear();
    LCD_ShowString(funcx, funcy + 2, 120, SELECT_H, 12, "GPS TIME ADJUST");
    while (1) {
        key = KEY_Scan(0);
        if (key == KEY0_LONGP) break; // key0 长按取消设置
        if (mode == 0) {
            Gps_Receive_Handle();
            utc_to_local_time(&gpsData.utc, 8, &temptime);
        } else {
            if (key == KEY0_PRES) {
                cur_subj++;
                if (cur_subj > 5) cur_subj = 0;
                POINT_COLOR = WHITE;
                LCD_DrawLine(funcx, funcy + 15,
                             funcx + 120, funcy + 15);
                LCD_DrawLine(funcx, funcy + 34,
                             funcx + 120, funcy + 34);
                POINT_COLOR = BLUE;
                LCD_DrawLine(funcx + 48 - (cur_subj % 3) * 24,
                             funcy + 15 + (cur_subj < 3) * 19,
                             funcx + 72 - (cur_subj % 3) * 24,
                             funcy + 15 + (cur_subj < 3) * 19);
            }
            if (key == KEY1_PRES) {
                if (cur_subj == 0) {
                    temptime.sec++;
                    temptime.sec %= 60;
                } else if (cur_subj == 1) {
                    temptime.min++;
                    temptime.min %= 60;
                } else if (cur_subj == 2) {
                    // ==2
                    temptime.hour++;
                    temptime.hour %= 24;
                } else if (cur_subj == 3) {
                    // day
                    temptime.date++;
                    if(temptime.date >31) temptime.date = 1;
                } else if (cur_subj == 4) {
                    temptime.month++;
                    if(temptime.month > 12) temptime.month = 1;
                } else {
                    temptime.year++;
                    if (temptime.year > 2040) {
                        temptime.year = 2020;
                    }
                }
            }
        }
        if (key == KEY1_LONGP) { // key1 long 模式选择
            mode = 1 - mode;
            if (mode == 0) {
                LCD_ShowString(funcx, funcy + 2, 120, SELECT_H, 12, "GPS TIME ADJUST");
            } else {
                temptime.year = calendar.w_year;
                temptime.month = calendar.w_month;
                temptime.date = calendar.w_date;
                temptime.hour = calendar.hour;
                temptime.min = calendar.min;
                temptime.sec = calendar.sec;
				POINT_COLOR = BLUE;
                LCD_DrawLine(funcx + 48 - (cur_subj % 3) * 24,
                             funcy + 15 + (cur_subj < 3) * 19,
                             funcx + 72 - (cur_subj % 3) * 24,
                             funcy + 15 + (cur_subj < 3) * 19);
                LCD_ShowString(funcx, funcy + 2, 120, SELECT_H, 12, "MANUAL ADJUST");
            }
        }
        if (key == WKUP_PRES) {
            // 确定选择
            RTC_EnterConfigMode(); // 允许配置
            RTC_Set(temptime.year, temptime.month, temptime.date, temptime.hour, temptime.min, temptime.sec);
            // 设置时间
            RTC_ExitConfigMode(); // 退出配置模式
            LCD_ShowString(funcx, funcy + 2, 120, SELECT_H, 12, "TIME ADJUST DONE");
            delay_ms(800);
            break;
        }

        LCD_ShowString(funcx, funcy + 16, 120, 12, 16, "    -  -  ");
        // LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
        LCD_ShowxNum(funcx, funcy + 16, temptime.year, 4, 16, 0x80);
        LCD_ShowxNum(funcx + 40, funcy + 16, temptime.month, 2, 16, 0x80);
        LCD_ShowxNum(funcx + 64, funcy + 16, temptime.date, 2, 16, 0x80);
        LCD_ShowString(funcx, funcy + 36, 200, 16, 16, "  :  :  ");
        LCD_ShowxNum(funcx, funcy + 36, temptime.hour, 2, 16, 0x80);
        LCD_ShowxNum(funcx + 24, funcy + 36, temptime.min, 2, 16, 0x80);
        LCD_ShowxNum(funcx + 48, funcy + 36, temptime.sec, 2, 16, 0x80);
        delay_ms(100);
    }
    FuncClear();
}

void FuncClear(void) {
    LCD_Fill(funcx, funcy, LCD_XEND, LCD_YEND, WHITE);
}

// 呼救函数 蜂鸣器 向上位机发送信息 闪灯
void CallingHelp(void) {
    LED0(0);
    LED1(0);
    BEEP = 0;

    for (u16 i = 0; i < 3; i++) {
        BEEP = 1;
        LED0_Toggle;
        LED1_Toggle;
        delay_ms(100);
        LED0_Toggle;
        LED1_Toggle;
        delay_ms(100);
        BEEP = 0;
        atk_8266_quit_trans();
        if (!atk_8266_send_cmd("AT+CIPSEND", "OK", 20)) {
            // 返回为0 成功进入透传发送
            u3_printf("In danger!!!");
            u3_printf("Lat:%u,Lon:%u,", gpsData.latitude, gpsData.longitude);
        }
        atk_8266_quit_trans();
    }
    LED0(1);
    LED1(1);
    BEEP = 0;
}

void setBackCount(void) {
    // u32 timecount = RTC_GetCounter();
    u32 cnts = 0; // 计时秒数
    u8 key;
    u8 breakflag = 0;
    FuncClear();
    Show_Str(funcx, funcy + 2, 120, 16, "倒计时设置",12 ,0);
    LCD_ShowNum(funcx, funcy + 14, cnts, 4, 12);
    while (1) {
        key = KEY_Scan(0);
		if(key == KEY0_LONGP) break;
        switch (key) {
        case KEY0_PRES:
            cnts++;
            LCD_ShowNum(funcx, funcy + 14, cnts, 4, 12);
            break;
        case KEY1_PRES:
            if (cnts > 0) cnts--;
            LCD_ShowNum(funcx, funcy + 14, cnts, 4, 12);
            break;
        case WKUP_PRES:
            alarm_cnt = cnts;
            breakflag = 1;
            break;
        default:
            break;
        }
        if (breakflag) break;
    }
}
