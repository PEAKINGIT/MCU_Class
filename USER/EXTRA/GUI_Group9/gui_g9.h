/**
 * @name 	gui_g9.h
 * @brief 	encoding in UTF-8
 * 			OOP Style gui lib
 */
#ifndef __GUI_G9_H
#define __GUI_G9_H

/*includes*/
#include "stm32f10x.h"
#include "lcd.h"

/* GUI Size, adjust to fit screen*/
#define G_WIDTH (240)
#define G_HEIGHT (320)

typedef uint8_t IE_JUDGE; // Interface Exit Judge
#define I_CONTINUE (0x00)
#define I_EXIT (0x01)

typedef uint8_t I_EXIT_STATE; // Interface state at Exit
#define I_EXIT_WELL (0x00)
#define I_LOAD_FAIL (0x01)
#define I_EXIT_FAIL (0x02)
#define I_JUDGE_FAIL (0x03)

/**
 * @brief interface struct
 * @property interface related functions
 */
typedef struct _interface_obj {
    void (*init)(struct _interface_obj *obj_ptr);
    void (*looping)(struct _interface_obj *obj_ptr);
    IE_JUDGE (*exit_judge)(struct _interface_obj *obj_ptr);
    void (*exit)(struct _interface_obj *obj_ptr);
    void *data_ptr;
} InterFaceObj;

/**
 * @brief load an Interface and display
 * @attention usage is shown below
 */
/*
InterFaceObj interface1;
...
int main() {
    LCD_Init();
    if(Load_InterFace(&gui1)!=I_EXIT_WELL){
        printf("param wrong!");
    }
}
*/
I_EXIT_STATE Load_InterFace(InterFaceObj *interFacePtr);

/**
 * @brief Draw Extend
 */
int32_t my_pow(int32_t x,int32_t n);

#endif
/*end of file*/
