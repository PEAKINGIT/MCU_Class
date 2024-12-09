/*
 �˵�ҳ��
*/
#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include "lcd.h"
#include "gui_g9.h"

#include "main_interface.h"
#include "gps_interface.h"
#include "dht11_interface.h"

//interfaces
// ����״̬ö��
typedef enum {
	EMPTY_INTREFACE = 0,	//�ս���
    MAIN_INTERFACE,  // ������
    SUB_INTERFACE_1, // �ֽ���1
    SUB_INTERFACE_2, // �ֽ���2
	MENU,
    MAX_INTERFACE,    // ��������
} InterfaceState;

typedef struct _select_opt{
	uint8_t *text;
	void (*select_event)(void);
}Select;

typedef struct _menu_data{
	uint8_t *title;		//ҳ�����
	Select *selects;	//ѡ��
}MenuData;

extern InterfaceState current_page; // ��ǰ����״̬
extern InterFaceObj Menu_Obj;

void interface_init(void);           // ����ģ���ʼ��
// �������溯��ָ������
extern void (*interface_functions[MAX_INTERFACE])(void); // ʹ�� extern ����

void display_sub_interface_2(void);
void handle_key_event(void); // �����¼�������
void display_sub_interface_2_init(void); //�ֽ���2��ʼ������
void display_empty(void);

#endif
