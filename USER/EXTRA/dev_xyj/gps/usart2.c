#include "./gps/usart2.h"

// ���ڽ��ջ�����
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; // ���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; // ���ͻ���,���USART2_MAX_SEND_LEN�ֽ�

// ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
// ���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�

// �κ�����,���ʾ�˴ν������.
// ���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART2_RX_STA = 0;

void USART2_IRQHandler(void) {
    u8 res;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // ���յ�����
    {
        res = USART_ReceiveData(USART2);
		LED0(1);
        if ((USART2_RX_STA & (1 << 15)) == 0) // �������һ������,��û�б�����,���ٽ�����������
        {
            if (USART2_RX_STA < USART2_MAX_RECV_LEN) // �����Խ�������
            {
                TIM_SetCounter(TIM5, 0); // ���������          				//���������
                if (USART2_RX_STA == 0)  // ʹ�ܶ�ʱ��5���ж�
                {
					TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
                    TIM_Cmd(TIM5, ENABLE); // ʹ�ܶ�ʱ��5
                }
                USART2_RX_BUF[USART2_RX_STA++] = res; // ��¼���յ���ֵ
            } else {
                USART2_RX_STA |= 1 << 15; // ǿ�Ʊ�ǽ������
            }
        }
    }
}

// ��ʼ��IO ����2�������жϳ�ʱ��TIM5
// pclk1:PCLK1ʱ��Ƶ��(Mhz)
// bound:������
void USART2_init(u32 bound) {

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

	//PA2 TX PA3 RX
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // ����2ʱ��ʹ��

    USART_DeInit(USART2); // ��λ����2
                          // USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    USART_InitStructure.USART_BaudRate = bound;                                     // ������һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // �ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); // ��ʼ������	2

    USART_Cmd(USART2, ENABLE); // ʹ�ܴ���

    // ʹ�ܽ����ж�
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // �����ж�

    // �����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

    TIM5_Int_Init(100 - 1, 7200 - 1); // TIM5 10ms�жϳ�ʼ��
	//TIM5_Int_Init(10000 - 1, 7200 - 1); // TIM5 10ms�жϳ�ʼ�� 1ms for debug
    USART2_RX_STA = 0;                 // ����
}

//TIM5 Init
//On APB1 36Mhz
//for TIMx clk = APB1*2 = 72MHz
void TIM5_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//TIM5ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM5�ж�,��������ж�
	TIM_Cmd(TIM5,DISABLE);//shut down��ʱ��5

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
}
//TIM5 ���ڸ�����2�ṩ��ʱ�ж�
//��ʱ��5�жϷ������		    
void TIM5_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART2_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //���TIM5�����жϱ�־
		//LED0(0);   //for Debug
		TIM_Cmd(TIM5, DISABLE);  //�ر�TIM5
	}	    
}


// ����2,printf ����
//blocking 
// ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char *fmt, ...) {
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART2_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART2_TX_BUF); // �˴η������ݵĳ���
    for (j = 0; j < i; j++)                  // ѭ����������
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
            ; // ѭ������,ֱ���������
        USART_SendData(USART2, USART2_TX_BUF[j]);
    }
}
