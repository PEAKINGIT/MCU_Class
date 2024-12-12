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
    LCD_ShowString(50, 60, 180, 16, 12, "Temperature and Humidity");

    POINT_COLOR = BLUE; // �������ߵ���ɫΪ��ɫ
// ���� X �� (ʱ����)
int x_axis_start = 30;  // X �����
int x_axis_end = 210;   // X ���յ� (30 + 180 �Ŀ��)
int x_axis_y = 220;     // X ���ھ��εײ�
LCD_DrawLine(x_axis_start, x_axis_y, x_axis_end, x_axis_y); // ���� X ����

// ���� Y �� (ʪ����)
int y_axis_start = 55;  // Y �ᶥ��
int y_axis_end = 220;   // Y ��ײ�
int y_axis_x = 30;      // Y ���ھ������
LCD_DrawLine(y_axis_x, y_axis_start, y_axis_x, y_axis_end); // ���� Y ����

	 //���Ʊ߽�
	  POINT_COLOR = RED;
	  LCD_DrawRectangle(15, 55, 224, 264);
		POINT_COLOR = BLUE;
	   // X ��̶Ⱥͱ�ǩ
        for (int i = 0; i <= MAX_DATA_POINTS; i++) {
            int x = (i * 190) / MAX_DATA_POINTS + 20; // ӳ�䵽��Ļ X ����
            if (i % X_AXIS_STEP == 0) {               // ÿ�� X_AXIS_STEP ������ʾһ��ʱ��̶�
                char time_str[10];
                sprintf(time_str, "%d", i);                         // ��ʾʱ��
                LCD_ShowString(x, 230, 20, 16, 12, (u8 *)time_str); // ת��Ϊ u8 *
            }
        }

        
// ��������ϵ�������̱�ǩ֮��ľ��루ֵԽС����Խ�̣�
float scale_factor = 1; // ����ϵ�� (< 1 ���̾��룬> 1 �������)

for (int i = 0; i <= Y_AXIS_MAX; i += 20) {
    // ���� Y �����ӳ�䷶Χ����ʪ������ͼһ��
	  if(i>0){
    int y = 240 - (int)((i * 170 * scale_factor) / Y_AXIS_MAX); // ӳ�䵽ʪ������ͼ�� Y ���귶Χ
    char value_str[10];
    sprintf(value_str, "%d", i);                           // ��ʾʪ�ȿ̶�
    LCD_ShowString(17, y - 6, 20, 16, 12, (u8 *)value_str); // ����ǩ��ʾ�ڿ̶���
		}
}



}
void display_sub_interface_1(void) {
    /*������ʼ��*/
    u8 key;
	
    /*������Ƴ�ʼ��*/
	display_sub_interface_1_init();
    /*����ѭ��, ÿ�������ж�����ѭ�������Ƕ�����ѭ����*/
    while (1) {
        /*ѭ�����¼���Ӧ, �����Ҳ�������ɨ���, �����Լ���*/
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
     

        // �����¶�����ͼ
				for (int i = 1; i < MAX_DATA_POINTS; i++) {
						int prev_idx = (data_index + i - 1 + MAX_DATA_POINTS) % MAX_DATA_POINTS;
						int curr_idx = (data_index + i + MAX_DATA_POINTS) % MAX_DATA_POINTS;

						// ������Ч��
						if (time_data[prev_idx] == 0 || temperature_data[prev_idx] == 0 ||
								time_data[curr_idx] == 0 || temperature_data[curr_idx] == 0) {
								continue;
						}

						// ����ʱ����������Ļ��ȷ�Χ (0 ~ 180)
						int x0 = (time_data[prev_idx] * 180) / MAX_DATA_POINTS + 30;    
						int y0 = 240 - (temperature_data[prev_idx] * 170) / Y_AXIS_MAX; 
						int x1 = (time_data[curr_idx] * 180) / MAX_DATA_POINTS + 30;    
						int y1 = 240 - (temperature_data[curr_idx] * 170) / Y_AXIS_MAX;

						// ��������
						POINT_COLOR = BLUE;           
						LCD_DrawLine(x0, y0, x1, y1); 
		}


        // ����ʪ������ͼ
        for (int i = 1; i < MAX_DATA_POINTS; i++) {
				
            int prev_idx = (data_index + i - 1) % MAX_DATA_POINTS;
            int curr_idx = (data_index + i) % MAX_DATA_POINTS;
					// ������Ч��
					if (time_data[prev_idx] == 0 || temperature_data[prev_idx] == 0 ||
							time_data[curr_idx] == 0 || temperature_data[curr_idx] == 0) {
							continue;
					}
            // ����ʱ����������Ļ��ȷ�Χ (55 ~ 264)
           int x0 = (time_data[prev_idx] * 180) / MAX_DATA_POINTS + 30; // ӳ�䵽���η�Χ�� X ����
           int y0 = 240 - (humidity_data[prev_idx] * 170) / Y_AXIS_MAX; // ӳ�䵽���η�Χ�� Y ����
           int x1 = (time_data[curr_idx] * 180) / MAX_DATA_POINTS + 30; // ӳ�䵽���η�Χ�� X ����
           int y1 = 240 - (humidity_data[curr_idx] * 170) / Y_AXIS_MAX; // ӳ�䵽���η�Χ�� Y ����


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
            // ӳ��Ԥ���¶����ߵ�����
              LCD_DrawLine(data_index * 180 / MAX_DATA_POINTS + 30, 240 - temp_pred * 170 / Y_AXIS_MAX,(data_index + 1) * 180 / MAX_DATA_POINTS + 30, 240 - temp_pred * 170 / Y_AXIS_MAX);

             // ӳ��Ԥ��ʪ�����ߵ�����
              LCD_DrawLine(data_index * 180 / MAX_DATA_POINTS + 30, 240 - humi_pred * 170 / Y_AXIS_MAX,(data_index + 1) * 180 / MAX_DATA_POINTS + 30, 240 - humi_pred * 170 / Y_AXIS_MAX);

            POINT_COLOR = BLUE;                                                                              // �������ߵ���ɫΪ��ɫ
        }

        // ����Ļ�·���ʾ��ǰ��ʪ��ֵ
        char temp_str[20];
        sprintf(temp_str, "Temp: %d C", temperature);         // ��ǰ�¶�
        LCD_ShowString(30, 245, 200, 16, 12, (u8 *)temp_str); // ת��Ϊ u8 *

        char humi_str[20];
        sprintf(humi_str, "Humi: %d %%", humidity);           // ��ǰʪ��
        LCD_ShowString(100, 245, 200, 16, 12, (u8 *)humi_str); // ת��Ϊ u8 *

        // ���ʱ���Ƿ񳬹� 50������ʱ�������
        if (data_index >= 49) {
            // �����ʪ�����ݺ�ʱ������
            memset(temperature_data, 0, sizeof(temperature_data));
            memset(humidity_data, 0, sizeof(humidity_data));
            memset(time_data, 0, sizeof(time_data));
            data_index = 0; // ������������
            display_sub_interface_1_init();
        }

     
    }
        key = KEY_Scan(0);
        /*�˳��߼� ������ʲô�����˳�����*/
        if (key == KEY0_PRES) break;
       
        delay_ms(100);
        
    }
    /*��һ����ȷ�� ����������չ�ɸ��ḻ���߼�*/
    current_page = MENU; // �������˳��ص�menu
    /*�����ʾ*/
	LCD_Clear(WHITE);
}
