#include "other_interface.h"
#include "key.h"
#include "lcd.h"
#include "delay.h"
#include "main_interface.h"
#include "dht11.h"

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
					  display_sub_interface_1_init();  // ���÷ֽ���1�ĳ�ʼ������
        } else if (key == KEY1_PRES) {
            current_state = SUB_INTERFACE_2; // ����ֽ���2
					 display_sub_interface_2_init();  // ���÷ֽ���2�ĳ�ʼ������
        }
    } else if (current_state == SUB_INTERFACE_1) {
        // �ֽ���1�����߼�
        if (key == KEY0_PRES) {
            current_state = MAIN_INTERFACE; // ����������
					//interface_init();//�������ʼ��
        }
    } else if (current_state == SUB_INTERFACE_2) {
        // �ֽ���2�����߼�
        if (key == KEY1_PRES) {
            current_state = MAIN_INTERFACE; // ����������
					//interface_init();//�������ʼ��
        }
    }
}
// ��������ʾ����
void display_main_interface(void) {
    draw_mainInterface(); // ���û���������ĺ���
    delay_ms(100);
    LED1_Toggle;
}
// �ֽ���1��ʼ������
void display_sub_interface_1_init(void) {
    LCD_Clear(WHITE);  // ������������ʼ�����汳��
    // ������ʼ������������ƹ̶��ľ�̬����
    LCD_ShowString(10, 10, 200, 16, 16, "Temperature and Humidity");  // ��ʾ����
}
// �ֽ���1��ʾ��������ʪ����ʾ��
void display_sub_interface_1(void) {
   DHT11_Update_Data_If_Expired(); // �����Ը��� DHT11 ����

    // ��ӡ��ʪ�����ݣ�ÿ��һ��
    static uint32_t last_print_tick = 0;
    if (tick_expired(&last_print_tick, 1000)) {
        // ��ȡ��ʪ������
        u8 temperature = DHT11_Get_Temperature();
        u8 humidity = DHT11_Get_Humidity();

        // ����ʪ������ת��Ϊ�ַ���
        char temp_str[32], humi_str[32];
        sprintf(temp_str, "Temp: %d C", temperature); // �¶��ַ���
        sprintf(humi_str, "Humi: %d %%", humidity); // ʪ���ַ���

        // ��ʾ�¶Ⱥ�ʪ������
			  
        LCD_ShowString(10, 50, 200, 16, 16, temp_str); // ��ʾ�¶�
        LCD_ShowString(10, 70, 200, 16, 16, humi_str); // ��ʾʪ��
    }

    delay_ms(100);  // ��ʱ��ģ�⶯̬����
}
// �ֽ���2��ʼ������
void display_sub_interface_2_init(void) {
    LCD_Clear(WHITE);  // ������������ʼ�����汳��
    // ������ʼ������������ƹ̶��ľ�̬����
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 2");  // ��ʾ����
}

// �ֽ���2��ʾ����
void display_sub_interface_2(void) {
    
    delay_ms(100);  // ��ʱ��ģ�⶯̬����
}
// ����ģ���ʼ��
void interface_init(void) {
    current_state = MAIN_INTERFACE; // ���ó�ʼ����
    interface_functions[current_state](); // ��ʾ��ʼ����
}