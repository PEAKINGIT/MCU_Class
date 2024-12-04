// GPS应用层代码
#include "gps_app.h"
// 串口接收有关变量
u16 i, rxlen;
u16 lenx;
u8 rtn = 0;
u8 upload = 1;
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; // 串口1,发送缓存区
__align(4) u8 dtbuf[50];   								//打印缓存器

void Gps_Init(void) {
    //
    if (SkyTra_Cfg_Rate(5) != 0) // 设置定位信息更新速度为5Hz,顺便判断GPS模块是否在位.
    {
        printf("SkyTraF8-BD Setting...\r\n");
        do {
            USART2_init(9600);           // 初始化串口2波特率为9600
            SkyTra_Cfg_Prt(3);           // 重新设置模块的波特率为38400
            USART2_init(38400);          // 初始化串口2波特率为38400
            rtn = SkyTra_Cfg_Tp(100000); // 脉冲宽度为100ms
        } while (SkyTra_Cfg_Rate(5) != 0 && rtn != 0); // 配置SkyTraF8-BD的更新速率为5Hz
        printf("SkyTraF8-BD Set Done!!\r\n");
        delay_ms(500);
    }
}

// 放在主循环中 处理串口接收到的数据
void Gps_Receive_Handle(void) {
    if (USART2_RX_STA & 0X8000) // 接收到一次数据了
    {
        rxlen = USART2_RX_STA & 0X7FFF; // 得到数据长度
        for (i = 0; i < rxlen; i++)
            USART1_TX_BUF[i] = USART2_RX_BUF[i];
        USART2_RX_STA = 0; // 启动下一次接收

        GPS_Analysis(&gpsData, (u8 *)USART1_TX_BUF);     // 分析字符串 从缓存中获取数据
        Gps_Msg_Print();                                 // 显示信息
        USART1_TX_BUF[i] = 0;                            // 自动添加结束符
        if (upload) printf("\r\n%s\r\n", USART1_TX_BUF); // 发送接收到的数据到串口1
    }
    if ((lenx % 500) == 0)
        LED0_Toggle;
    lenx++;
}

// 显示GPS定位信息 GUI or USART1
void Gps_Msg_Print(void) {

#define GPS_GUI
#ifdef GPS_GUI
    float tp;
    POINT_COLOR = BLUE;
    tp = gpsData.longitude;
    sprintf((char *)dtbuf, "Longitude:%.5f %1c   ", tp /= 100000, gpsData.ewhemi); // 得到经度字符串
    LCD_ShowString(30, 120, 200, 16, 16, dtbuf);
    tp = gpsData.latitude;
    sprintf((char *)dtbuf, "Latitude:%.5f %1c   ", tp /= 100000, gpsData.nshemi); // 得到纬度字符串
    LCD_ShowString(30, 140, 200, 16, 16, dtbuf);
    tp = gpsData.altitude;
    sprintf((char *)dtbuf, "Altitude:%.1fm     ", tp /= 10); // 得到高度字符串
    LCD_ShowString(30, 160, 200, 16, 16, dtbuf);
    tp = gpsData.speed;
    sprintf((char *)dtbuf, "Speed:%.3fkm/h     ", tp /= 1000); // 得到速度字符串
    LCD_ShowString(30, 180, 200, 16, 16, dtbuf);
    if (gpsData.fixmode <= 3){
		 // 定位状态
        sprintf((char *)dtbuf, "Fix Mode:%s", fixmode_tbl[gpsData.fixmode]);
        LCD_ShowString(30, 200, 200, 16, 16, dtbuf);
    }
    sprintf((char *)dtbuf, "GPS+BD Valid satellite:%02d", gpsData.posslnum); // 用于定位的GPS卫星数
    LCD_ShowString(30, 220, 200, 16, 16, dtbuf);
    sprintf((char *)dtbuf, "GPS Visible satellite:%02d", gpsData.svnum % 100); // 可见GPS卫星数
    LCD_ShowString(30, 240, 200, 16, 16, dtbuf);

    sprintf((char *)dtbuf, "BD Visible satellite:%02d", gpsData.beidou_svnum % 100); // 可见北斗卫星数
    LCD_ShowString(30, 260, 200, 16, 16, dtbuf);

    sprintf((char *)dtbuf, "UTC Date:%04d/%02d/%02d   ", gpsData.utc.year, gpsData.utc.month, gpsData.utc.date); // 显示UTC日期
    LCD_ShowString(30, 280, 200, 16, 16, dtbuf);
    sprintf((char *)dtbuf, "UTC Time:%02d:%02d:%02d   ", gpsData.utc.hour, gpsData.utc.min, gpsData.utc.sec); // 显示UTC时间
    LCD_ShowString(30, 300, 200, 16, 16, dtbuf);
#else
    float tp;
    tp = gpsData.longitude;
    printf("Longitude:%.5f %1c\r\n", tp /= 100000, gpsData.ewhemi); // 得到经度字符串
    tp = gpsData.latitude;
    printf("Latitude:%.5f %1c\r\n", tp /= 100000, gpsData.nshemi); // 得到纬度字符串
    tp = gpsData.altitude;
    printf("Altitude:%.1fm\r\n", tp /= 10); // 得到高度字符串
    tp = gpsData.speed;
    printf("Speed:%.3fkm/h\r\n", tp /= 1000); // 得到速度字符串
    if (gpsData.fixmode <= 3) {               // 定位状态
        printf("Fix Mode:%s\r\n", fixmode_tbl[gpsData.fixmode]);
    }
    printf("GPS+BD Valid satellite:%02d\r\n", gpsData.posslnum);                                  // 用于定位的GPS卫星数
    printf("GPS Visible satellite:%02d\r\n", gpsData.svnum % 100);                                // 可见GPS卫星数
    printf("BD Visible satellite:%02d\r\n", gpsData.beidou_svnum % 100);                          // 可见北斗卫星数
    printf("UTC Date:%04d/%02d/%02d\r\n", gpsData.utc.year, gpsData.utc.month, gpsData.utc.date); // 显示UTC日期
    printf("UTC Time:%02d:%02d:%02d\r\n", gpsData.utc.hour, gpsData.utc.min, gpsData.utc.sec);    // 显示UTC时间
#endif
}
