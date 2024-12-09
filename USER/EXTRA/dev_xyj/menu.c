#include "menu.h"

uint8_t *empty_str = "Nothing Here~";
// 当前界面状态
InterfaceState current_page = MAIN_INTERFACE;
// 界面函数指针数组
// 顺序注意和界面枚举保持相同
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

//空界面 显示完一句话直接退出
void display_empty(void){
	LCD_ShowString(60,160,120,20,12,empty_str);
}

// 按键事件处理函数
void handle_key_event(void) {
    u8 key = KEY_Scan(0); // 扫描按键状态

    if (current_page == MAIN_INTERFACE) {
        // 主界面按键逻辑
        if (key == KEY0_PRES) {
            current_page = SUB_INTERFACE_1; // 进入分界面1
            display_sub_interface_1_init();  // 调用分界面1的初始化函数
        } else if (key == KEY1_PRES) {
            current_page = SUB_INTERFACE_2; // 进入分界面2
            display_sub_interface_2_init();  // 调用分界面2的初始化函数
        }
    } else if (current_page == SUB_INTERFACE_1) {
        // 分界面1按键逻辑
        if (key == KEY0_PRES) {
			LCD_Clear(WHITE);
			draw_clock();
            current_page = MAIN_INTERFACE; // 返回主界面
                                            // interface_init();//主界面初始化
        }
    } else if (current_page == SUB_INTERFACE_2) {
        // 分界面2按键逻辑
        if (key == KEY1_PRES) {
			LCD_Clear(WHITE);
			draw_clock();
            current_page = MAIN_INTERFACE; // 返回主界面
                                            // interface_init();//主界面初始化
        }
    }
}

// 分界面2初始化函数
void display_sub_interface_2_init(void) {
    LCD_Clear(WHITE); // 清屏操作，初始化界面背景
    // 其他初始化操作，如绘制固定的静态内容
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 2"); // 显示标题
	gpsGui_Init(&gpsInterface);
}

// 分界面2显示函数
void display_sub_interface_2(void) {
	gpsGui_Refresh(&gpsInterface);
    delay_ms(100); // 延时，模拟动态内容
}
// 界面模块初始化
void interface_init(void) {
    current_page = MAIN_INTERFACE;       // 设置初始界面
    interface_functions[current_page](); // 显示初始界面
}
