#ifndef __WIFI_INTERF_H
#define __WIFI_INTERF_H

#include "stm32f10x.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "wifi_app.h"
#include "menu.h"
#include "key.h"

void WIFI_Gui_Load(void);
void WIFI_GUI_Cmd(u8 *cmd);

#endif
