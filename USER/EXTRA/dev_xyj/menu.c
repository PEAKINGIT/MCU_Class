#include "menu.h"

static uint8_t *title = "Menu";		//ҳ����� 
static Select selects[SELECT_NUM];	//ѡ�� �ṹ����

uint8_t *empty_str = "Nothing Here~";	//�ս����ַ�����ʾ
InterfaceState current_page = MAIN_INTERFACE;	// ��ǰ����״̬
// ���溯��ָ������
// ˳��ע��ͽ���ö�ٱ�����ͬ
void (*interface_functions[MAX_INTERFACE])(void) = {
	display_empty,					//�ս���
    Load_MainInterface,				//������
    display_sub_interface_1,		//��ʪ��������ʾ
    gpsGui_Load,					//GPS����
	Menu_Load,						//�˵�
};

//Ϊselect���ض��±����ù���
void add_Select(uint16_t index,uint8_t *text,void (*select_event)(void)){
	selects[index].select_event = select_event;
	selects[index].text = text;
}

//�˵���ʼ��
void Menu_Init(void){
	
}

void Menu_Load(void){
	u8 key;
	
	Menu_Init();
	while(1){
		key = KEY_Scan(0);

		if(0) break;
	}
	/* do before exit */
	LCD_Clear(WHITE);
	current_page = MAIN_INTERFACE;
}


//�ս��� ��ʾ��һ�仰ֱ���˳�
void display_empty(void){
	LCD_ShowString(60,160,120,20,12,empty_str);
}
