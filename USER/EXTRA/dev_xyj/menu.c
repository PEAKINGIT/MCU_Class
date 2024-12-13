#include "menu.h"

static uint8_t *menu_title = "Menu"; // ҳ�����
static Select selects[SELECT_NUM];   // ѡ�� �ṹ����
static uint16_t cur_index = 0;       // ��ǰѡ���±�
static uint16_t effect_index_num = 0;

const uint8_t *empty_str = "Nothing Here~"; // �ս����ַ�����ʾ
InterfaceState current_page = MENU;         // ��ǰ����״̬
// ���溯��ָ������
// ˳��ע��ͽ���ö�ٱ�����ͬ
void (*interface_functions[MAX_INTERFACE])(void) = {
    display_empty,          // �ս���
    Load_MainInterface,     // ������
    DHT_interface,          // ��ʪ��������ʾ
    gpsGui_Load,            // GPS����
    Load_calendarInterface, // ����
    Menu_Load,              // �˵�
};

// Ϊselect���ض��±������ù���
void add_Select(uint16_t index, uint8_t *text, void (*select_event)(void), uint8_t type) {
    selects[index].select_event = select_event;
    selects[index].text = text;
    selects[index].type = type;
}

// ��ʾindex1��index2֮���ѡ��
void disp_selects(u16 index1, u16 index2) {
    for (u16 i = index1; i <= index2; i++) {
        if (i == cur_index) {
            // ѡ�и���
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

// �˵���ʼ��
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
}

void Menu_Load(void) {
    u8 key;
    u8 page = 1;       // ��ǰҳ�� ѡ��Ų��� use���ҳ��
    u8 break_flag = 0; // ����ѭ����־λ

    Menu_Init();
    while (1) {
        key = KEY_Scan(0);
        switch (key) {
        case WKUP_PRES:
            // confirm select
            if (selects[cur_index].type != EMPTY) {
                // ���Ͳ�Ϊ��
                selects[cur_index].select_event();
                // ���Ϊ�����л���ѡ�� �˳�ѭ��
                if (selects[cur_index].type == INTERF_S) break_flag = 1;
            }
            break;
        case KEY0_PRES:
            // change cur index
            cur_index++;
            if (cur_index > PAGE_INUM * page) {
                // ��ҳ��ص�һ��
                cur_index = (page - 1) * PAGE_INUM + 1;
            }
            disp_MenuPage(page);
            // ҳ��ˢ��
            break;
        case KEY0_LONGP:
            // ����KEY0 ��ҳ
            page++;
            if (page > 3) {
                page = 1;
            }
            cur_index = (page - 1) * PAGE_INUM + 1;
            disp_MenuPage(page);
        default:
            break;
        }

        // �л������¼� break loop
        if (break_flag == 1) break;
    }
    /* do before exit */
    if (current_page == MENU) {
        current_page = MAIN_INTERFACE;
    }
    LCD_Clear(WHITE);
}

// �ս��� ��ʾ��һ�仰ֱ���˳�
void display_empty(void) {
    LCD_ShowString(60, 160, 120, 20, 12,(u8 *) empty_str);
}

/**
 * @brief ѡ���
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
