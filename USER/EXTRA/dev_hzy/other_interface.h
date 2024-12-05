#ifndef __INTERFACE_H
#define __INTERFACE_H

// ����״̬ö��
typedef enum {
    MAIN_INTERFACE,  // ������
    SUB_INTERFACE_1, // �ֽ���1
    SUB_INTERFACE_2, // �ֽ���2
    MAX_INTERFACE    // ��������
} InterfaceState;

// ���������ر����ͽӿ�
extern InterfaceState current_state; // ��ǰ����״̬
void interface_init(void);           // ����ģ���ʼ��

// �������溯��ָ������
extern void (*interface_functions[MAX_INTERFACE])(void); // ʹ�� extern ����

void display_main_interface(void);
void display_sub_interface_1(void);
void display_sub_interface_2(void);
void handle_key_event(void); // �����¼�������
void display_sub_interface_1_init(void); //�ֽ���1��ʼ������
void display_sub_interface_2_init(void); //�ֽ���2��ʼ������
#endif // __OTHER_INTERFACE_H
