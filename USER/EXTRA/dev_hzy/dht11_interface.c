#include "dht11_interface.h"

////////////////��ʪ�����

static uint8_t temperature_data[MAX_DATA_POINTS]; // �洢�¶�����
static uint8_t humidity_data[MAX_DATA_POINTS];    // �洢ʪ������
static uint32_t time_data[MAX_DATA_POINTS];       // �洢���ݲɼ�ʱ�������λ��ms��

static uint8_t data_index = 0; // ��ǰ���ݵ�����
// �򵥵�������Ϻ���������Ԥ��ֵ
int linear_predict(int x0, int y0, int x1, int y1, int x) {
    // ����б��
    int slope = (y1 - y0) / (x1 - x0);
    // Ԥ��yֵ
    return y0 + slope * (x - x0);
}
////////////////��ʪ�����

// �ֽ���1��ʼ������
void display_sub_interface_1_init(void) {
    LCD_Clear(WHITE); // ������������ʼ�����汳��

    // �����ʪ������
    memset(temperature_data, 0, sizeof(temperature_data)); // �� temperature_data �������
    memset(humidity_data, 0, sizeof(humidity_data));       // �� humidity_data �������
    memset(time_data, 0, sizeof(time_data));               // �� time_data �������
    data_index = 0;                                        // ������������
    // ���Ʊ���
    LCD_ShowString(40, 10, 200, 16, 16, "Temperature and Humidity");

    POINT_COLOR = BLUE; // �������ߵ���ɫΪ��ɫ
    // ����������
    // X �� (ʱ����)
    LCD_DrawLine(30, 220, 310, 220); // X ����
    // Y �� (��ʪ����)
    LCD_DrawLine(30, 20, 30, 220); // Y ����
}

// �ֽ���1��ʾ��������ʪ����ʾ��
void display_sub_interface_1(void) {
    DHT11_Update_Data_If_Expired(); // �����Ը��� DHT11 ����

    static uint32_t last_print_tick = 0;
    if (tick_expired(&last_print_tick, 1000)) {
        // ��ȡ��ʪ������
        uint8_t temperature = DHT11_Get_Temperature();
        uint8_t humidity = DHT11_Get_Humidity();

        // �洢��ǰ����ʪ�����ݺ�ʱ������
        temperature_data[data_index] = temperature;
        humidity_data[data_index] = humidity;
        time_data[data_index] = data_index;              // ʹ��������Ϊʱ��
        data_index = (data_index + 1) % MAX_DATA_POINTS; // ѭ���洢����
        // X ��̶Ⱥͱ�ǩ
        for (int i = 0; i < MAX_DATA_POINTS; i++) {
            int x = (i * 320) / MAX_DATA_POINTS + 30; // ӳ�䵽��Ļ X ����
            if (i % X_AXIS_STEP == 0) {               // ÿ�� X_AXIS_STEP ������ʾһ��ʱ��̶�
                char time_str[10];
                sprintf(time_str, "%d", i);                         // ��ʾʱ��
                LCD_ShowString(x, 230, 20, 16, 16, (u8 *)time_str); // ת��Ϊ u8 *
            }
        }

        // Y ��̶Ⱥͱ�ǩ (0 ~ 100)
        for (int i = 0; i <= Y_AXIS_MAX; i += 20) {
            int y = 220 - (i * 200) / Y_AXIS_MAX; // ����ʪ��ֵӳ�䵽��Ļ�߶ȷ�Χ
            char value_str[10];
            sprintf(value_str, "%d", i);                           // ��ʾ��ʪ�ȿ̶�
            LCD_ShowString(5, y - 8, 20, 16, 16, (u8 *)value_str); // ת��Ϊ u8 *
        }

        // �����¶�����ͼ
        for (int i = 1; i < MAX_DATA_POINTS; i++) {
            int prev_idx = (data_index + i - 1) % MAX_DATA_POINTS;
            int curr_idx = (data_index + i) % MAX_DATA_POINTS;

            // ����ʱ����������Ļ��ȷ�Χ (0 ~ 320)
            int x0 = (time_data[prev_idx] * 320) / MAX_DATA_POINTS + 30;    // ӳ�䵽��Ļ X ����
            int y0 = 220 - (temperature_data[prev_idx] * 200) / Y_AXIS_MAX; // ӳ�䵽��Ļ Y ����
            int x1 = (time_data[curr_idx] * 320) / MAX_DATA_POINTS + 30;
            int y1 = 220 - (temperature_data[curr_idx] * 200) / Y_AXIS_MAX;

            POINT_COLOR = BLUE;           // �������ߵ���ɫΪ��ɫ
            LCD_DrawLine(x0, y0, x1, y1); // �����¶����ݵ�����
        }

        // ����ʪ������ͼ
        for (int i = 1; i < MAX_DATA_POINTS; i++) {
            int prev_idx = (data_index + i - 1) % MAX_DATA_POINTS;
            int curr_idx = (data_index + i) % MAX_DATA_POINTS;

            // ����ʱ����������Ļ��ȷ�Χ (0 ~ 320)
            int x0 = (time_data[prev_idx] * 320) / MAX_DATA_POINTS + 30; // ӳ�䵽��Ļ X ����
            int y0 = 220 - (humidity_data[prev_idx] * 200) / Y_AXIS_MAX; // ӳ�䵽��Ļ Y ����
            int x1 = (time_data[curr_idx] * 320) / MAX_DATA_POINTS + 30;
            int y1 = 220 - (humidity_data[curr_idx] * 200) / Y_AXIS_MAX;

            POINT_COLOR = BLUE;           // �������ߵ���ɫΪ��ɫ
            LCD_DrawLine(x0, y0, x1, y1); // ����ʪ�����ݵ�����
        }

        // Ԥ����һ�����ʪ��
        if (data_index > 1) {
            int temp_pred = linear_predict(time_data[data_index - 2], temperature_data[data_index - 2],
                                           time_data[data_index - 1], temperature_data[data_index - 1],
                                           data_index + 1); // ʹ��������ΪԤ���ʱ��
            int humi_pred = linear_predict(time_data[data_index - 2], humidity_data[data_index - 2],
                                           time_data[data_index - 1], humidity_data[data_index - 1],
                                           data_index + 1); // ʹ��������ΪԤ���ʱ��

            POINT_COLOR = RED; // ����Ԥ���ߵ���ɫΪ��ɫ
            LCD_DrawLine(data_index * 320 / MAX_DATA_POINTS + 30, 220 - temp_pred * 200 / Y_AXIS_MAX,
                         (data_index + 1) * 320 / MAX_DATA_POINTS + 30, 220 - temp_pred * 200 / Y_AXIS_MAX); // Ԥ���¶�����
            LCD_DrawLine(data_index * 320 / MAX_DATA_POINTS + 30, 220 - humi_pred * 200 / Y_AXIS_MAX,
                         (data_index + 1) * 320 / MAX_DATA_POINTS + 30, 220 - humi_pred * 200 / Y_AXIS_MAX); // Ԥ��ʪ������
            POINT_COLOR = BLUE;                                                                              // �������ߵ���ɫΪ��ɫ
        }

        // ����Ļ�·���ʾ��ǰ��ʪ��ֵ
        char temp_str[20];
        sprintf(temp_str, "Temp: %d C", temperature);         // ��ǰ�¶�
        LCD_ShowString(30, 250, 200, 16, 16, (u8 *)temp_str); // ת��Ϊ u8 *

        char humi_str[20];
        sprintf(humi_str, "Humi: %d %%", humidity);           // ��ǰʪ��
        LCD_ShowString(30, 270, 200, 16, 16, (u8 *)humi_str); // ת��Ϊ u8 *

        // ���ʱ���Ƿ񳬹� 30������ʱ�������
        if (data_index >= 30) {
            // �����ʪ�����ݺ�ʱ������
            memset(temperature_data, 0, sizeof(temperature_data));
            memset(humidity_data, 0, sizeof(humidity_data));
            memset(time_data, 0, sizeof(time_data));
            data_index = 0; // ������������
            display_sub_interface_1_init();
        }

        delay_ms(100); // ��ʱ��ģ�⶯̬����
    }
}
