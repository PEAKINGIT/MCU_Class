#include "other_interface.h"
#include "delay.h"
#include "dht11.h"
#include "key.h"
#include "lcd.h"
#include "main_interface.h"
#include <string.h>  // 包含 memset 的标准库头文件

////////////////温湿度相关

#define MAX_DATA_POINTS 50 // 每分钟采集一次数据，所以最多存储60个数据点
#define X_AXIS_STEP 5      // X轴每隔几个点显示一个坐标
#define Y_AXIS_MAX 100     // Y轴最大值，100表示最大温湿度值


static uint8_t temperature_data[MAX_DATA_POINTS]; // 存储温度数据
static uint8_t humidity_data[MAX_DATA_POINTS];    // 存储湿度数据
static uint32_t time_data[MAX_DATA_POINTS];       // 存储数据采集时间戳（单位：ms）

static uint8_t data_index = 0; // 当前数据点索引
// 简单的线性拟合函数，返回预测值
int linear_predict(int x0, int y0, int x1, int y1, int x) {
    // 计算斜率
    int slope = (y1 - y0) / (x1 - x0);
    // 预测y值
    return y0 + slope * (x - x0);
}
////////////////温湿度相关



// 当前界面状态
extern InterfaceState current_state;
// 当前界面状态
InterfaceState current_state = MAIN_INTERFACE;

// 界面函数指针数组
void (*interface_functions[MAX_INTERFACE])(void) = {
    display_main_interface,
    display_sub_interface_1,
    display_sub_interface_2};

// 按键事件处理函数
void handle_key_event(void) {
    u8 key = KEY_Scan(0); // 扫描按键状态

    if (current_state == MAIN_INTERFACE) {
        // 主界面按键逻辑
        if (key == KEY0_PRES) {
            current_state = SUB_INTERFACE_1; // 进入分界面1
            display_sub_interface_1_init();  // 调用分界面1的初始化函数
        } else if (key == KEY1_PRES) {
            current_state = SUB_INTERFACE_2; // 进入分界面2
            display_sub_interface_2_init();  // 调用分界面2的初始化函数
        }
    } else if (current_state == SUB_INTERFACE_1) {
        // 分界面1按键逻辑
        if (key == KEY0_PRES) {
            current_state = MAIN_INTERFACE; // 返回主界面
                                            // interface_init();//主界面初始化
        }
    } else if (current_state == SUB_INTERFACE_2) {
        // 分界面2按键逻辑
        if (key == KEY1_PRES) {
            current_state = MAIN_INTERFACE; // 返回主界面
                                            // interface_init();//主界面初始化
        }
    }
}
// 主界面显示函数
void display_main_interface(void) {
    draw_mainInterface(); // 调用绘制主界面的函数
    delay_ms(100);
    LED1_Toggle;
}
// 分界面1初始化函数
void display_sub_interface_1_init(void) {
    LCD_Clear(WHITE); // 清屏操作，初始化界面背景

    // 清空温湿度数据
    memset(temperature_data, 0, sizeof(temperature_data));  // 将 temperature_data 数组清空
    memset(humidity_data, 0, sizeof(humidity_data));        // 将 humidity_data 数组清空
    memset(time_data, 0, sizeof(time_data));                // 将 time_data 数组清空
    data_index = 0; // 重置数据索引

    // 其他初始化操作，如绘制固定的静态内容
    // 绘制标题
    LCD_ShowString(40, 10, 200, 16, 16, "Temperature and Humidity");

    POINT_COLOR = BLUE; // 设置折线的颜色为蓝色

    // 绘制坐标轴
    // X 轴 (时间轴)
    LCD_DrawLine(30, 220, 310, 220); // X 轴线
    // Y 轴 (温湿度轴)
    LCD_DrawLine(30, 20, 30, 220);  // Y 轴线
}

#include <string.h>   // 解决 memset 未声明问题
#include <stdio.h>    // 解决 sprintf 未声明问题

// 分界面1显示函数（温湿度显示）
void display_sub_interface_1(void) {
    DHT11_Update_Data_If_Expired(); // 周期性更新 DHT11 数据

    static uint32_t last_print_tick = 0;
    if (tick_expired(&last_print_tick, 1000)) {
        // 获取温湿度数据
        uint8_t temperature = DHT11_Get_Temperature();
        uint8_t humidity = DHT11_Get_Humidity();
        
        // 存储当前的温湿度数据和时间索引
        temperature_data[data_index] = temperature;
        humidity_data[data_index] = humidity;
        time_data[data_index] = data_index; // 使用索引作为时间
        data_index = (data_index + 1) % MAX_DATA_POINTS; // 循环存储数据
        // X 轴刻度和标签
        for (int i = 0; i < MAX_DATA_POINTS; i++) {
            int x = (i * 320) / MAX_DATA_POINTS + 30; // 映射到屏幕 X 坐标
            if (i % X_AXIS_STEP == 0) {  // 每隔 X_AXIS_STEP 个点显示一个时间刻度
                char time_str[10];
                sprintf(time_str, "%d", i); // 显示时间
                LCD_ShowString(x, 230, 20, 16, 16, (u8 *)time_str); // 转换为 u8 *
            }
        }

        // Y 轴刻度和标签 (0 ~ 100)
        for (int i = 0; i <= Y_AXIS_MAX; i += 20) {
            int y = 220 - (i * 200) / Y_AXIS_MAX; // 将温湿度值映射到屏幕高度范围
            char value_str[10];
            sprintf(value_str, "%d", i); // 显示温湿度刻度
            LCD_ShowString(5, y - 8, 20, 16, 16, (u8 *)value_str); // 转换为 u8 *
        }

        // 绘制温度折线图
        for (int i = 1; i < MAX_DATA_POINTS; i++) {
            int prev_idx = (data_index + i - 1) % MAX_DATA_POINTS;
            int curr_idx = (data_index + i) % MAX_DATA_POINTS;

            // 缩放时间索引到屏幕宽度范围 (0 ~ 320)
            int x0 = (time_data[prev_idx] * 320) / MAX_DATA_POINTS + 30; // 映射到屏幕 X 坐标
            int y0 = 220 - (temperature_data[prev_idx] * 200) / Y_AXIS_MAX; // 映射到屏幕 Y 坐标
            int x1 = (time_data[curr_idx] * 320) / MAX_DATA_POINTS + 30;
            int y1 = 220 - (temperature_data[curr_idx] * 200) / Y_AXIS_MAX;
         
            POINT_COLOR = BLUE; // 设置折线的颜色为蓝色
            LCD_DrawLine(x0, y0, x1, y1); // 绘制温度数据的折线
        }

        // 绘制湿度折线图
        for (int i = 1; i < MAX_DATA_POINTS; i++) {
            int prev_idx = (data_index + i - 1) % MAX_DATA_POINTS;
            int curr_idx = (data_index + i) % MAX_DATA_POINTS;

            // 缩放时间索引到屏幕宽度范围 (0 ~ 320)
            int x0 = (time_data[prev_idx] * 320) / MAX_DATA_POINTS + 30; // 映射到屏幕 X 坐标
            int y0 = 220 - (humidity_data[prev_idx] * 200) / Y_AXIS_MAX; // 映射到屏幕 Y 坐标
            int x1 = (time_data[curr_idx] * 320) / MAX_DATA_POINTS + 30;
            int y1 = 220 - (humidity_data[curr_idx] * 200) / Y_AXIS_MAX;
					
            POINT_COLOR = BLUE; // 设置折线的颜色为蓝色
            LCD_DrawLine(x0, y0, x1, y1); // 绘制湿度数据的折线
        }

        // 预测下一秒的温湿度
        if (data_index > 1) {
            int temp_pred = linear_predict(time_data[data_index - 2], temperature_data[data_index - 2],
                                           time_data[data_index - 1], temperature_data[data_index - 1],
                                           data_index + 1); // 使用索引作为预测的时间
            int humi_pred = linear_predict(time_data[data_index - 2], humidity_data[data_index - 2],
                                           time_data[data_index - 1], humidity_data[data_index - 1],
                                           data_index + 1); // 使用索引作为预测的时间

            POINT_COLOR = RED; // 设置预测线的颜色为红色
            LCD_DrawLine(data_index * 320 / MAX_DATA_POINTS + 30, 220 - temp_pred * 200 / Y_AXIS_MAX, 
                         (data_index + 1) * 320 / MAX_DATA_POINTS + 30, 220 - temp_pred * 200 / Y_AXIS_MAX); // 预测温度折线
            LCD_DrawLine(data_index * 320 / MAX_DATA_POINTS + 30, 220 - humi_pred * 200 / Y_AXIS_MAX, 
                         (data_index + 1) * 320 / MAX_DATA_POINTS + 30, 220 - humi_pred * 200 / Y_AXIS_MAX); // 预测湿度折线
            POINT_COLOR = BLUE; // 设置折线的颜色为蓝色
        }

        // 在屏幕下方显示当前温湿度值
        char temp_str[20];
        sprintf(temp_str, "Temp: %d C", temperature); // 当前温度
        LCD_ShowString(30, 250, 200, 16, 16, (u8 *)temp_str);  // 转换为 u8 *

        char humi_str[20];
        sprintf(humi_str, "Humi: %d %%", humidity); // 当前湿度
        LCD_ShowString(30, 270, 200, 16, 16, (u8 *)humi_str); // 转换为 u8 *

        // 检查时间是否超过 30，超过时清空数据
        if (data_index >= 30) {
            // 清空温湿度数据和时间数据
            memset(temperature_data, 0, sizeof(temperature_data));  
            memset(humidity_data, 0, sizeof(humidity_data));        
            memset(time_data, 0, sizeof(time_data));                
            data_index = 0; // 重置数据索引
						display_sub_interface_1_init();
        }

        delay_ms(100); // 延时，模拟动态内容
    }
}




// 分界面2初始化函数
void display_sub_interface_2_init(void) {
    LCD_Clear(WHITE); // 清屏操作，初始化界面背景
    // 其他初始化操作，如绘制固定的静态内容
    LCD_ShowString(10, 10, 200, 16, 16, "This is Sub Interface 2"); // 显示标题
}

// 分界面2显示函数
void display_sub_interface_2(void) {

    delay_ms(100); // 延时，模拟动态内容
}
// 界面模块初始化
void interface_init(void) {
    current_state = MAIN_INTERFACE;       // 设置初始界面
    interface_functions[current_state](); // 显示初始界面
}