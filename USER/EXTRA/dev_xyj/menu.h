/*
 �˵�ҳ��
*/
#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include "lcd.h"

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

#define SELECT_NUM (MAX_INTERFACE+1)	//���ҳ���� ������ѡ��

extern InterfaceState current_page; // ��ǰ����״̬
extern void (*interface_functions[MAX_INTERFACE])(void); // �������溯��ָ������

void Menu_Init(void);
void Menu_Load(void);

void display_empty(void);

#endif
