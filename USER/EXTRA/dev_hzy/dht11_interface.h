#ifndef __DHT_INTERFACE
#define __DHT_INTERFACE

#include "stm32f10x.h"
#include "lcd.h"
#include "delay.h"
#include "dht11.h"
#include "key.h"
#include "menu.h"
#include <string.h>  // ���� memset �ı�׼��ͷ�ļ�
#include <stdio.h>    // ��� sprintf δ��������

#define MAX_DATA_POINTS 50 // ÿ���Ӳɼ�һ�����ݣ��������洢60�����ݵ�
#define X_AXIS_STEP 10      // X��ÿ����������ʾһ������
#define Y_AXIS_MAX 100     // Y�����ֵ��100��ʾ�����ʪ��ֵ

void DHT_interface(void);
void display_sub_interface_1_init(void); //�ֽ���1��ʼ������
extern int linear_predict(int x0, int y0, int x1, int y1, int x);

#endif
