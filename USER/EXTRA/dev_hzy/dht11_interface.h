#ifndef __DHT_INTERFACE
#define __DHT_INTERFACE

#include "stm32f10x.h"
#include "lcd.h"
#include "delay.h"
#include "dht11.h"
#include "key.h"
#include "menu.h"
#include <string.h>  // 包含 memset 的标准库头文件
#include <stdio.h>    // 解决 sprintf 未声明问题

#define MAX_DATA_POINTS 50 // 每分钟采集一次数据，所以最多存储60个数据点
#define X_AXIS_STEP 10      // X轴每隔几个点显示一个坐标
#define Y_AXIS_MAX 100     // Y轴最大值，100表示最大温湿度值

void DHT_interface(void);
void display_sub_interface_1_init(void); //分界面1初始化函数
extern int linear_predict(int x0, int y0, int x1, int y1, int x);

#endif
