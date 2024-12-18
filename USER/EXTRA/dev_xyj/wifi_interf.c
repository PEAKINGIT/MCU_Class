#include "wifi_interf.h"

//WIFI通讯页面
void WIFI_Gui_Load(void){
	u32 no_input_last = 0;
    u8 wifi_isok = WIFI_OK;
	u8 key;
	// LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
	LCD_Clear(WHITE);
    POINT_COLOR = RED;
    LCD_DrawRectangle(LCD_XSTART - 1, LCD_YSTART - 1, LCD_XEND + 1, LCD_YEND + 1);
	POINT_COLOR = BLACK;
	Show_Str_Mid(LCD_XSTART,LCD_YSTART+20,"WIFI TCP通信",16,208);
	LCD_DrawRectangle(LCD_XSTART+2, LCD_YSTART+38, LCD_XEND -2, LCD_YEND -2);
	while(1){
		key = KEY_Scan(0);
		if(key == KEY0_LONGP) break;
		if (USART3_RX_STA & 0X8000) {
            no_input_last = globalTick_Get();
        } else {
            if ((globalTick_Get() - no_input_last) >= 10000) {
                // 长时间无数据接收检查
                wifi_isok = WIFI_ConnectCheck();
                no_input_last = globalTick_Get();
            }
        }
		WIFI_RcvHandle(wifi_isok,WIFI_GUI_Cmd); // WIFI服务器数据接收处理

		// printf("Debug\r\n");
		delay_ms(500);
	}
	LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
	current_page = MENU;
}

//执行WIFI gui下的服务器指令执行
void WIFI_GUI_Cmd(u8 *cmd){
	printf("%s\r\n",cmd);
	LCD_ShowString(LCD_XSTART+6,LCD_YSTART+40,120,20,12,cmd);
}
