#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h" 
#include "delay.h" 
//IO方向设置
#define DHT11_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define DHT11_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}
////IO操作函数											   
#define	DHT11_DQ_OUT PGout(11) //数据端口	PA0 
#define	DHT11_DQ_IN  PGin(11)  //数据端口	PA0 


u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11    

void DHT11_Init_Wrapper(void);  // 初始化DHT11
void DHT11_Update_Data_If_Expired(void);   // 更新温湿度数据（供中断调用）
u8 DHT11_Get_Temperature(void); // 获取温度
u8 DHT11_Get_Humidity(void);    // 获取湿度


#endif
