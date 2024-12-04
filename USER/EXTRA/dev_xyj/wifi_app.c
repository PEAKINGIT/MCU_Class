#include "wifi_app.h"

u8 test_wifi = 1; // WIFI������ʱ���� 1:����wifi

u8 net_mode = 0; // ����ģʽ
// WIFI��ʼ��Ӧ�ü����
//  ��Ҫ�ȳ�ʼ��LCD
void WIFI_App_Init(u8 mode) {
    mode = WIFI_MODE_STA; // ���Խ׶���ʱ����ô��
    atk_8266_hw_init();
    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(0, 30, "ATK-ESP8266 WIFIģ�����", 16, 240);
    while (atk_8266_send_cmd("AT", "OK", 20)) // ���WIFIģ���Ƿ�����
    {
        atk_8266_quit_trans();                        // �˳�͸��
        atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); // �ر�͸��ģʽ
        Show_Str(40, 55, 200, 16, "δ��⵽ģ��!!!", 16, 0);
        delay_ms(800);
        LCD_Fill(40, 55, 200, 55 + 16, WHITE);
        Show_Str(40, 55, 200, 16, "��������ģ��...", 16, 0);
    }
    while (atk_8266_send_cmd("ATE0", "OK", 20))
        ; // �رջ���

    switch (mode) {
    case WIFI_MODE_AP:
        /* code */
        break;
    case WIFI_MODE_STA:
        WIFI_STA_Set();
        break;
    case WIFI_MODE_APSTA:
        break;
    default:
        // Ĭ��STAģʽ
        WIFI_STA_Set();
        break;
    }
}

void WIFI_STA_Set(void) {
    // ֱ������Ϊ������TCP�ͻ���ģʽ
    u8 key;
    u8 timex = 0;
    u8 ipbuf[16]; // IP����
    u8 *p;
    u16 t = 999;     // ���ٵ�һ�λ�ȡ����״̬
    u8 constate = 0; // ����״̬

    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(0, 30, "����ΪSTA", 16, 240);
    p = mymalloc(SRAMIN, 32);                   // ����32�ֽ��ڴ�
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50); // ����WIFI STAģʽ
    atk_8266_send_cmd("AT+RST", "OK", 20);      // DHCP�������ر�(��APģʽ��Ч)
    delay_ms(1000);                             // ��ʱ3S�ȴ������ɹ�
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    // �������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!!
    sprintf((char *)p, "AT+CWJAP=\"%s\",\"%s\"", wifista_ssid, wifista_password); // �������߲���:ssid,����
    while (atk_8266_send_cmd(p, "WIFI GOT IP", 300))
        LED1_Toggle;
    LED0_Toggle;
    ; // ����Ŀ��·����,���һ��IP

PRESTA:
    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(0, 30, "ATK-ESP WIFI-STA ����", 16, 240);
    Show_Str(30, 50, 200, 16, "��������ATK-ESPģ��,���Ե�...", 12, 0);
    if (atk_8266_ip_set("WIFI-STA Զ��IP����", (u8 *)ATK_ESP8266_WORKMODE_TBL[net_mode], (u8 *)portnum, ipbuf)) goto PRESTA; // IP����

    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20);                                // 0�������ӣ�1��������
    sprintf((char *)p, "AT+CIPSTART=\"TCP\",\"%s\",%s", ipbuf, (u8 *)portnum); // ����Ŀ��TCP������
    while (atk_8266_send_cmd(p, "OK", 200)) {
        LCD_Clear(WHITE);
        POINT_COLOR = RED;
        Show_Str_Mid(0, 40, "WK_UP:������ѡ", 16, 240);
        Show_Str_Mid(0, 80, "ATK-ESP ����TCP Serverʧ��", 12, 240); // ����ʧ��
        key = KEY_Scan(0);
        if (key == WKUP_PRES) goto PRESTA;
    }
    atk_8266_send_cmd("AT+CIPMODE=1", "OK", 200); // ����ģʽΪ��͸��

    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(0, 30, "ATK-ESP WIFI-STA ����", 16, 240);
    Show_Str(30, 50, 200, 16, "��������ATK-ESPģ��,���Ե�...", 12, 0);
    LCD_Fill(30, 50, 239, 50 + 12, WHITE); // ���֮ǰ����ʾ
    Show_Str(30, 50, 200, 16, "WK_UP:�˳�����  KEY0:��������", 12, 0);
    LCD_Fill(30, 80, 239, 80 + 12, WHITE);
    atk_8266_get_wanip(ipbuf); // ��ȡģ��IP��ַ
    sprintf((char *)p, "IP��ַ:%s �˿�:%s", ipbuf, (u8 *)portnum);
    Show_Str(30, 65, 200, 12, p, 12, 0);            // ��ʾIP��ַ�Ͷ˿�
    Show_Str(30, 80, 200, 12, "״̬:", 12, 0);      // ����״̬
    Show_Str(120, 80, 200, 12, "ģʽ:", 12, 0);     // ����״̬
    Show_Str(30, 100, 200, 12, "��������:", 12, 0); // ��������
    Show_Str(30, 115, 200, 12, "��������:", 12, 0); // ��������
    atk_8266_wificonf_show(30, 180, "������·�������߲���Ϊ:", (u8 *)wifista_ssid, (u8 *)wifista_encryption, (u8 *)wifista_password);
    POINT_COLOR = BLUE;
    Show_Str(120 + 30, 80, 200, 12, (u8 *)ATK_ESP8266_WORKMODE_TBL[net_mode], 12, 0); // ����״̬
    USART3_RX_STA = 0;
    while (1) {
        key = KEY_Scan(0);
        if (key == WKUP_PRES) // WK_UP �˳�����
        {
            atk_8266_quit_trans();                       // �˳�͸��
            atk_8266_send_cmd("AT+CIPMODE=0", "OK", 20); // �ر�͸��ģʽ
            break;
        } else if (key == KEY0_PRES) // KEY0 ��������
        {
            // TCP Client
            atk_8266_quit_trans();
            atk_8266_send_cmd("AT+CIPSEND", "OK", 20);                                              // ��ʼ͸��
            sprintf((char *)p, "ATK-8266%s����%d\r\n", ATK_ESP8266_WORKMODE_TBL[net_mode], t / 10); // ��������
            Show_Str(30 + 54, 100, 200, 12, p, 12, 0);
            u3_printf("%s", p);
            timex = 100;
        } else
            ;
        if (timex) timex--;
        if (timex == 1) LCD_Fill(30 + 54, 100, 239, 112, WHITE);
        t++;
        delay_ms(10);

        t = WIFI_RcvHandle(constate, p);        // Rcv handle

        if (t == 1000) // ����10����û���յ��κ�����,��������ǲ��ǻ�����.
        {
            constate = atk_8266_consta_check(); // �õ�����״̬
            if (constate == '+')
                Show_Str(30 + 30, 80, 200, 12, "���ӳɹ�", 12, 0); // ����״̬
            else
                Show_Str(30 + 30, 80, 200, 12, "����ʧ��", 12, 0);
            t = 0;
        }
        if ((t % 20) == 0) LED0_Toggle;
        atk_8266_at_response(1);
    }
    myfree(SRAMIN, p); // �ͷ��ڴ�
}

// ���ݽ��մ��� ��������ѭ���е���
// ���� con_state:����״̬
// p:�ⲿ������ַ���ָ�� Ԥ���������ڴ�ռ� ���ڲ�����ʽ���ַ���
u16 WIFI_RcvHandle(u8 con_state, u8 *p) {
    u16 t = 0;
    u16 rlen = 0;
    if (USART3_RX_STA & 0X8000) // ���յ�һ��������
    {
        rlen = USART3_RX_STA & 0X7FFF;                   // �õ����ν��յ������ݳ���
        USART3_RX_BUF[rlen] = 0;                         // ��ӽ�����
        printf("%s", USART3_RX_BUF);                     // ���͵�����
        sprintf((char *)p, "�յ�%d�ֽ�,��������", rlen); // ���յ����ֽ���
        LCD_Fill(30 + 54, 115, 239, 130, WHITE);
        POINT_COLOR = BRED;
        Show_Str(30 + 54, 115, 156, 12, p, 12, 0); // ��ʾ���յ������ݳ���
        POINT_COLOR = BLUE;
        LCD_Fill(30, 130, 239, 319, WHITE);
        Show_Str(30, 130, 180, 190, USART3_RX_BUF, 12, 0); // ��ʾ���յ�������
        USART3_RX_STA = 0;
        if (con_state != '+')
            t = 1000; // ״̬Ϊ��δ����,������������״̬
        else
            t = 0; // ״̬Ϊ�Ѿ�������,10����ټ��
    }
    return t;
}