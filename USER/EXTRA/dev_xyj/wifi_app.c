#include "wifi_app.h"

u8 test_wifi = 1; // WIFI测试临时变量 1:测试wifi

u8 net_mode = 0; // 网络模式
// WIFI初始化应用及检查
//  需要先初始化LCD
void WIFI_App_Init(u8 mode) {
    mode = WIFI_MODE_STA; // 测试阶段暂时先这么设
    atk_8266_hw_init();
    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(0, 30, "ATK-ESP8266 WIFI模块测试", 16, 240);
    while (atk_8266_send_cmd("AT", "OK", 20)) // 检查WIFI模块是否在线
    {
        atk_8266_quit_trans();                        // 退出透传
        atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); // 关闭透传模式
        Show_Str(40, 55, 200, 16, "未检测到模块!!!", 16, 0);
        delay_ms(800);
        LCD_Fill(40, 55, 200, 55 + 16, WHITE);
        Show_Str(40, 55, 200, 16, "尝试连接模块...", 16, 0);
    }
    while (atk_8266_send_cmd("ATE0", "OK", 20))
        ; // 关闭回显

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
        // 默认STA模式
        WIFI_STA_Set();
        break;
    }
}

void WIFI_STA_Set(void) {
    // 直接设置为工作在TCP客户端模式
    u8 key;
    u8 timex = 0;
    u8 ipbuf[16]; // IP缓存
    u8 *p;
    u16 t = 999;     // 加速第一次获取链接状态
    u8 constate = 0; // 连接状态

    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(0, 30, "设置为STA", 16, 240);
    p = mymalloc(SRAMIN, 32);                   // 申请32字节内存
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50); // 设置WIFI STA模式
    atk_8266_send_cmd("AT+RST", "OK", 20);      // DHCP服务器关闭(仅AP模式有效)
    delay_ms(1000);                             // 延时3S等待重启成功
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    // 设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!!
    sprintf((char *)p, "AT+CWJAP=\"%s\",\"%s\"", wifista_ssid, wifista_password); // 设置无线参数:ssid,密码
    while (atk_8266_send_cmd(p, "WIFI GOT IP", 300))
        LED1_Toggle;
    LED0_Toggle;
    ; // 连接目标路由器,并且获得IP

PRESTA:
    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(0, 30, "ATK-ESP WIFI-STA 测试", 16, 240);
    Show_Str(30, 50, 200, 16, "正在配置ATK-ESP模块,请稍等...", 12, 0);
    if (atk_8266_ip_set("WIFI-STA 远端IP设置", (u8 *)ATK_ESP8266_WORKMODE_TBL[net_mode], (u8 *)portnum, ipbuf)) goto PRESTA; // IP输入

    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20);                                // 0：单连接，1：多连接
    sprintf((char *)p, "AT+CIPSTART=\"TCP\",\"%s\",%s", ipbuf, (u8 *)portnum); // 配置目标TCP服务器
    while (atk_8266_send_cmd(p, "OK", 200)) {
        LCD_Clear(WHITE);
        POINT_COLOR = RED;
        Show_Str_Mid(0, 40, "WK_UP:返回重选", 16, 240);
        Show_Str_Mid(0, 80, "ATK-ESP 连接TCP Server失败", 12, 240); // 连接失败
        key = KEY_Scan(0);
        if (key == WKUP_PRES) goto PRESTA;
    }
    atk_8266_send_cmd("AT+CIPMODE=1", "OK", 200); // 传输模式为：透传

    LCD_Clear(WHITE);
    POINT_COLOR = RED;
    Show_Str_Mid(0, 30, "ATK-ESP WIFI-STA 测试", 16, 240);
    Show_Str(30, 50, 200, 16, "正在配置ATK-ESP模块,请稍等...", 12, 0);
    LCD_Fill(30, 50, 239, 50 + 12, WHITE); // 清除之前的显示
    Show_Str(30, 50, 200, 16, "WK_UP:退出测试  KEY0:发送数据", 12, 0);
    LCD_Fill(30, 80, 239, 80 + 12, WHITE);
    atk_8266_get_wanip(ipbuf); // 获取模块IP地址
    sprintf((char *)p, "IP地址:%s 端口:%s", ipbuf, (u8 *)portnum);
    Show_Str(30, 65, 200, 12, p, 12, 0);            // 显示IP地址和端口
    Show_Str(30, 80, 200, 12, "状态:", 12, 0);      // 连接状态
    Show_Str(120, 80, 200, 12, "模式:", 12, 0);     // 连接状态
    Show_Str(30, 100, 200, 12, "发送数据:", 12, 0); // 发送数据
    Show_Str(30, 115, 200, 12, "接收数据:", 12, 0); // 接收数据
    atk_8266_wificonf_show(30, 180, "请设置路由器无线参数为:", (u8 *)wifista_ssid, (u8 *)wifista_encryption, (u8 *)wifista_password);
    POINT_COLOR = BLUE;
    Show_Str(120 + 30, 80, 200, 12, (u8 *)ATK_ESP8266_WORKMODE_TBL[net_mode], 12, 0); // 连接状态
    USART3_RX_STA = 0;
    while (1) {
        key = KEY_Scan(0);
        if (key == WKUP_PRES) // WK_UP 退出测试
        {
            atk_8266_quit_trans();                       // 退出透传
            atk_8266_send_cmd("AT+CIPMODE=0", "OK", 20); // 关闭透传模式
            break;
        } else if (key == KEY0_PRES) // KEY0 发送数据
        {
            // TCP Client
            atk_8266_quit_trans();
            atk_8266_send_cmd("AT+CIPSEND", "OK", 20);                                              // 开始透传
            sprintf((char *)p, "ATK-8266%s测试%d\r\n", ATK_ESP8266_WORKMODE_TBL[net_mode], t / 10); // 测试数据
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

        if (t == 1000) // 连续10秒钟没有收到任何数据,检查连接是不是还存在.
        {
            constate = atk_8266_consta_check(); // 得到连接状态
            if (constate == '+')
                Show_Str(30 + 30, 80, 200, 12, "连接成功", 12, 0); // 连接状态
            else
                Show_Str(30 + 30, 80, 200, 12, "连接失败", 12, 0);
            t = 0;
        }
        if ((t % 20) == 0) LED0_Toggle;
        atk_8266_at_response(1);
    }
    myfree(SRAMIN, p); // 释放内存
}

// 数据接收处理 在主函数循环中调用
// 参数 con_state:连接状态
// p:外部传入的字符串指针 预先声明过内存空间 用于产生格式化字符串
u16 WIFI_RcvHandle(u8 con_state, u8 *p) {
    u16 t = 0;
    u16 rlen = 0;
    if (USART3_RX_STA & 0X8000) // 接收到一次数据了
    {
        rlen = USART3_RX_STA & 0X7FFF;                   // 得到本次接收到的数据长度
        USART3_RX_BUF[rlen] = 0;                         // 添加结束符
        printf("%s", USART3_RX_BUF);                     // 发送到串口
        sprintf((char *)p, "收到%d字节,内容如下", rlen); // 接收到的字节数
        LCD_Fill(30 + 54, 115, 239, 130, WHITE);
        POINT_COLOR = BRED;
        Show_Str(30 + 54, 115, 156, 12, p, 12, 0); // 显示接收到的数据长度
        POINT_COLOR = BLUE;
        LCD_Fill(30, 130, 239, 319, WHITE);
        Show_Str(30, 130, 180, 190, USART3_RX_BUF, 12, 0); // 显示接收到的数据
        USART3_RX_STA = 0;
        if (con_state != '+')
            t = 1000; // 状态为还未连接,立即更新连接状态
        else
            t = 0; // 状态为已经连接了,10秒后再检查
    }
    return t;
}