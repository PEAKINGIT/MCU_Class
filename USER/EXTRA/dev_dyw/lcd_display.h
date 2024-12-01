#ifndef __LCD_DISPLAY
#define	__LCD_DISPLAY

#include "stm32f10x.h"
#include "lcd.h"
#include "ff.h"
#include "malloc.h"
#include "text.h" 
#include "piclib.h"
#include "delay.h"
#include "string.h"

// ��Ļ��������
#define CENTER_X 120
#define CENTER_Y 160

typedef enum{
	PIC_OK = 0,
	PIC_ERR = 1
}PicDrawState;	//��ͼ״̬ �ɻ�ͼƬ����Draw_Picture()����

u16 pic_get_tnum(u8 *path);
void Picture_Init_Check(void);
void Draw_Picture_Init(void);
PicDrawState Draw_Picture(u16 picIndex);
void Picture_Free(void);

#endif
