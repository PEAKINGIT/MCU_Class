// GPSӦ�ò����
#include "gps_app.h"
// ���ڽ����йر���
u16 i, rxlen;
u8 rtn = 0;
u8 upload = 0;
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; // ����1,���ͻ�����
__align(4) u8 dtbuf[50];               // ��ӡ������

u8 Gps_Init(void) {
    //
	POINT_COLOR = RED;
    LCD_DrawRectangle(LCD_XSTART - 1, LCD_YSTART - 1, LCD_XEND + 1, LCD_YEND + 1);
    USART2_init(38400); // ��ʼ������2������Ϊ9600
    u8 time_out = 0;    // ��ʱ�ж�
    if (SkyTra_Cfg_Rate(5) != 0) {
        // ���ö�λ��Ϣ�����ٶ�Ϊ5Hz
        // ��0˵��ʧ�� ��������˳���ж�GPSģ���Ƿ���λ.
        LCD_ShowString(30, 120, 200, 16, 16, "SkyTraF8-BD Setting...");
        do {
            delay_ms(10);
            time_out++;
            if (time_out > GPS_INIT_TIMEOUT) {
                // ��ʱ����(2s)
                LCD_ShowString(30, 120, 200, 16, 16, "SkyTraF8-BD Set TimeOut!");
                LCD_ShowString(30, 140, 200, 16, 16, "Check Connection!");
                delay_ms(500);
                return GPS_TimeOut;
            }
            USART2_init(9600);           // ��ʼ������2������Ϊ9600
            SkyTra_Cfg_Prt(3);           // ��������ģ��Ĳ�����Ϊ38400
            USART2_init(38400);          // ��ʼ������2������Ϊ38400
            rtn = SkyTra_Cfg_Tp(100000); // ������Ϊ100ms
        } while (SkyTra_Cfg_Rate(5) != 0 && rtn != 0); // ����SkyTraF8-BD�ĸ�������Ϊ5Hz
        LCD_ShowString(30, 120, 200, 16, 16, "SkyTraF8-BD Set Done!!");
        delay_ms(500);
        return GPS_InitOK;
    } else {
        LCD_ShowString(30, 120, 200, 16, 16, "SkyTraF8-BD Set Done!!");
        delay_ms(1000);
        return GPS_InitOK;
    }
}

// ������ѭ���� �����ڽ��յ�������
void Gps_Receive_Handle(void) {
    if (USART2_RX_STA & 0X8000) // ���յ�һ��������
    {
        rxlen = USART2_RX_STA & 0X7FFF; // �õ����ݳ���
        for (i = 0; i < rxlen; i++)
            USART1_TX_BUF[i] = USART2_RX_BUF[i];
        USART1_TX_BUF[i] = 0;                            // �Զ���ӽ�����
        if (upload) printf("\r\n%s\r\n", USART1_TX_BUF); // ���ͽ��յ������ݵ�����1
        USART2_RX_STA = 0;                               // ������һ�ν���

        GPS_Analysis(&gpsData, (u8 *)USART1_TX_BUF); // �����ַ��� �ӻ����л�ȡ����
        // Gps_Msg_Print();                                 // ��ʾ��Ϣ
    }
}

// ��ʾGPS��λ��Ϣ GUI or USART1
void Gps_Msg_Print(void) {

#define GPS_GUI
#ifdef GPS_GUI
    float tp;
    POINT_COLOR = BLUE;
    tp = gpsData.longitude;
    sprintf((char *)dtbuf, "Longitude:%.5f %1c   ", tp /= 100000, gpsData.ewhemi); // �õ������ַ���
    LCD_ShowString(30, 120, 200, 16, 16, dtbuf);
    tp = gpsData.latitude;
    sprintf((char *)dtbuf, "Latitude:%.5f %1c   ", tp /= 100000, gpsData.nshemi); // �õ�γ���ַ���
    LCD_ShowString(30, 140, 200, 16, 16, dtbuf);
    tp = gpsData.altitude;
    sprintf((char *)dtbuf, "Altitude:%.1fm     ", tp /= 10); // �õ��߶��ַ���
    LCD_ShowString(30, 160, 200, 16, 16, dtbuf);
    tp = gpsData.speed;
    sprintf((char *)dtbuf, "Speed:%.3fkm/h     ", tp /= 1000); // �õ��ٶ��ַ���
    LCD_ShowString(30, 180, 200, 16, 16, dtbuf);
    if (gpsData.fixmode <= 3) {
        // ��λ״̬
        sprintf((char *)dtbuf, "Fix Mode:%s", fixmode_tbl[gpsData.fixmode]);
        LCD_ShowString(30, 200, 200, 16, 16, dtbuf);
    }
    sprintf((char *)dtbuf, "GPS+BD Valid satellite:%02d", gpsData.posslnum); // ���ڶ�λ��GPS������
    LCD_ShowString(30, 220, 200, 16, 16, dtbuf);
    sprintf((char *)dtbuf, "GPS Visible satellite:%02d", gpsData.svnum % 100); // �ɼ�GPS������
    LCD_ShowString(30, 240, 200, 16, 16, dtbuf);

    sprintf((char *)dtbuf, "BD Visible satellite:%02d", gpsData.beidou_svnum % 100); // �ɼ�����������
    LCD_ShowString(30, 260, 200, 16, 16, dtbuf);

    sprintf((char *)dtbuf, "UTC Date:%04d/%02d/%02d   ", gpsData.utc.year, gpsData.utc.month, gpsData.utc.date); // ��ʾUTC����
    LCD_ShowString(30, 280, 200, 16, 16, dtbuf);
    sprintf((char *)dtbuf, "UTC Time:%02d:%02d:%02d   ", gpsData.utc.hour, gpsData.utc.min, gpsData.utc.sec); // ��ʾUTCʱ��
    LCD_ShowString(30, 300, 200, 16, 16, dtbuf);
#else
    float tp;
    tp = gpsData.longitude;
    printf("Longitude:%.5f %1c\r\n", tp /= 100000, gpsData.ewhemi); // �õ������ַ���
    tp = gpsData.latitude;
    printf("Latitude:%.5f %1c\r\n", tp /= 100000, gpsData.nshemi); // �õ�γ���ַ���
    tp = gpsData.altitude;
    printf("Altitude:%.1fm\r\n", tp /= 10); // �õ��߶��ַ���
    tp = gpsData.speed;
    printf("Speed:%.3fkm/h\r\n", tp /= 1000); // �õ��ٶ��ַ���
    if (gpsData.fixmode <= 3) {               // ��λ״̬
        printf("Fix Mode:%s\r\n", fixmode_tbl[gpsData.fixmode]);
    }
    printf("GPS+BD Valid satellite:%02d\r\n", gpsData.posslnum);                                  // ���ڶ�λ��GPS������
    printf("GPS Visible satellite:%02d\r\n", gpsData.svnum % 100);                                // �ɼ�GPS������
    printf("BD Visible satellite:%02d\r\n", gpsData.beidou_svnum % 100);                          // �ɼ�����������
    printf("UTC Date:%04d/%02d/%02d\r\n", gpsData.utc.year, gpsData.utc.month, gpsData.utc.date); // ��ʾUTC����
    printf("UTC Time:%02d:%02d:%02d\r\n", gpsData.utc.hour, gpsData.utc.min, gpsData.utc.sec);    // ��ʾUTCʱ��
#endif
}
