#include "dht11_interface.h"

////////////////温湿度相关

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

// 分界面1初始化函数
void display_sub_interface_1_init(void) {
    LCD_Clear(WHITE); // 清屏操作，初始化界面背景

    // 清空温湿度数据
    memset(temperature_data, 0, sizeof(temperature_data)); // 将 temperature_data 数组清空
    memset(humidity_data, 0, sizeof(humidity_data));       // 将 humidity_data 数组清空
    memset(time_data, 0, sizeof(time_data));               // 将 time_data 数组清空
    data_index = 0;                                        // 重置数据索引
    // 绘制标题
    LCD_ShowString(50, 60, 180, 16, 12, "Temperature and Humidity");

    POINT_COLOR = BLUE; // 设置折线的颜色为蓝色
    // 绘制 X 轴 (时间轴)
    int x_axis_start = 30;                                      // X 轴起点
    int x_axis_end = 210;                                       // X 轴终点 (30 + 180 的宽度)
    int x_axis_y = 220;                                         // X 轴在矩形底部
    LCD_DrawLine(x_axis_start, x_axis_y, x_axis_end, x_axis_y); // 绘制 X 轴线

    // 绘制 Y 轴 (湿度轴)
    int y_axis_start = 55;                                      // Y 轴顶部
    int y_axis_end = 220;                                       // Y 轴底部
    int y_axis_x = 30;                                          // Y 轴在矩形左侧
    LCD_DrawLine(y_axis_x, y_axis_start, y_axis_x, y_axis_end); // 绘制 Y 轴线

    // 绘制边界
    POINT_COLOR = RED;
    LCD_DrawRectangle(15, 55, 224, 264);
    POINT_COLOR = BLUE;
    // X 轴刻度和标签
    for (int i = 0; i <= MAX_DATA_POINTS; i++) {
        int x = (i * 190) / MAX_DATA_POINTS + 20; // 映射到屏幕 X 坐标
        if (i % X_AXIS_STEP == 0) {               // 每隔 X_AXIS_STEP 个点显示一个时间刻度
            char time_str[10];
            sprintf(time_str, "%d", i);                         // 显示时间
            LCD_ShowString(x, 230, 20, 16, 12, (u8 *)time_str); // 转换为 u8 *
        }
    }

    // 定义缩放系数，缩短标签之间的距离（值越小距离越短）
    float scale_factor = 1; // 缩放系数 (< 1 缩短距离，> 1 拉伸距离)

    for (int i = 0; i <= Y_AXIS_MAX; i += 20) {
        // 调整 Y 坐标的映射范围，与湿度折线图一致
        if (i > 0) {
            int y = 240 - (int)((i * 170 * scale_factor) / Y_AXIS_MAX); // 映射到湿度折线图的 Y 坐标范围
            char value_str[10];
            sprintf(value_str, "%d", i);                            // 显示湿度刻度
            LCD_ShowString(17, y - 6, 20, 16, 12, (u8 *)value_str); // 将标签显示在刻度旁
        }
    }
}
void DHT_interface(void) {
    /*变量初始化*/
    u8 key;
    uint32_t last_print_tick = 0;
    /*界面绘制初始化*/
    display_sub_interface_1_init();
    /*界面循环, 每个界面有独立的循环而不是都在主循环里*/
    while (1) {
        /*循环中事件响应, 包括且不仅按键扫描等, 可以自己加*/
        DHT11_Update_Data_If_Expired(); // 周期性更新 DHT11 数据
        if (tick_expired(&last_print_tick, 1000)) {
            // 获取温湿度数据
            uint8_t temperature = DHT11_Get_Temperature();
            uint8_t humidity = DHT11_Get_Humidity();

            // 存储当前的温湿度数据和时间索引
            temperature_data[data_index] = temperature;
            humidity_data[data_index] = humidity;
            time_data[data_index] = data_index;              // 使用索引作为时间
            data_index = (data_index + 1) % MAX_DATA_POINTS; // 循环存储数据

            // 绘制温度折线图
            for (int i = 1; i < MAX_DATA_POINTS; i++) {
                int prev_idx = (data_index + i - 1 + MAX_DATA_POINTS) % MAX_DATA_POINTS;
                int curr_idx = (data_index + i + MAX_DATA_POINTS) % MAX_DATA_POINTS;

                // 跳过无效点
                if (time_data[prev_idx] == 0 || temperature_data[prev_idx] == 0 ||
                    time_data[curr_idx] == 0 || temperature_data[curr_idx] == 0) {
                    continue;
                }

                // 缩放时间索引到屏幕宽度范围 (0 ~ 180)
                int x0 = (time_data[prev_idx] * 180) / MAX_DATA_POINTS + 30;
                int y0 = 240 - (temperature_data[prev_idx] * 170) / Y_AXIS_MAX;
                int x1 = (time_data[curr_idx] * 180) / MAX_DATA_POINTS + 30;
                int y1 = 240 - (temperature_data[curr_idx] * 170) / Y_AXIS_MAX;

                // 绘制折线
                POINT_COLOR = BLUE;
                LCD_DrawLine(x0, y0, x1, y1);
            }

            // 绘制湿度折线图
            for (int i = 1; i < MAX_DATA_POINTS; i++) {

                int prev_idx = (data_index + i - 1) % MAX_DATA_POINTS;
                int curr_idx = (data_index + i) % MAX_DATA_POINTS;
                // 跳过无效点
                if (time_data[prev_idx] == 0 || temperature_data[prev_idx] == 0 ||
                    time_data[curr_idx] == 0 || temperature_data[curr_idx] == 0) {
                    continue;
                }
                // 缩放时间索引到屏幕宽度范围 (55 ~ 264)
                int x0 = (time_data[prev_idx] * 180) / MAX_DATA_POINTS + 30; // 映射到矩形范围的 X 坐标
                int y0 = 240 - (humidity_data[prev_idx] * 170) / Y_AXIS_MAX; // 映射到矩形范围的 Y 坐标
                int x1 = (time_data[curr_idx] * 180) / MAX_DATA_POINTS + 30; // 映射到矩形范围的 X 坐标
                int y1 = 240 - (humidity_data[curr_idx] * 170) / Y_AXIS_MAX; // 映射到矩形范围的 Y 坐标

                POINT_COLOR = BLUE;           // 设置折线的颜色为蓝色
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
                                   // 映射预测温度折线的坐标
                LCD_DrawLine(data_index * 180 / MAX_DATA_POINTS + 30, 240 - temp_pred * 170 / Y_AXIS_MAX, (data_index + 1) * 180 / MAX_DATA_POINTS + 30, 240 - temp_pred * 170 / Y_AXIS_MAX);

                // 映射预测湿度折线的坐标
                LCD_DrawLine(data_index * 180 / MAX_DATA_POINTS + 30, 240 - humi_pred * 170 / Y_AXIS_MAX, (data_index + 1) * 180 / MAX_DATA_POINTS + 30, 240 - humi_pred * 170 / Y_AXIS_MAX);

                POINT_COLOR = BLUE; // 设置折线的颜色为蓝色
            }

            // 在屏幕下方显示当前温湿度值
            char temp_str[20];
            sprintf(temp_str, "Temp: %d C", temperature);         // 当前温度
            LCD_ShowString(30, 245, 200, 16, 12, (u8 *)temp_str); // 转换为 u8 *

            char humi_str[20];
            sprintf(humi_str, "Humi: %d %%", humidity);            // 当前湿度
            LCD_ShowString(100, 245, 200, 16, 12, (u8 *)humi_str); // 转换为 u8 *

            // 检查时间是否超过 50，超过时清空数据
            if (data_index >= 49) {
                // 清空温湿度数据和时间数据
                memset(temperature_data, 0, sizeof(temperature_data));
                memset(humidity_data, 0, sizeof(humidity_data));
                memset(time_data, 0, sizeof(time_data));
                data_index = 0; // 重置数据索引
                display_sub_interface_1_init();
            }
        }
        key = KEY_Scan(0);
        /*退出逻辑 即满足什么条件退出界面*/
        if (key == KEY0_PRES) break;

        delay_ms(100);
    }
    /*下一界面确定 可以自行拓展成更丰富的逻辑*/
    current_page = MENU; // 主界面退出回到menu
                         /*清空显示*/
    LCD_Clear(WHITE);
}
