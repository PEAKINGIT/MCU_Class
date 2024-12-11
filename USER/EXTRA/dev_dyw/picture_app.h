#ifndef __PICTURE_APP_H
#define	__PICTURE_APP_H

#include "stm32f10x.h"
#include "lcd.h"
#include "ff.h"
#include "malloc.h"
#include "text.h" 
#include "piclib.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"

typedef enum{
	PIC_OK = 0,
	PIC_ERR = 1
}PicDrawState;	//»æÍ¼×´Ì¬ ÓÉ»æÍ¼Æ¬º¯ÊýDraw_Picture()·µ»Ø

u16 pic_get_tnum(u8 *path);
void Picture_Init_Check(void);
void Draw_Picture_Init(void);
PicDrawState Draw_Picture(u16 picIndex);
void PicDebug_ListPics(void);

void Picture_Free(void);

#endif
