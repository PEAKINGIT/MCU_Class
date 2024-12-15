// GPS应用层代码
#include "gps_app.h"
// 串口接收有关变量
u16 i, rxlen;
u8 rtn = 0;
u8 upload = 0;
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; // 串口1,发送缓存区
__align(4) u8 dtbuf[50];               // 打印缓存器

u8 Gps_Init(void) {
    //
    POINT_COLOR = RED;
    LCD_DrawRectangle(LCD_XSTART - 1, LCD_YSTART - 1, LCD_XEND + 1, LCD_YEND + 1);
    USART2_init(38400); // 初始化串口2波特率为9600
    u8 time_out = 0;    // 超时判断
    if (SkyTra_Cfg_Rate(5) != 0) {
        // 设置定位信息更新速度为5Hz
        // 非0说明失败 重新配置顺便判断GPS模块是否在位.
        LCD_ShowString(30, 120, 200, 16, 16, "SkyTraF8-BD Setting...");
        do {
            delay_ms(10);
            time_out++;
            if (time_out > GPS_INIT_TIMEOUT) {
                // 超时返回(2s)
                LCD_ShowString(30, 120, 200, 16, 16, "SkyTraF8-BD Set TimeOut!");
                LCD_ShowString(30, 140, 200, 16, 16, "Check Connection!");
                delay_ms(500);
                return GPS_TimeOut;
            }
            USART2_init(9600);           // 初始化串口2波特率为9600
            SkyTra_Cfg_Prt(3);           // 重新设置模块的波特率为38400
            USART2_init(38400);          // 初始化串口2波特率为38400
            rtn = SkyTra_Cfg_Tp(100000); // 脉冲宽度为100ms
        } while (SkyTra_Cfg_Rate(5) != 0 && rtn != 0); // 配置SkyTraF8-BD的更新速率为5Hz
        LCD_ShowString(30, 120, 200, 16, 16, "SkyTraF8-BD Set Done!!");
        delay_ms(500);
        return GPS_InitOK;
    } else {
        LCD_ShowString(30, 120, 200, 16, 16, "SkyTraF8-BD Set Done!!");
        delay_ms(1000);
        return GPS_InitOK;
    }
}

// 放在主循环中 处理串口接收到的数据
void Gps_Receive_Handle(void) {
    if (USART2_RX_STA & 0X8000) // 接收到一次数据了
    {
        rxlen = USART2_RX_STA & 0X7FFF; // 得到数据长度
        for (i = 0; i < rxlen; i++)
            USART1_TX_BUF[i] = USART2_RX_BUF[i];
        USART1_TX_BUF[i] = 0;                            // 自动添加结束符
        if (upload) printf("\r\n%s\r\n", USART1_TX_BUF); // 发送接收到的数据到串口1
        USART2_RX_STA = 0;                               // 启动下一次接收

        GPS_Analysis(&gpsData, (u8 *)USART1_TX_BUF); // 分析字符串 从缓存中获取数据
        // Gps_Msg_Print();                                 // 显示信息
    }
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
    if (gpsData.fixmode <= 3) {
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

// UTC时间转换为任意时区时间，如果是转换为北京时间，timezone传8即可
void utc_to_local_time(nmea_utc_time *utc_time, int8_t timezone, nmea_utc_time *local_time) {
    int year, month, day, hour;
    int lastday = 0;     // last day of this month 本月天数
    int lastlastday = 0; // last day of last month 上个月天数

    year = utc_time->year; // utc time
    month = utc_time->month;
    day = utc_time->date;
    hour = utc_time->hour + timezone;

    // 1月大，2月小，3月大，4月小，5月大，6月小，7月大，8月大，9月小，10月大，11月小，12月大
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        lastday = 31;     // 本月天数
        lastlastday = 30; // 这里应该补上上个月的天数

        if (month == 3) {
            if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) // if this is lunar year
                lastlastday = 29;
            else
                lastlastday = 28;
        }

        if (month == 8 || month == 1) // 这里应该是8月和1月，因为8月和1月的上一个月（7月和12月）的天数是31天的
            lastlastday = 31;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        lastday = 30;
        lastlastday = 31;
    } else {
        lastlastday = 31;

        if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
            lastday = 29;
        else
            lastday = 28;
    }

    if (hour >= 24) // if >24, day+1
    {
        hour -= 24;
        day += 1;
        if (day > lastday) // next month, day-lastday of this month
        {
            day -= lastday;
            month += 1;
            if (month > 12) // next year, month-12
            {
                month -= 12;
                year += 1;
            }
        }
    }

    if (hour < 0) // if <0, day-1
    {
        hour += 24;
        day -= 1;
        if (day < 1) // month-1, day=last day of last month
        {
            day = lastlastday;
            month -= 1;
            if (month < 1) // last year, month=12
            {
                month = 12;
                year -= 1;
            }
        }
    }

    // transfer value to local_time
    local_time->year = year;
    local_time->month = month;
    local_time->date = day;
    local_time->hour = hour;
    local_time->min = utc_time->min;
    local_time->sec = utc_time->sec;
}
