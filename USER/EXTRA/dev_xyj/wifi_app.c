#include "wifi_app.h"

u8 *default_ip = "10.162.191.181"; // 默认IP 设为作为TCP服务器的热点设备的地址
u8 net_mode = 1;                   // 网络模式 1:TCP Client
static u8 p[40];                   // 用于输出字符串的指针
u8 WIFI_state = 0;

/**
 * @brief 自定义通讯协议关键字
 */
const u8 *cmd_head = "GP9++:";

/**
 * @endparblock
 */

// WIFI初始化应用及检查
//  需要先初始化LCD
u8 WIFI_App_Init(void) {
    // u8 mode = WIFI_MODE_STA;
    u8 cnt = 0;
    atk_8266_hw_init();
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "WIFI模块初始化", 16, LCD_WIDTH);
    WIFI_state = 0;
    while (atk_8266_send_cmd("AT", "OK", 20)) // 检查WIFI模块是否在线
    {
        cnt++;
        if (cnt >= 3) { // 超时返回
            POINT_COLOR = RED;
            Show_Str(LCD_XSTART, LCD_YSTART + 20, 200, 16, "请检查WIFI模块连接!!!", 16, 0);
            delay_ms(400);
            WIFI_state |= (WIFIDEV_OFF | WIFI_ERR);
            return WIFI_state;
        }
        atk_8266_quit_trans();                        // 退出透传
        atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); // 关闭透传模式
        POINT_COLOR = RED;
        Show_Str(LCD_XSTART, LCD_YSTART + 20, 200, 16, "未检测到模块!!!", 16, 0);
        delay_ms(400);
        LCD_Fill(LCD_XSTART, LCD_YSTART + 20, 200, 55 + 16, WHITE);
        POINT_COLOR = RED;
        Show_Str(LCD_XSTART, LCD_YSTART + 20, 200, 16, "尝试连接模块...", 16, 0);
    }
	atk_8266_send_cmd("AT+CIPCLOSE", "OK", 100);	//关闭连接
    atk_8266_quit_trans();                        // 退出透传
    atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); // 关闭透传模式
    while (atk_8266_send_cmd("ATE0", "OK", 20))
        ; // 关闭回显
    WIFI_state |= WIFI_STA_Set();
    return WIFI_state;
}

// 设置作为WIFI STA, 工作在TCP客户端模式
u8 WIFI_STA_Set(void) {
    u8 key;
    uint16_t timeout = 0;
    u8 ipbuf[16]; // IP缓存
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "设置中...", 12, LCD_WIDTH); // 连接失败
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50);                      // 设置WIFI STA模式
    atk_8266_send_cmd("AT+RST", "OK", 20);                           // DHCP服务器关闭(仅AP模式有效)
    delay_ms(1000);                                                  // 延时3S等待重启成功
    delay_ms(1000);
    //delay_ms(1000);
    // 设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!!
    sprintf((char *)p, "AT+CWJAP=\"%s\",\"%s\"", wifista_ssid, wifista_password); // 设置无线参数:ssid,密码
    printf("cmd send:%s\r\n", p);
    timeout = 0;
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "开始连接网络", 12, LCD_WIDTH);
    while (atk_8266_send_cmd(p, "WIFI GOT IP", 300)) {
        // 连接目标路由器,并且获得IP
        timeout++;
        if (timeout >= 2) {
            // 超时退出连接
			LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
            POINT_COLOR = RED;
            Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "连接网络超时", 12, LCD_WIDTH); // 连接失败
            delay_ms(500);
            return NO_CNNCT;
        }
        LED1_Toggle;
        LED0_Toggle;
    }
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20);      // 0：单连接，1：多连接
    strcpy((char *)ipbuf, (const char *)default_ip); // 设置目标服务器IP
    atk_8266_send_cmd("AT+CIPCLOSE", "OK", 100);
    sprintf((char *)p, "AT+CIPSTART=\"TCP\",\"%s\",%s", ipbuf, (u8 *)portnum); // 配置目标TCP服务器
    printf("cmd send:%s\r\n", p);
    timeout = 0;
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(LCD_XSTART, LCD_YSTART + 20, "WK_UP:暂停连接服务器", 16, LCD_WIDTH);
    while (atk_8266_send_cmd(p, "OK", 200)) {
        timeout++;
        if (timeout >= 2) {
            // 超时退出连接
            POINT_COLOR = RED;
            Show_Str_Mid(LCD_XSTART, LCD_YSTART + 40, "连接服务器超时", 12, LCD_WIDTH); // 连接失败
            delay_ms(500);
            return NO_CNNCT;
        }
        key = KEY_Scan(0);
        if (key == WKUP_PRES) {
            POINT_COLOR = RED;
            Show_Str_Mid(LCD_XSTART, LCD_YSTART + 40, "暂停连接服务器", 12, LCD_WIDTH); // 连接失败
            delay_ms(500);
            return NO_CNNCT;
        }
    }
    atk_8266_send_cmd("AT+CIPMODE=1", "OK", 200); // 初始化透传模式
    USART3_RX_STA = 0;                            // 清空接收状态
    return GET_CNNCT;
}

// 封装的发送启动函数
// 接收只要透传模式启动即可
// 调用该函数后通过以下方式向服务器发送数据
// u3_printf("%s", send);
void WIFI_StartSendtoSever(void) {
    atk_8266_quit_trans();
    atk_8266_send_cmd("AT+CIPSEND", "OK", 20); // 开始透传发送
}

// 当前连接状态检查
// 建议当10s无数据接收时调用一次
// ret 0:WIFI_OK
// ret 1:ERR
uint8_t WIFI_ConnectCheck(void) {
    u8 constate = 0;                    // 连接状态
    constate = atk_8266_consta_check(); // 得到连接状态
    if (constate == '+')
        return WIFI_OK; // 连接正常
    else
        return WIFI_ERR; // 连接错误
}

// WIFI 状态显示
void WIFI_StateDisp(void) {
    u8 ipbuf[16];              // IP缓存
    atk_8266_get_wanip(ipbuf); // 获取模块IP地址
    sprintf((char *)p, "IP地址:%s 端口:%s", ipbuf, (u8 *)portnum);
    LCD_Fill(LCD_XSTART,LCD_YSTART,LCD_XEND,LCD_YEND,WHITE);
    POINT_COLOR = RED;
    Show_Str(30, 65, 200, 12, p, 12, 0);        // 显示IP地址和端口
    Show_Str(30, 80, 200, 12, "状态:", 12, 0);  // 连接状态
    Show_Str(120, 80, 200, 12, "模式:", 12, 0); // 连接状态
    atk_8266_wificonf_show(30, 180, "请设置路由器无线参数为:", (u8 *)wifista_ssid, (u8 *)wifista_encryption, (u8 *)wifista_password);
    POINT_COLOR = BLUE;
    Show_Str(120 + 30, 80, 200, 12, (u8 *)ATK_ESP8266_WORKMODE_TBL[net_mode], 12, 0); // 连接状态
}

// 数据接收处理 在主函数循环中调用
// param: wifi_isok:当前wifi状况;cmdExec:特定页面的指令执行函数指针
void WIFI_RcvHandle(u8 wifi_isok, void (*cmdExec)(u8 *)) {
    u16 rlen = 0;
	u8 *sptr;	//string ptr
	if(wifi_isok==WIFI_ERR){
		printf("WIFI CONNECTION NEED RESET!!!\r\n");
		return;
	}
    if (USART3_RX_STA & 0X8000) {
        // 接收到一次数据
        rlen = USART3_RX_STA & 0X7FFF; // 得到本次接收到的数据长度
        USART3_RX_BUF[rlen] = 0;       // 添加结束符

		//服务器命令处理
		sptr = (u8 *)strstr((const char *)USART3_RX_BUF,(const char *)cmd_head);
		sptr += 6;	//跳过cmd_head
		if(sptr!=NULL&&strlen((char *)sptr) > 0){
			//判断是否有正常的指令可以执行
			printf("sptr:%s\r\n",sptr);
			cmdExec(sptr);	//使用传入的函数指针接口来为特定页面实现专门的指令解析处理
		}
        printf("%s\r\n", USART3_RX_BUF);   // 发送到串口
		for(uint16_t i =0;i<rlen;i++){
			USART3_RX_BUF[i] = 0;	//清空缓冲区
		}
		USART3_RX_STA = 0;             // 清空接收状态
    }
}
