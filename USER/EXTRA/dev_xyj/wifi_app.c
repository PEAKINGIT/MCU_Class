#include "wifi_app.h"

u8 *default_ip = "10.162.191.181"; // Ĭ��IP ��Ϊ��ΪTCP���������ȵ��豸�ĵ�ַ
u8 net_mode = 1;                   // ����ģʽ 1:TCP Client
static u8 p[40];                   // ��������ַ�����ָ��
u8 WIFI_state = 0;

/**
 * @brief �Զ���ͨѶЭ��ؼ���
 */
const u8 *cmd_head = "GP9++:";

/**
 * @endparblock
 */

// WIFI��ʼ��Ӧ�ü����
//  ��Ҫ�ȳ�ʼ��LCD
u8 WIFI_App_Init(void) {
    // u8 mode = WIFI_MODE_STA;
    u8 cnt = 0;
    atk_8266_hw_init();
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "WIFIģ���ʼ��", 16, LCD_WIDTH);
    WIFI_state = 0;
    while (atk_8266_send_cmd("AT", "OK", 20)) // ���WIFIģ���Ƿ�����
    {
        cnt++;
        if (cnt >= 4) { // ��ʱ����
            POINT_COLOR = RED;
            Show_Str(LCD_XSTART, LCD_YSTART + 20, 200, 16, "����WIFIģ������!!!", 16, 0);
            delay_ms(400);
            WIFI_state |= (WIFIDEV_OFF | WIFI_ERR);
            return WIFI_state;
        }
        atk_8266_quit_trans();                        // �˳�͸��
        atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); // �ر�͸��ģʽ
        POINT_COLOR = RED;
        Show_Str(LCD_XSTART, LCD_YSTART + 20, 200, 16, "δ��⵽ģ��!!!", 16, 0);
        delay_ms(400);
        LCD_Fill(LCD_XSTART, LCD_YSTART + 20, 200, 55 + 16, WHITE);
        POINT_COLOR = RED;
        Show_Str(LCD_XSTART, LCD_YSTART + 20, 200, 16, "��������ģ��...", 16, 0);
    }
	atk_8266_send_cmd("AT+CIPCLOSE", "OK", 100);	//�ر�����
    atk_8266_quit_trans();                        // �˳�͸��
    atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); // �ر�͸��ģʽ
    while (atk_8266_send_cmd("ATE0", "OK", 20))
        ; // �رջ���
    WIFI_state |= WIFI_STA_Set();
    return WIFI_state;
}

// ������ΪWIFI STA, ������TCP�ͻ���ģʽ
u8 WIFI_STA_Set(void) {
    u8 key;
    uint16_t timeout = 0;
    u8 ipbuf[16]; // IP����
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "������...", 12, LCD_WIDTH); // ����ʧ��
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50);                      // ����WIFI STAģʽ
    atk_8266_send_cmd("AT+RST", "OK", 20);                           // DHCP�������ر�(��APģʽ��Ч)
    delay_ms(1000);                                                  // ��ʱ3S�ȴ������ɹ�
    delay_ms(1000);
    delay_ms(1000);
    // �������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!!
    sprintf((char *)p, "AT+CWJAP=\"%s\",\"%s\"", wifista_ssid, wifista_password); // �������߲���:ssid,����
    printf("cmd send:%s\r\n", p);
    timeout = 0;
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "��ʼ��������", 12, LCD_WIDTH);
    while (atk_8266_send_cmd(p, "WIFI GOT IP", 300)) {
        // ����Ŀ��·����,���һ��IP
        timeout++;
        if (timeout >= 2) {
            // ��ʱ�˳�����
			LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
            POINT_COLOR = RED;
            Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "�������糬ʱ", 12, LCD_WIDTH); // ����ʧ��
            delay_ms(500);
            return NO_CNNCT;
        }
        LED1_Toggle;
        LED0_Toggle;
    }
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20);      // 0�������ӣ�1��������
    strcpy((char *)ipbuf, (const char *)default_ip); // ����Ŀ�������IP
    atk_8266_send_cmd("AT+CIPCLOSE", "OK", 100);
    sprintf((char *)p, "AT+CIPSTART=\"TCP\",\"%s\",%s", ipbuf, (u8 *)portnum); // ����Ŀ��TCP������
    printf("cmd send:%s\r\n", p);
    timeout = 0;
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "WK_UP:��ͣ���ӷ�����", 16, LCD_WIDTH);
    while (atk_8266_send_cmd(p, "OK", 200)) {
        timeout++;
        if (timeout >= 2) {
            // ��ʱ�˳�����
            POINT_COLOR = RED;
            Show_Str_Mid(LCD_XSTART, LCD_YSTART + 40, "���ӷ�������ʱ", 12, LCD_WIDTH); // ����ʧ��
            delay_ms(500);
            return NO_CNNCT;
        }
        key = KEY_Scan(0);
        if (key == WKUP_PRES) {
            POINT_COLOR = RED;
            Show_Str_Mid(LCD_XSTART, LCD_YSTART + 40, "��ͣ���ӷ�����", 12, LCD_WIDTH); // ����ʧ��
            delay_ms(500);
            return NO_CNNCT;
        }
    }
    atk_8266_send_cmd("AT+CIPMODE=1", "OK", 200); // ��ʼ��͸��ģʽ
    USART3_RX_STA = 0;                            // ��ս���״̬
    return GET_CNNCT;
}

// ��װ�ķ�����������
// ����ֻҪ͸��ģʽ��������
// ���øú�����ͨ�����·�ʽ���������������
// u3_printf("%s", send);
void WIFI_StartSendtoSever(void) {
    atk_8266_quit_trans();
    atk_8266_send_cmd("AT+CIPSEND", "OK", 20); // ��ʼ͸������
}

// ��ǰ����״̬���
// ���鵱10s�����ݽ���ʱ����һ��
// ret 0:WIFI_OK
// ret 1:ERR
uint8_t WIFI_ConnectCheck(void) {
    u8 constate = 0;                    // ����״̬
    constate = atk_8266_consta_check(); // �õ�����״̬
    if (constate == '+')
        return WIFI_OK; // ��������
    else
        return WIFI_ERR; // ���Ӵ���
}

// WIFI ״̬��ʾ
void WIFI_StateDisp(void) {
    u8 ipbuf[16];              // IP����
    atk_8266_get_wanip(ipbuf); // ��ȡģ��IP��ַ
    sprintf((char *)p, "IP��ַ:%s �˿�:%s", ipbuf, (u8 *)portnum);
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str(30, 65, 200, 12, p, 12, 0);        // ��ʾIP��ַ�Ͷ˿�
    Show_Str(30, 80, 200, 12, "״̬:", 12, 0);  // ����״̬
    Show_Str(120, 80, 200, 12, "ģʽ:", 12, 0); // ����״̬
    atk_8266_wificonf_show(30, 180, "������·�������߲���Ϊ:", (u8 *)wifista_ssid, (u8 *)wifista_encryption, (u8 *)wifista_password);
    POINT_COLOR = BLUE;
    Show_Str(120 + 30, 80, 200, 12, (u8 *)ATK_ESP8266_WORKMODE_TBL[net_mode], 12, 0); // ����״̬
}

// ���ݽ��մ��� ��������ѭ���е���
// param: wifi_isok:��ǰwifi״��;cmdExec:�ض�ҳ���ָ��ִ�к���ָ��
void WIFI_RcvHandle(u8 wifi_isok, void (*cmdExec)(u8 *)) {
    u16 rlen = 0;
	u8 *sptr;	//string ptr
	if(wifi_isok==WIFI_ERR){
		printf("WIFI CONNECTION NEED RESET!!!\r\n");
		return;
	}
    if (USART3_RX_STA & 0X8000) {
        // ���յ�һ������
        rlen = USART3_RX_STA & 0X7FFF; // �õ����ν��յ������ݳ���
        USART3_RX_BUF[rlen] = 0;       // ��ӽ�����

		//�����������
		sptr = (u8 *)strstr((const char *)USART3_RX_BUF,(const char *)cmd_head);
		sptr += 6;	//����cmd_head
		if(sptr!=NULL&&strlen((char *)sptr) > 0){
			//�ж��Ƿ���������ָ�����ִ��
			printf("sptr:%s\r\n",sptr);
			cmdExec(sptr);	//ʹ�ô���ĺ���ָ��ӿ���Ϊ�ض�ҳ��ʵ��ר�ŵ�ָ���������
		}
        printf("%s\r\n", USART3_RX_BUF);   // ���͵�����
		for(uint16_t i =0;i<rlen;i++){
			USART3_RX_BUF[i] = 0;	//��ջ�����
		}
		USART3_RX_STA = 0;             // ��ս���״̬
    }
}
