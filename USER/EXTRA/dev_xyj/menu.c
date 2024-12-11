#include "menu.h"

static uint8_t *menu_title = "Menu"; // ҳ�����
static Select selects[SELECT_NUM];   // ѡ�� �ṹ����

uint8_t *empty_str = "Nothing Here~"; // �ս����ַ�����ʾ
InterfaceState current_page = MENU;   // ��ǰ����״̬
// ���溯��ָ������
// ˳��ע��ͽ���ö�ٱ�����ͬ
void (*interface_functions[MAX_INTERFACE])(void) = {
    display_empty,           // �ս���
    Load_MainInterface,      // ������
    display_sub_interface_1, // ��ʪ��������ʾ
    gpsGui_Load,             // GPS����
    Load_calendarInterface,  // ����
    Menu_Load,               // �˵�
};

// Ϊselect���ض��±������ù���
void add_Select(uint16_t index, uint8_t *text, void (*select_event)(void)) {
    selects[index].select_event = select_event;
    selects[index].text = text;
}

// �˵���ʼ��
void Menu_Init(void) {
    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, menu_title, 16, 208);
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 40, "Press Key0 to return", 16, 208);
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 60, "Press Key1 to GPS", 16, 208);
}

void Menu_Load(void) {
    u8 key;

    Menu_Init();
    while (1) {
        key = KEY_Scan(0);

        if (key == KEY0_PRES) {
            current_page = MAIN_INTERFACE;
            break;
        }
        if (key == KEY1_PRES) {
            current_page = GPS_GUI;
            break;
        }
    }
    /* do before exit */
    LCD_Clear(WHITE);
}

// �ս��� ��ʾ��һ�仰ֱ���˳�
void display_empty(void) {
    LCD_ShowString(60, 160, 120, 20, 12, empty_str);
}
