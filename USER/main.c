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
 
// 屏幕中心坐标
#define CENTER_X 120
#define CENTER_Y 160

// 指针长度
#define HOUR_HAND_LENGTH 50
#define MINUTE_HAND_LENGTH 70

// 将角度转换为弧度
#define DEG_TO_RAD(angle)((angle) * 3.1415 / 180.0)


//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息	
	u8 *fn;	 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录
  	tfileinfo.lfsize=_MAX_LFN*2+1;				//长文件名最大长度
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	return rval;
}

// 计算指针终点坐标
void CalculatePointerCoordinates(uint16_t centerX, uint16_t centerY, float angle, uint16_t length, uint16_t *x, uint16_t *y) {
    *x = centerX + length * cos(DEG_TO_RAD(angle - 90));
    *y = centerY + length * sin(DEG_TO_RAD(angle - 90));
}

// 绘制表针
void draw_clock(uint16_t hour, uint16_t minute) {
    uint16_t hour_x, hour_y, minute_x, minute_y;
    
    // 计算时针和分针的角度
    float angle_hour = (hour % 12 + minute / 60.0) * 30.0;
    float angle_minute = minute * 6.0;
    
    // 计算时针和分针终点坐标
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_hour, HOUR_HAND_LENGTH, &hour_x, &hour_y);
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_minute, MINUTE_HAND_LENGTH, &minute_x, &minute_y);

    // 绘制时针
	POINT_COLOR=RED; 
    LCD_DrawLine(CENTER_X, CENTER_Y, hour_x, hour_y);
    
    // 绘制分针
	POINT_COLOR=BLUE;
    LCD_DrawLine(CENTER_X, CENTER_Y, minute_x, minute_y);
}
// 擦除表针
void earse_clock(uint16_t hour, uint16_t minute) {
	uint16_t hour_x, hour_y, minute_x, minute_y;
    
    // 计算时针和分针的角度
    float angle_hour = (hour % 12 + minute / 60.0) * 30.0;
    float angle_minute = minute * 6.0;
    
    // 计算时针和分针终点坐标
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_hour, HOUR_HAND_LENGTH, &hour_x, &hour_y);
    CalculatePointerCoordinates(CENTER_X, CENTER_Y, angle_minute, MINUTE_HAND_LENGTH, &minute_x, &minute_y);
	
	POINT_COLOR=WHITE;
    // 绘制时针
	LCD_DrawLine(CENTER_X, CENTER_Y, hour_x, hour_y);
    
    // 绘制分针
	LCD_DrawLine(CENTER_X, CENTER_Y, minute_x, minute_y);
}


int main(void)
 {	 
 	u8 t=0;
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO picfileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 pause=0;			//暂停标记
	u16 temp;
	u16 *picindextbl;	//图片索引表
	
	u16 hour;
	u16 minute;
	u16 former_hour;
	u16 former_minute;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			    //LED端口初始化
	LCD_Init();			 	//初始化LCD
	usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART	
	RTC_Init();	  			//RTC初始化
	SD_Init();	
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	
	POINT_COLOR=RED;//设置字体为红色 
	while(font_init()) 		//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}
	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(30,170,240,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	} 
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(30,170,240,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//为带路径的文件名分配内存
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//申请2*totpicnum个字节的内存,用于存放图片索引
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//内存分配出错
 	{	    
		Show_Str(30,170,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.index;								//记录当前index
	        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picindextbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}
	piclib_init();										//初始化画图	   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录	
	
	
	while(res==FR_OK)
	{
		dir_sdi(&picdir,picindextbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出
     	fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面
		LCD_Clear(WHITE);
		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//显示图片    
		Show_Str(2,2,240,16,pname,16,1); 				//显示图片名字		
		draw_clock(calendar.hour,calendar.min);
		while(1)
		{
			former_hour=hour;
			former_minute=minute;
			hour=calendar.hour;
			minute=calendar.min;
			key=KEY_Scan(0);		//扫描按键
			if(t!=calendar.sec)
			{
				t=calendar.sec;
				//显示时间
				POINT_COLOR=BLUE;//设置字体为蓝色	 
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
	myfree(SRAMIN,picfileinfo.lfname);	//释放内存			    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picindextbl);			//释放内存		
 }

