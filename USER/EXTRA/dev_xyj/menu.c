#include "menu.h"

static uint8_t *menu_title = "Menu"; // 页面标题
static Select selects[SELECT_NUM];   // 选项 结构数组
static uint16_t cur_index = 0;       // 当前选项下标
static uint16_t effect_index_num = 0;

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
            LCD_DrawRectangle(LCD_XSTART + 2, LCD_YSTART + 20 * (i - index1 + 1) + 2, LCD_XEND - 2, LCD_YSTART + 20 * (i - index1 + 1) + 20 - 2);
        }
        LCD_DrawRectangle(LCD_XSTART, LCD_YSTART + 20 * (i - index1 + 1), LCD_XEND, LCD_YSTART + 20 * (i - index1 + 1) + 20);
        LCD_ShowNum(LCD_XSTART, LCD_YSTART + 20 * (i - index1 + 1), i, 1, 12);
        if (selects[i].text != NULL) {
            LCD_ShowString(LCD_XSTART + 12, LCD_YSTART + 20 * (i - index1 + 1), 120, 20, 12, selects[i].text);
        }
    }
}

// 1 select 1-3
// 2 select 4-6
// 3 ...
void disp_MenuPage(uint16_t page) {
	LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
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

    cur_index = 1;

    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    LCD_DrawRectangle(LCD_XSTART - 1, LCD_YSTART - 1, LCD_XEND + 1, LCD_YEND + 1);
    POINT_COLOR = BLUE;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 10, menu_title, 16, 208);
    disp_MenuPage(1);
	// LCD_DrawRectangle(LCD_XSTART-1, LCD_YSTART-1, LCD_XEND+1, LCD_YEND+1);
	// POINT_COLOR = BLUE;
    // Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, menu_title, 16, 208);
    // Show_Str_Mid(LCD_XSTART, LCD_YSTART + 40, "Press Key0 to return", 16, 208);
    // Show_Str_Mid(LCD_XSTART, LCD_YSTART + 60, "Press Key1 to GPS", 16, 208);
	//   Show_Str_Mid(LCD_XSTART, LCD_YSTART + 80, "Press Key_UP to T&H", 16, 208);
	// LCD_Draw_Circle(LCD_XSTART,LCD_YSTART,50);
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
            if (page > 3) {
                page = 1;
            }
            cur_index = (page - 1) * PAGE_INUM + 1;
            disp_MenuPage(page);
        default:
            break;
        }
    }
    /* do before exit */
    if (current_page == MENU) {
        current_page = MAIN_INTERFACE;
    }
    LCD_Clear(WHITE);
}

// 空界面 显示完一句话直接退出
void display_empty(void) {
    LCD_ShowString(60, 160, 120, 20, 12,(u8 *) empty_str);
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
