//GPSӦ�ò����
#include "./gps/gps_app.h"
//���ڽ����йر���
u16 i, rxlen;	
u16 lenx;
u8 rtn = 0;
u8 upload = 1;
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; // ����1,���ͻ�����

void Gps_Init(void) {
	//
    if (SkyTra_Cfg_Rate(5) != 0) // ���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ.
    {
        printf("SkyTraF8-BD Setting...\r\n");
        do {
            USART2_init(9600);           // ��ʼ������2������Ϊ9600
            SkyTra_Cfg_Prt(3);           // ��������ģ��Ĳ�����Ϊ38400
            USART2_init(38400);          // ��ʼ������2������Ϊ38400
            rtn = SkyTra_Cfg_Tp(100000); // ������Ϊ100ms
        } while (SkyTra_Cfg_Rate(5) != 0 && rtn != 0); // ����SkyTraF8-BD�ĸ�������Ϊ5Hz
        printf("SkyTraF8-BD Set Done!!\r\n");
        delay_ms(500);
    }
}

//������ѭ���� �����ڽ��յ�������
void Gps_Receive_Handle(void){
    if (USART2_RX_STA & 0X8000) // ���յ�һ��������
    {
        rxlen = USART2_RX_STA & 0X7FFF; // �õ����ݳ���
        for (i = 0; i < rxlen; i++)
            USART1_TX_BUF[i] = USART2_RX_BUF[i];
        USART2_RX_STA = 0;                               // ������һ�ν���
        USART1_TX_BUF[i] = 0;                            // �Զ���ӽ�����
        GPS_Analysis(&gpsData, (u8 *)USART1_TX_BUF);     // �����ַ���
        Gps_Msg_Print();                                 // ��ʾ��Ϣ
        if (upload) printf("\r\n%s\r\n", USART1_TX_BUF); // ���ͽ��յ������ݵ�����1
    }
    if ((lenx % 500) == 0)
        LED0_Toggle;
    lenx++;
}

// usart1��ʾGPS��λ��Ϣ
void Gps_Msg_Print(void) {
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
}