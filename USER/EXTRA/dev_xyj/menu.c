#include "menu.h"

static uint8_t *title = "Menu";		//页面标题 
static Select selects[SELECT_NUM];	//选项 结构数组

uint8_t *empty_str = "Nothing Here~";	//空界面字符串显示
InterfaceState current_page = MAIN_INTERFACE;	// 当前界面状态
// 界面函数指针数组
// 顺序注意和界面枚举保持相同
void (*interface_functions[MAX_INTERFACE])(void) = {
	display_empty,					//空界面
    Load_MainInterface,				//主界面
    display_sub_interface_1,		//温湿度曲线显示
    gpsGui_Load,					//GPS界面
	Menu_Load,						//菜单
};

//为select的特定下标设置功能
void add_Select(uint16_t index,uint8_t *text,void (*select_event)(void)){
	selects[index].select_event = select_event;
	selects[index].text = text;
}

//菜单初始化
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


//空界面 显示完一句话直接退出
void display_empty(void){
	LCD_ShowString(60,160,120,20,12,empty_str);
}
