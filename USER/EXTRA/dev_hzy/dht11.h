#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h" 
#include "delay.h" 
//IO��������
#define DHT11_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define DHT11_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}
////IO��������											   
#define	DHT11_DQ_OUT PGout(11) //���ݶ˿�	PA0 
#define	DHT11_DQ_IN  PGin(11)  //���ݶ˿�	PA0 


u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    

void DHT11_Init_Wrapper(void);  // ��ʼ��DHT11
void DHT11_Update_Data_If_Expired(void);   // ������ʪ�����ݣ����жϵ��ã�
u8 DHT11_Get_Temperature(void); // ��ȡ�¶�
u8 DHT11_Get_Humidity(void);    // ��ȡʪ��


#endif
