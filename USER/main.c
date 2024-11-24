#include "stm32f10x.h"
#include "usart.h"
#include "led.h"

void Delay(u32 count);

// HZY
#include "key.h" //2024/11/10 hzy
//

int main(void) {

    uart_init(115200);
    SysTick_Config(SystemCoreClock / 1000); // ��ʼ��SysTickÿ1ms����һ��

    // HZY
    KEY_Init(); // 2024/11/10 hzy
    //

    while (1) {
    }
}

void Delay(u32 count) {
    u32 i = 0;
    for (; i < count; i++);
}
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	
#include "usmart.h"	 
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
 
// ��Ļ��������
#define CENTER_X 120
#define CENTER_Y 160

// ָ�볤��
#define HOUR_HAND_LENGTH 50
#define MINUTE_HAND_LENGTH 70

// ���Ƕ�ת��Ϊ����
#define DEG_TO_RAD(angle)((angle) * 3.1415 / 180.0)


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

// ����ָ���յ�����
void CalculatePointerCoordinates(uint16_t centerX, uint16_t centerY, float angle, uint16_t length, uint16_t *x, uint16_t *y) {
    *x = centerX + length * cos(DEG_TO_RAD(angle - 90));
    *y = centerY + length * sin(DEG_TO_RAD(angle - 90));
}

// ���Ʊ���
void draw_clock(uint16_t hour, uint16_t minute) {
    uint16_t hour_x, hour_y, minute_x, minute_y;
    
    // ����ʱ��ͷ���ĽǶ�
    float angle_hour = (hour % 12 + minute / 60.0) * 30.0;
    float angle_minute = minute * 6.0;
    
    // ����ʱ��ͷ����յ�����
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_hour, HOUR_HAND_LENGTH, &hour_x, &hour_y);
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_minute, MINUTE_HAND_LENGTH, &minute_x, &minute_y);

    // ����ʱ��
	POINT_COLOR=RED; 
    LCD_DrawLine(CENTER_X, CENTER_Y, hour_x, hour_y);
    
    // ���Ʒ���
	POINT_COLOR=BLUE;
    LCD_DrawLine(CENTER_X, CENTER_Y, minute_x, minute_y);
}
// ��������
void earse_clock(uint16_t hour, uint16_t minute) {
	uint16_t hour_x, hour_y, minute_x, minute_y;
    
    // ����ʱ��ͷ���ĽǶ�
    float angle_hour = (hour % 12 + minute / 60.0) * 30.0;
    float angle_minute = minute * 6.0;
    
    // ����ʱ��ͷ����յ�����
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_hour, HOUR_HAND_LENGTH, &hour_x, &hour_y);
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_minute, MINUTE_HAND_LENGTH, &minute_x, &minute_y);
	
	POINT_COLOR=WHITE;
    // ����ʱ��
	LCD_DrawLine(CENTER_X, CENTER_Y, hour_x, hour_y);
    
    // ���Ʒ���
	LCD_DrawLine(CENTER_X, CENTER_Y, minute_x, minute_y);
}


int main(void)
 {	 
 	u8 t=0;
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
	
	u16 hour;
	u16 minute;
	u16 former_hour;
	u16 former_minute;
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			    //LED�˿ڳ�ʼ��
	LCD_Init();			 	//��ʼ��LCD
	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
	RTC_Init();	  			//RTC��ʼ��
	SD_Init();	
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	while(font_init()) 		//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(30,170,240,16,"PICTURE�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	} 
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0		
 	{	    
		Show_Str(30,170,240,16,"û��ͼƬ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//����2*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//�ڴ�������
 	{	    
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
		draw_clock(calendar.hour,calendar.min);
		while(1)
		{
			former_hour=hour;
			former_minute=minute;
			hour=calendar.hour;
			minute=calendar.min;
			key=KEY_Scan(0);		//ɨ�谴��
			if(t!=calendar.sec)
			{
				t=calendar.sec;
				//��ʾʱ��
				POINT_COLOR=BLUE;//��������Ϊ��ɫ	 
				switch(calendar.week)
				{
					case 0:
						LCD_ShowString(145,280,200,16,16,"Sunday   ");
						break;
					case 1:
						LCD_ShowString(145,280,200,16,16,"Monday   ");
						break;
					case 2:
						LCD_ShowString(145,280,200,16,16,"Tuesday  ");
						break;
					case 3:
						LCD_ShowString(145,280,200,16,16,"Wednesday");
						break;
					case 4:
						LCD_ShowString(145,280,200,16,16,"Thursday ");
						break;
					case 5:
						LCD_ShowString(145,280,200,16,16,"Friday   ");
						break;
					case 6:
						LCD_ShowString(145,280,200,16,16,"Saturday ");
						break;  
				}
				LCD_ShowString(60,280,200,16,16,"    -  -  ");
				LCD_ShowNum(60,280,calendar.w_year,4,16);									  
				LCD_ShowNum(100,280,calendar.w_month,2,16);									  
				LCD_ShowNum(124,280,calendar.w_date,2,16);
				LCD_ShowString(60,300,200,16,16,"  :  :  ");
				LCD_ShowNum(60,300,calendar.hour,2,16);									  
				LCD_ShowNum(84,300,calendar.min,2,16);									  
				LCD_ShowNum(108,300,calendar.sec,2,16);
				LED0=!LED0;
			}
			if(hour!=former_hour||minute!=former_minute)
			{
				earse_clock(former_hour, former_minute);
				draw_clock(hour, minute);
			}				
			delay_ms(10);
		}
		res=0;
	}
	myfree(SRAMIN,picfileinfo.lfname);	//�ͷ��ڴ�			    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picindextbl);			//�ͷ��ڴ�		
 }

