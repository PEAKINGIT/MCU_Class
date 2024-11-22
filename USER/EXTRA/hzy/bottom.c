#include "bottom.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "usart.h" 

extern volatile uint32_t time_counter;  // ������һ��ȫ��ʱ���������ÿ�������

const uint32_t debounce_interval = 5;  // ����ʱ�䣬10����
const uint32_t long_press_duration = 700;  // ������ֵ��1000����

volatile uint32_t press_start_time_key0 = 0;
volatile uint32_t press_start_time_key1 = 0;
volatile uint32_t press_start_time_wkup = 0;

// ��ʼ������������Ϊ�ⲿ�ж�ģʽ
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ʹ��GPIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // ����KEY0��KEY1Ϊ��������ģʽ
    GPIO_InitStructure.GPIO_Pin = KEY0_PIN | KEY1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY0_PORT, &GPIO_InitStructure);

    // ����WK_UPΪ��������ģʽ
    GPIO_InitStructure.GPIO_Pin = WKUP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(WKUP_PORT, &GPIO_InitStructure);

    // ���� KEY0 ��Ӧ�� EXIT4 �ж�
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  // ??????
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // ���� KEY1 ��Ӧ�� EXIT3 �ж�
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_Init(&EXTI_InitStructure);

    // ����WK_UP ��Ӧ�� EXTI0 �ж�
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  // WK_UP����Ϊ�����ش���
    EXTI_Init(&EXTI_InitStructure);

    // �ж����ȼ�����
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

// �жϷ��������������¼�
void EXTI4_IRQHandler(void)  // KEY0 �������ж�
{
    if (EXTI_GetITStatus(EXTI_Line4) != RESET)
    { 
          if (GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN) == 0) // ��ⰴ������
        {
					 //printf("��������\r\n"); 
            // ����Ƿ񳬹�����ʱ��
            if ((time_counter - press_start_time_key0) >= debounce_interval)
            {
							//printf("�������£��Ѿ���������ʱ�䣩\r\n");
                press_start_time_key0 = time_counter;  // ��¼����ʱ��
            }
        }
        else  // ��⵽�����ͷ�
        {
					//printf("�����ͷ�\r\n");
            uint32_t press_duration = time_counter - press_start_time_key0;
            if (press_duration >= long_press_duration)
            {
                printf("KEY0 Long Press\r\n");  // �����¼�
            }
            else if (press_duration >= debounce_interval)
            {
                printf("KEY0 Short Press\r\n");  // �̰��¼�
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line4);  // ����жϱ�־
    }
}

void EXTI3_IRQHandler(void)  // KEY1 
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        if (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 0) 
        {
            if ((time_counter - press_start_time_key1) >= debounce_interval)
            {
                press_start_time_key1 = time_counter;  
            }
        }
        else  
        {
            uint32_t press_duration = time_counter - press_start_time_key1;
            if (press_duration >= long_press_duration)
            {
                printf("KEY1 Long Press\r\n");
            }
            else if (press_duration >= debounce_interval)
            {
                printf("KEY1 Short Press\r\n");
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

void EXTI0_IRQHandler(void)  // WK_UP
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        if (GPIO_ReadInputDataBit(WKUP_PORT, WKUP_PIN) == 1) 
        {
            if ((time_counter - press_start_time_wkup) >= debounce_interval)
            {
                press_start_time_wkup = time_counter; 
            }
        }
        else  
        {
            uint32_t press_duration = time_counter - press_start_time_wkup;
            if (press_duration >= long_press_duration)
            {
                printf("WK_UP Long Press\r\n");
            }
            else if (press_duration >= debounce_interval)
            {
                printf("WK_UP Short Press\r\n");
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

