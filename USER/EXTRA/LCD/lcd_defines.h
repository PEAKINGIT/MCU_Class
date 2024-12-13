#ifndef __LCD_DEFS_H
#define __LCD_DEFS_H

#include "lcd.h"

#define LCD_WIDTH (208)
#define LCD_HEIGHT (208)

#define LCD_XSTART (16)
#define LCD_YSTART (56)

#define LCD_XEND (LCD_XSTART+LCD_WIDTH-1)
#define LCD_YEND (LCD_YSTART+LCD_HEIGHT-1)

#define AREA_CLEAR (LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,BACK_COLOR))

#endif
