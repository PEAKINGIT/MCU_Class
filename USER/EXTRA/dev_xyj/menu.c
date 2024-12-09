#include "menu.h"

uint8_t *empty_str = "Nothing Here~";
// ��ǰ����״̬
InterfaceState current_page = MAIN_INTERFACE;
// ���溯��ָ������
// ˳��ע��ͽ���ö�ٱ�����ͬ
void (*interface_functions[MAX_INTERFACE])(void) = {
	display_empty,
    Load_MainInterface,
    display_sub_interface_1,
    display_sub_interface_2,
	NULL,
};

InterFaceObj Menu_Obj;

	

void Menu_Init(struct _interface_obj *obj_ptr){
	
}

void Menu_Loop(struct _interface_obj *obj_ptr){
	
}

void Menu_ExitJudge(struct _interface_obj *obj_ptr){

}

void Menu_Exit(struct _interface_obj *obj_ptr){
	
}

//�ս��� ��ʾ��һ�仰ֱ���˳�
void display_empty(void){
	LCD_ShowString(60,160,120,20,12,empty_str);
}

// �����¼�������
void handle_key_event(void) {
    u8 key = KEY_Scan(0); // ɨ�谴��״̬

    if (current_page == MAIN_INTERFACE) {
        // �����水���߼�
        if (key == KEY0_PRES) {
            current_page = SUB_INTERFACE_1; // ����ֽ���1
            display_sub_interface_1_init();  // ���÷ֽ���1�ĳ�ʼ������
        } else if (key == KEY1_PRES) {
            current_page = SUB_INTERFACE_2; // ����ֽ���2
            display_sub_interface_2_init();  // ���÷ֽ���2�ĳ�ʼ������
        }
    } else if (current_page == SUB_INTERFACE_1) {
        // �ֽ���1�����߼�
        if (key == KEY0_PRES) {
			LCD_Clear(WHITE);
			draw_clock();
            current_page = MAIN_INTERFACE; // ����������
                                            // interface_init();//�������ʼ��
        }
    } else if (current_page == SUB_INTERFACE_2) {
        // �ֽ���2�����߼�
        if (key == KEY1_PRES) {
			LCD_Clear(WHITE);
			draw_clock();
            current_page = MAIN_INTERFACE; // ����������
                                            // interface_init();//�������ʼ��
        }
    }
}

// �ֽ���2��ʼ������
void display_sub_interface_2_init(void) {
    LCD_Clear(WHITE); // ������������ʼ�����汳��
    // ������ʼ������������ƹ̶��ľ�̬����
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 2"); // ��ʾ����
	gpsGui_Init(&gpsInterface);
}

// �ֽ���2��ʾ����
void display_sub_interface_2(void) {
	gpsGui_Refresh(&gpsInterface);
    delay_ms(100); // ��ʱ��ģ�⶯̬����
}
// ����ģ���ʼ��
void interface_init(void) {
    current_page = MAIN_INTERFACE;       // ���ó�ʼ����
    interface_functions[current_page](); // ��ʾ��ʼ����
}
