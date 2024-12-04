#include "picture_app.h"

//Picture��ʾӦ�ò����

//�Լ�ͼƬ�ļ���ʾ
//������Ϊ����ԭ������lcd.c/h

u8 res;
DIR picdir;          // ͼƬĿ¼
FILINFO picfileinfo; // �ļ���Ϣ
u8 *fn;              // ���ļ���
u8 *pname;           // ��·�����ļ���
u16 totPicNum;       // ͼƬ�ļ�����
u16 curIndex;        // ͼƬ��ǰ����
u16 temp;
u16 *picIndexTbl; // ͼƬ������

// �õ�path·����,Ŀ���ļ����ܸ���
// path:·��
// ����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path) {
    u8 res;
    u16 rval = 0;
    DIR tdir;          // ��ʱĿ¼
    FILINFO tfileinfo; // ��ʱ�ļ���Ϣ
    u8 *fn;
    res = f_opendir(&tdir, (const TCHAR *)path);           // ��Ŀ¼
    tfileinfo.lfsize = _MAX_LFN * 2 + 1;                   // ���ļ�����󳤶�
    tfileinfo.lfname = mymalloc(SRAMIN, tfileinfo.lfsize); // Ϊ���ļ������������ڴ�
    if (res == FR_OK && tfileinfo.lfname != NULL) {
        while (1) // ��ѯ�ܵ���Ч�ļ���
        {
            res = f_readdir(&tdir, &tfileinfo);                 // ��ȡĿ¼�µ�һ���ļ�
            if (res != FR_OK || tfileinfo.fname[0] == 0) break; // ������/��ĩβ��,�˳�
            fn = (u8 *)(*tfileinfo.lfname ? tfileinfo.lfname : tfileinfo.fname);
            res = f_typetell(fn);
            if ((res & 0XF0) == 0X50) // ȡ����λ,�����ǲ���ͼƬ�ļ�
            {
                rval++; // ��Ч�ļ�������1
            }
        }
    }
	myfree(SRAMIN, tfileinfo.lfname); // �ͷ��ڴ�
    return rval;
}


//ͼƬ��ʼ��&���
//��Ҫ���г�ʼ���ļ�ϵͳ!!!
//��������ʼ���е���
void Picture_Init_Check(void){
	while (f_opendir(&picdir, "0:/PICTURE")) {
		// ��ͼƬ�ļ���
        Show_Str(30, 170, 240, 16, "PICTURE�ļ��д���!", 16, 0);
        delay_ms(200);
        LCD_Fill(30, 170, 240, 186, WHITE); // �����ʾ
        delay_ms(200);
    }
    totPicNum = pic_get_tnum("0:/PICTURE"); // �õ�����Ч�ļ���
    while (totPicNum == 0) {
		// ͼƬ�ļ�Ϊ0
        Show_Str(30, 170, 240, 16, "û��ͼƬ�ļ�!", 16, 0);
        delay_ms(200);
        LCD_Fill(30, 170, 240, 186, WHITE); // �����ʾ
        delay_ms(200);
    }
    picfileinfo.lfsize = _MAX_LFN * 2 + 1;                     // ���ļ�����󳤶�
    picfileinfo.lfname = mymalloc(SRAMIN, picfileinfo.lfsize); // Ϊ���ļ������������ڴ�
    pname = mymalloc(SRAMIN, picfileinfo.lfsize);              // Ϊ��·�����ļ��������ڴ�
    picIndexTbl = mymalloc(SRAMIN, 2 * totPicNum);             // ����2*totPicNum���ֽڵ��ڴ�,���ڴ��ͼƬ����
    while (picfileinfo.lfname == NULL || pname == NULL || picIndexTbl == NULL) {
        // �ڴ�������
        Show_Str(30, 170, 240, 16, "�ڴ����ʧ��!", 16, 0);
        delay_ms(200);
        LCD_Fill(30, 170, 240, 186, WHITE); // �����ʾ
        delay_ms(200);
    }
    // ��¼���� ������ͼƬ·����¼����
    res = f_opendir(&picdir, "0:/PICTURE"); // ��Ŀ¼
    if (res == FR_OK) {
        curIndex = 0; // ��ǰ����Ϊ0
        while (1)     // ȫ����ѯһ��
        {
            temp = picdir.index;                                  // ��¼��ǰindex
            res = f_readdir(&picdir, &picfileinfo);               // ��ȡĿ¼�µ�һ���ļ�
            if (res != FR_OK || picfileinfo.fname[0] == 0) break; // ������/��ĩβ��,�˳�
            fn = (u8 *)(*picfileinfo.lfname ? picfileinfo.lfname : picfileinfo.fname);
            res = f_typetell(fn);
            if ((res & 0XF0) == 0X50) // ȡ����λ,�����ǲ���ͼƬ�ļ�
            {
                picIndexTbl[curIndex] = temp; // ��¼����
                curIndex++;
            }
        }
    }
}

void Draw_Picture_Init(void){
	piclib_init();                                         // ��ʼ����ͼ
    curIndex = 0;                                          // ��0��ʼ��ʾ
    res = f_opendir(&picdir, (const TCHAR *)"0:/PICTURE"); // ��Ŀ¼
}

PicDrawState Draw_Picture(u16 picIndex){
	//������ΪpicIndex��ͼ��
	if(picIndex<totPicNum){	
		//�����±�����
		curIndex = picIndex;
		dir_sdi(&picdir, picIndexTbl[picIndex]);              // �ı䵱ǰĿ¼����
		res = f_readdir(&picdir, &picfileinfo);               // ��ȡĿ¼�µ�һ���ļ�
		if (res != FR_OK || picfileinfo.fname[0] == 0) return PIC_ERR; // ������/��ĩβ��,�˳�
		fn = (u8 *)(*picfileinfo.lfname ? picfileinfo.lfname : picfileinfo.fname);
		strcpy((char *)pname, "0:/PICTURE/");    // ����·��(Ŀ¼)
		strcat((char *)pname, (const char *)fn); // ���ļ������ں���
		LCD_Clear(WHITE);
		ai_load_picfile(pname, 0, 0, lcddev.width, lcddev.height, 1); // ��ʾͼƬ
		Show_Str(2, 2, 240, 16, pname, 16, 1);                        // ��ʾͼƬ����
		return PIC_OK;
	} else {
		return PIC_ERR;
	}
}

//list all picture files in usart
void PicDebug_ListPics(void){
	u16 i ;
	for(i = 0;i<totPicNum;i++){
		dir_sdi(&picdir, picIndexTbl[i]);              // �ı䵱ǰĿ¼����
		res = f_readdir(&picdir, &picfileinfo);               // ��ȡĿ¼�µ�һ���ļ�
		if (res != FR_OK || picfileinfo.fname[0] == 0) {
			// ������/��ĩβ��
			printf("File ERROR\r\n");
		} else {
			fn = (u8 *)(*picfileinfo.lfname ? picfileinfo.lfname : picfileinfo.fname);
			strcpy((char *)pname, "0:/PICTURE/");    // ����·��(Ŀ¼)
			strcat((char *)pname, (const char *)fn); // ���ļ������ں���
			printf("%s\r\n",pname);	
		}
	}
}

//		��ͼƬԭʼ����
//        dir_sdi(&picdir, picindextbl[curindex]);              // �ı䵱ǰĿ¼����
//        res = f_readdir(&picdir, &picfileinfo);               // ��ȡĿ¼�µ�һ���ļ�
//        if (res != FR_OK || picfileinfo.fname[0] == 0) break; // ������/��ĩβ��,�˳�
//        fn = (u8 *)(*picfileinfo.lfname ? picfileinfo.lfname : picfileinfo.fname);
//        strcpy((char *)pname, "0:/PICTURE/");    // ����·��(Ŀ¼)
//        strcat((char *)pname, (const char *)fn); // ���ļ������ں���
//        LCD_Clear(WHITE);
//        ai_load_picfile(pname, 0, 0, lcddev.width, lcddev.height, 1); // ��ʾͼƬ
//        Show_Str(2, 2, 240, 16, pname, 16, 1);                        // ��ʾͼƬ����


//�ͷ�����ͼƬ�йص�������ڴ� ��������ͼƬ���ٵ��� һ���ò���
//��Picture_Init_Check()��Ӧ
void Picture_Free(void){
	myfree(SRAMIN, picfileinfo.lfname); // �ͷ��ڴ�
    myfree(SRAMIN, pname);              // �ͷ��ڴ�
    myfree(SRAMIN, picIndexTbl);        // �ͷ��ڴ�
}
