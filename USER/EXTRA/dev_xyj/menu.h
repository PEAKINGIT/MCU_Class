/*
 �˵�ҳ��
*/
#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include "lcd.h"
#include "lcd_defines.h"

#include "main_interface.h"
#include "gps_interface.h"
#include "dht11_interface.h"
#include "calendar_interface.h"
#include "wifi_interf.h"
#include "rtc.h"
#include "wifi_app.h"
#include "beep.h"
#include "key.h"

//interfaces
// ����״̬ö��
typedef enum {
	EMPTY_INTREFACE = 0,	//�ս���
    MAIN_INTERFACE,  // ������
    DHT_INTREFACE, // �ֽ���1
    GPS_GUI, // �ֽ���2
	CALENDAR,
	MENU,
	WIFI_GUI,
    MAX_INTERFACE,    // ��������
} InterfaceState;

typedef struct _select_opt{
	uint8_t *text;
	uint8_t type;	// ѡ������ �����ͻ���ҳ���л�
	void (*select_event)(void);
}Select;

#define EMPTY	(0x00)	//Ĭ�� �޹���ѡ��
#define INTERF_S (0x01)	//ҳ���л�
#define FUNC (0x02)		//�˵�����С����

#define PAGE_INUM	(6)	//ҳ����Ŀ��
#define PAGE_NUM	(2)	//page��
#define SELECT_NUM (12)	// 
#define M_PAGE1 (1)
#define M_PAGE2 (2)
#define M_PAGE3 (3)

#define SELECT_H (22)

extern InterfaceState current_page; // ��ǰ����״̬
extern void (*interface_functions[MAX_INTERFACE])(void); // �������溯��ָ������

void Menu_Init(void);
void Menu_Load(void);
void add_Select(uint16_t index, uint8_t *text, void (*select_event)(void),uint8_t type) ;
void disp_selects(u16 index1, u16 index2) ;
void disp_MenuPage(uint16_t page) ;


void display_empty(void);

// ѡ���
void s2Main(void);
void s2DHT(void);
void s2GPS(void);
void s2Calendar(void);
void s2WIFI(void);

void rstWIFI(void);
void adjTime(void);
void FuncClear(void);
void CallingHelp(void);
void setBackCount(void);

#endif
