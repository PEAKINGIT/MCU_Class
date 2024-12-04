#include "other_interface.h"
#include "key.h"
#include "lcd.h"
#include "delay.h"
#include "main_interface.h"

// ��ǰ����״̬
extern InterfaceState current_state;
// ��ǰ����״̬
InterfaceState current_state = MAIN_INTERFACE;

// ���溯��ָ������
void (*interface_functions[MAX_INTERFACE])(void) = {
    display_main_interface,
    display_sub_interface_1,
    display_sub_interface_2
};

// �����¼�������
void handle_key_event(void) {
    u8 key = KEY_Scan(0); // ɨ�谴��״̬

    if (current_state == MAIN_INTERFACE) {
        // �����水���߼�
        if (key == KEY0_PRES) {
            current_state = SUB_INTERFACE_1; // ����ֽ���1
        } else if (key == KEY1_PRES) {
            current_state = SUB_INTERFACE_2; // ����ֽ���2
        }
    } else if (current_state == SUB_INTERFACE_1) {
        // �ֽ���1�����߼�
        if (key == KEY0_PRES) {
            current_state = MAIN_INTERFACE; // ����������
        }
    } else if (current_state == SUB_INTERFACE_2) {
        // �ֽ���2�����߼�
        if (key == KEY1_PRES) {
            current_state = MAIN_INTERFACE; // ����������
        }
    }
}
// ��������ʾ����
void display_main_interface(void) {
    draw_mainInterface(); // ���û���������ĺ���
    delay_ms(100);
    LED1_Toggle;
}
// �ֽ���1��ʾ����
void display_sub_interface_1(void) {
    LCD_Clear(WHITE);
    // �� (10, 10) λ�ã���ʾ���200���߶�16�������С16���ַ��� "This is Sub Interface 1"
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 1");
    delay_ms(100);  // ��ʱ��ģ�⶯̬����
}

// �ֽ���2��ʾ����
void display_sub_interface_2(void) {
    LCD_Clear(WHITE);
    // �� (10, 10) λ�ã���ʾ���200���߶�16�������С16���ַ��� "This is Sub Interface 2"
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 2");
    delay_ms(100);  // ��ʱ��ģ�⶯̬����
}
// ����ģ���ʼ��
void interface_init(void) {
    current_state = MAIN_INTERFACE; // ���ó�ʼ����
    interface_functions[current_state](); // ��ʾ��ʼ����
}