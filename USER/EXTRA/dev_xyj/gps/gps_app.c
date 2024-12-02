//GPS应用层代码
#include "./gps/gps_app.h"
//串口接收有关变量
u16 i, rxlen;	
u16 lenx;
u8 rtn = 0;
u8 upload = 1;
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; // 串口1,发送缓存区

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

//放在主循环中 处理串口接收到的数据
void Gps_Receive_Handle(void){
    if (USART2_RX_STA & 0X8000) // 接收到一次数据了
    {
        rxlen = USART2_RX_STA & 0X7FFF; // 得到数据长度
        for (i = 0; i < rxlen; i++)
            USART1_TX_BUF[i] = USART2_RX_BUF[i];
        USART2_RX_STA = 0;                               // 启动下一次接收
        USART1_TX_BUF[i] = 0;                            // 自动添加结束符
        GPS_Analysis(&gpsData, (u8 *)USART1_TX_BUF);     // 分析字符串
        Gps_Msg_Print();                                 // 显示信息
        if (upload) printf("\r\n%s\r\n", USART1_TX_BUF); // 发送接收到的数据到串口1
    }
    if ((lenx % 500) == 0)
        LED0_Toggle;
    lenx++;
}

// usart1显示GPS定位信息
void Gps_Msg_Print(void) {
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
}