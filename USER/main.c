#include "stm32f10x.h"

#include "led.h"
#include "delay.h"
#include "key.h"
#include "main_interface.h"
#include "lcd_display.h"

#include "sys.h"
#include "usart.h"
#include "lcd.h"	 
#include "rtc.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "piclib.h"	

#include "string.h"		
#include "math.h"

/**
 * @brief state for functions [called in main()] & [defined in main.c]
 */
void Delay(u32 count);
void SD_Init_Check(void);
void Font_Init_Check(void);

//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ	
	u8 *fn;	 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); 	//��Ŀ¼
  	tfileinfo.lfsize=_MAX_LFN*2+1;				//���ļ�����󳤶�
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);//Ϊ���ļ������������ڴ�
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	} 
	return rval;
}

int main(void)
 {	 
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO picfileinfo;//�ļ���Ϣ
	u8 *fn;   			//���ļ���
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	u8 pause=0;			//��ͣ���
	u16 temp;
	u16 *picindextbl;	//ͼƬ������
		  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 //��ʱ������ʼ��
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	led_init();			    //LED�˿ڳ�ʼ��
	KEY_Init();
	LCD_Init();			 	//��ʼ��LCD	
	RTC_Init();	  			//RTC��ʼ��
	SD_Init_Check();
	W25QXX_Init();				//��ʼ��W25Q128 �ⲿflash
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	Font_Init_Check();			//����ֿ��ʼ��

	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(30,170,240,16,"PICTURE�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	} 
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)	{
		//ͼƬ�ļ�Ϊ0		    
		Show_Str(30,170,240,16,"û��ͼƬ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//����2*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL){
		//�ڴ�������	    
		Show_Str(30,170,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	
	//��¼����
    res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.index;								//��¼��ǰindex
	        res=f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picindextbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}
	piclib_init();										//��ʼ����ͼ	   	   
	curindex=0;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼	
	
	while(res==FR_OK)
	{
		dir_sdi(&picdir,picindextbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
     	fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
		strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
		LCD_Clear(WHITE);
		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ    
		Show_Str(2,2,240,16,pname,16,1); 				//��ʾͼƬ����
		draw_clock();
		while(1) {
			draw_mainInterface();
		}
		res=0;
	}
	myfree(SRAMIN,picfileinfo.lfname);	//�ͷ��ڴ�			    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picindextbl);			//�ͷ��ڴ�		
 }

void Delay(u32 count) {
    u32 i = 0;
    for (; i < count; i++);
}

void SD_Init_Check(void){
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	while(SD_Init()) 		//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"SD Card Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
}

void Font_Init_Check(void){
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	while(font_init()) 		//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
}
