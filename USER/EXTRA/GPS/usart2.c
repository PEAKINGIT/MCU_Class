#include "usart2.h"

// 串口接收缓存区
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; // 接收缓冲,最大USART2_MAX_RECV_LEN个字节.
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; // 发送缓冲,最大USART2_MAX_SEND_LEN字节

// 通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
// 如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到

// 任何数据,则表示此次接收完毕.
// 接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART2_RX_STA = 0;

void USART2_IRQHandler(void) {
    u8 res;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收到数据
    {
        res = USART_ReceiveData(USART2);
        if ((USART2_RX_STA & (1 << 15)) == 0) // 接收完的一批数据,还没有被处理,则不再接收其他数据
        {
            if (USART2_RX_STA < USART2_MAX_RECV_LEN) // 还可以接收数据
            {
                TIM_SetCounter(TIM5, 0); // 计数器清空          				//计数器清空
                if (USART2_RX_STA == 0)  // 使能定时器5的中断
                {
					TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
                    TIM_Cmd(TIM5, ENABLE); // 使能定时器5
                }
                USART2_RX_BUF[USART2_RX_STA++] = res; // 记录接收到的值
            } else {
                USART2_RX_STA |= 1 << 15; // 强制标记接收完成
            }
        }
    }
}

// 初始化IO 串口2及用于判断超时的TIM5
// pclk1:PCLK1时钟频率(Mhz)
// bound:波特率
void USART2_init(u32 bound) {

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

	//PA2 TX PA3 RX
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 串口2时钟使能

    USART_DeInit(USART2); // 复位串口2
                          // USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    USART_InitStructure.USART_BaudRate = bound;                                     // 波特率一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式

    USART_Init(USART2, &USART_InitStructure); // 初始化串口	2

    USART_Cmd(USART2, ENABLE); // 使能串口

    // 使能接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启中断

    // 设置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           // 根据指定的参数初始化VIC寄存器

    TIM5_Int_Init(100 - 1, 7200 - 1); // TIM5 10ms中断初始化
	//TIM5_Int_Init(10000 - 1, 7200 - 1); // TIM5 10ms中断初始化 1ms for debug
    USART2_RX_STA = 0;                 // 清零
}

void Usart2_DeInit(void){
	//去初始化 对应init
	USART_DeInit(USART2);
	TIM_DeInit(TIM5);
}

//TIM5 Init
//On APB1 36Mhz
//for TIMx clk = APB1*2 = 72MHz
void TIM5_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//TIM5时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM5中断,允许更新中断
	TIM_Cmd(TIM5,DISABLE);//shut down定时器5

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
}
//TIM5 用于给串口2提供超时判断
//定时器5中断服务程序		    
void TIM5_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART2_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //清除TIM5更新中断标志
		//LED0(0);   //for Debug
		TIM_Cmd(TIM5, DISABLE);  //关闭TIM5
	}	    
}


// 串口2,printf 函数
//blocking 
// 确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char *fmt, ...) {
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART2_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART2_TX_BUF); // 此次发送数据的长度
    for (j = 0; j < i; j++)                  // 循环发送数据
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
            ; // 循环发送,直到发送完毕
        USART_SendData(USART2, USART2_TX_BUF[j]);
    }
}
