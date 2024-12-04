#include "picture_app.h"

//Picture显示应用层代码

//以及图片文件显示
//驱动层为正点原子例程lcd.c/h

u8 res;
DIR picdir;          // 图片目录
FILINFO picfileinfo; // 文件信息
u8 *fn;              // 长文件名
u8 *pname;           // 带路径的文件名
u16 totPicNum;       // 图片文件总数
u16 curIndex;        // 图片当前索引
u16 temp;
u16 *picIndexTbl; // 图片索引表

// 得到path路径下,目标文件的总个数
// path:路径
// 返回值:总有效文件数
u16 pic_get_tnum(u8 *path) {
    u8 res;
    u16 rval = 0;
    DIR tdir;          // 临时目录
    FILINFO tfileinfo; // 临时文件信息
    u8 *fn;
    res = f_opendir(&tdir, (const TCHAR *)path);           // 打开目录
    tfileinfo.lfsize = _MAX_LFN * 2 + 1;                   // 长文件名最大长度
    tfileinfo.lfname = mymalloc(SRAMIN, tfileinfo.lfsize); // 为长文件缓存区分配内存
    if (res == FR_OK && tfileinfo.lfname != NULL) {
        while (1) // 查询总的有效文件数
        {
            res = f_readdir(&tdir, &tfileinfo);                 // 读取目录下的一个文件
            if (res != FR_OK || tfileinfo.fname[0] == 0) break; // 错误了/到末尾了,退出
            fn = (u8 *)(*tfileinfo.lfname ? tfileinfo.lfname : tfileinfo.fname);
            res = f_typetell(fn);
            if ((res & 0XF0) == 0X50) // 取高四位,看看是不是图片文件
            {
                rval++; // 有效文件数增加1
            }
        }
    }
	myfree(SRAMIN, tfileinfo.lfname); // 释放内存
    return rval;
}


//图片初始化&检查
//需要先行初始化文件系统!!!
//主函数初始化中调用
void Picture_Init_Check(void){
	while (f_opendir(&picdir, "0:/PICTURE")) {
		// 打开图片文件夹
        Show_Str(30, 170, 240, 16, "PICTURE文件夹错误!", 16, 0);
        delay_ms(200);
        LCD_Fill(30, 170, 240, 186, WHITE); // 清除显示
        delay_ms(200);
    }
    totPicNum = pic_get_tnum("0:/PICTURE"); // 得到总有效文件数
    while (totPicNum == 0) {
		// 图片文件为0
        Show_Str(30, 170, 240, 16, "没有图片文件!", 16, 0);
        delay_ms(200);
        LCD_Fill(30, 170, 240, 186, WHITE); // 清除显示
        delay_ms(200);
    }
    picfileinfo.lfsize = _MAX_LFN * 2 + 1;                     // 长文件名最大长度
    picfileinfo.lfname = mymalloc(SRAMIN, picfileinfo.lfsize); // 为长文件缓存区分配内存
    pname = mymalloc(SRAMIN, picfileinfo.lfsize);              // 为带路径的文件名分配内存
    picIndexTbl = mymalloc(SRAMIN, 2 * totPicNum);             // 申请2*totPicNum个字节的内存,用于存放图片索引
    while (picfileinfo.lfname == NULL || pname == NULL || picIndexTbl == NULL) {
        // 内存分配出错
        Show_Str(30, 170, 240, 16, "内存分配失败!", 16, 0);
        delay_ms(200);
        LCD_Fill(30, 170, 240, 186, WHITE); // 清除显示
        delay_ms(200);
    }
    // 记录索引 把所有图片路径记录下来
    res = f_opendir(&picdir, "0:/PICTURE"); // 打开目录
    if (res == FR_OK) {
        curIndex = 0; // 当前索引为0
        while (1)     // 全部查询一遍
        {
            temp = picdir.index;                                  // 记录当前index
            res = f_readdir(&picdir, &picfileinfo);               // 读取目录下的一个文件
            if (res != FR_OK || picfileinfo.fname[0] == 0) break; // 错误了/到末尾了,退出
            fn = (u8 *)(*picfileinfo.lfname ? picfileinfo.lfname : picfileinfo.fname);
            res = f_typetell(fn);
            if ((res & 0XF0) == 0X50) // 取高四位,看看是不是图片文件
            {
                picIndexTbl[curIndex] = temp; // 记录索引
                curIndex++;
            }
        }
    }
}

void Draw_Picture_Init(void){
	piclib_init();                                         // 初始化画图
    curIndex = 0;                                          // 从0开始显示
    res = f_opendir(&picdir, (const TCHAR *)"0:/PICTURE"); // 打开目录
}

PicDrawState Draw_Picture(u16 picIndex){
	//画索引为picIndex的图像
	if(picIndex<totPicNum){	
		//索引下标正常
		curIndex = picIndex;
		dir_sdi(&picdir, picIndexTbl[picIndex]);              // 改变当前目录索引
		res = f_readdir(&picdir, &picfileinfo);               // 读取目录下的一个文件
		if (res != FR_OK || picfileinfo.fname[0] == 0) return PIC_ERR; // 错误了/到末尾了,退出
		fn = (u8 *)(*picfileinfo.lfname ? picfileinfo.lfname : picfileinfo.fname);
		strcpy((char *)pname, "0:/PICTURE/");    // 复制路径(目录)
		strcat((char *)pname, (const char *)fn); // 将文件名接在后面
		LCD_Clear(WHITE);
		ai_load_picfile(pname, 0, 0, lcddev.width, lcddev.height, 1); // 显示图片
		Show_Str(2, 2, 240, 16, pname, 16, 1);                        // 显示图片名字
		return PIC_OK;
	} else {
		return PIC_ERR;
	}
}

//list all picture files in usart
void PicDebug_ListPics(void){
	u16 i ;
	for(i = 0;i<totPicNum;i++){
		dir_sdi(&picdir, picIndexTbl[i]);              // 改变当前目录索引
		res = f_readdir(&picdir, &picfileinfo);               // 读取目录下的一个文件
		if (res != FR_OK || picfileinfo.fname[0] == 0) {
			// 错误了/到末尾了
			printf("File ERROR\r\n");
		} else {
			fn = (u8 *)(*picfileinfo.lfname ? picfileinfo.lfname : picfileinfo.fname);
			strcpy((char *)pname, "0:/PICTURE/");    // 复制路径(目录)
			strcat((char *)pname, (const char *)fn); // 将文件名接在后面
			printf("%s\r\n",pname);	
		}
	}
}

//		画图片原始代码
//        dir_sdi(&picdir, picindextbl[curindex]);              // 改变当前目录索引
//        res = f_readdir(&picdir, &picfileinfo);               // 读取目录下的一个文件
//        if (res != FR_OK || picfileinfo.fname[0] == 0) break; // 错误了/到末尾了,退出
//        fn = (u8 *)(*picfileinfo.lfname ? picfileinfo.lfname : picfileinfo.fname);
//        strcpy((char *)pname, "0:/PICTURE/");    // 复制路径(目录)
//        strcat((char *)pname, (const char *)fn); // 将文件名接在后面
//        LCD_Clear(WHITE);
//        ai_load_picfile(pname, 0, 0, lcddev.width, lcddev.height, 1); // 显示图片
//        Show_Str(2, 2, 240, 16, pname, 16, 1);                        // 显示图片名字


//释放所有图片有关的申请的内存 后续不用图片了再调用 一般用不到
//和Picture_Init_Check()对应
void Picture_Free(void){
	myfree(SRAMIN, picfileinfo.lfname); // 释放内存
    myfree(SRAMIN, pname);              // 释放内存
    myfree(SRAMIN, picIndexTbl);        // 释放内存
}
