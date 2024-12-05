#include "usart3.h"

// ���ڽ��ջ�����
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; // ���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; // ���ͻ���,���USART3_MAX_SEND_LEN�ֽ�

// ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
// ���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
// �κ�����,���ʾ�˴ν������.
// ���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART3_RX_STA = 0;

void USART3_IRQHandler(void) {
    u8 res;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // ���յ�����
    {
        res = USART_ReceiveData(USART3);
        if ((USART3_RX_STA & (1 << 15)) == 0) // �������һ������,��û�б�����,���ٽ�����������
        {
            if (USART3_RX_STA < USART3_MAX_RECV_LEN) // �����Խ�������
            {
                TIM_SetCounter(TIM7, 0); // ���������          				//���������
                if (USART3_RX_STA == 0)  // ʹ�ܶ�ʱ��7���ж�
                {
                    TIM_Cmd(TIM7, ENABLE); // ʹ�ܶ�ʱ��7
                }
                USART3_RX_BUF[USART3_RX_STA++] = res; // ��¼���յ���ֵ
            } else {
                USART3_RX_STA |= 1 << 15; // ǿ�Ʊ�ǽ������
            }
        }
    }
}

// ��ʼ��IO ����3
// pclk1:PCLK1ʱ��Ƶ��(Mhz)
// bound:������
void usart3_init(u32 bound) {

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // ����3ʱ��ʹ��

    USART_DeInit(USART3); // ��λ����3
                          // USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);          // ��ʼ��PB10

    // USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);                // ��ʼ��PB11

    USART_InitStructure.USART_BaudRate = bound;                                     // ������һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // �ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ

    USART_Init(USART3, &USART_InitStructure); // ��ʼ������	3

    USART_Cmd(USART3, ENABLE); // ʹ�ܴ���

    // ʹ�ܽ����ж�
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // �����ж�

    // �����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

    TIM7_Int_Init(100 - 1, 7200 - 1); // 10ms�ж�
    USART3_RX_STA = 0;                 // ����
}

// ����3,printf ����
// ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char *fmt, ...) {
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART3_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART3_TX_BUF); // �˴η������ݵĳ���
    for (j = 0; j < i; j++)                  // ѭ����������
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
            ; // ѭ������,ֱ���������
        USART_SendData(USART3, USART3_TX_BUF[j]);
    }
}

// ��ʱ��7�жϷ������
void TIM7_IRQHandler(void) {
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) // �Ǹ����ж�
    {
        USART3_RX_STA |= 1 << 15;                   // ��ǽ������
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // ���TIM7�����жϱ�־
        TIM_Cmd(TIM7, DISABLE);                     // �ر�TIM7
    }
}

// ͨ�ö�ʱ��7�жϳ�ʼ��
// ����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
// ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=��ʱ������Ƶ��,��λ:Mhz
// ͨ�ö�ʱ���жϳ�ʼ��
// ����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
void TIM7_Int_Init(u16 arr, u16 psc) {
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); // TIM7ʱ��ʹ��

    // ��ʱ��TIM7��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr;                     // ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // ����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ

    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);             // ����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE); // ʹ��ָ����TIM7�ж�,���������ж�
    TIM_Cmd(TIM7, DISABLE); // ������ʱ��7

    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // �����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���
}