#include "key.h"

extern volatile uint32_t globalTick; // 假设有一个全局时间计数器，每毫秒递增

const uint32_t debounce_interval = 5;     // 防抖时间，10毫秒
const uint32_t long_press_duration = 700; // 长按阈值，1000毫秒

volatile uint32_t press_start_time_key0 = 0;
volatile uint32_t press_start_time_key1 = 0;
volatile uint32_t press_start_time_wkup = 0;

u8 key_state = 0;
//key_state
//bit 7:6 WKUP:WKUP_Long
//bit 5:4 KEY0:KEY0_Long
//bit 3:2 KEY1:KEY1_Long

// 初始化按键并配置为外部中断模式
void KEY_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // 配置KEY0和KEY1为上拉输入模式
    GPIO_InitStructure.GPIO_Pin = KEY0_PIN | KEY1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY0_PORT, &GPIO_InitStructure);

    // 配置WK_UP为下拉输入模式
    GPIO_InitStructure.GPIO_Pin = WKUP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(WKUP_PORT, &GPIO_InitStructure);

    // 配置 KEY0 对应的 EXIT4 中断
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // ??????
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置 KEY1 对应的 EXIT3 中断
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_Init(&EXTI_InitStructure);

    // 配置WK_UP 对应的 EXTI0 中断
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // WK_UP按键为上升沿触发
    EXTI_Init(&EXTI_InitStructure);

    // 中断优先级配置
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

// 中断服务函数：处理按键事件
void EXTI4_IRQHandler(void) // KEY0 按键的中断
{
    if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
        if (GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN) == 0) // 检测按键按下
        {
            // printf("按键按下\r\n");
            // 检查是否超过防抖时间
            if ((globalTick - press_start_time_key0) >= debounce_interval) {
                // printf("按键按下（已经超过防抖时间）\r\n");
                press_start_time_key0 = globalTick; // 记录按下时刻
            }
        } else // 检测到按键释放
        {
            // printf("按键释放\r\n");
            uint32_t press_duration = globalTick - press_start_time_key0;
            if (press_duration >= long_press_duration) {
                key_state = KEY0_Long_Msk;
				//printf("KEY0 Long Press\r\n"); // 长按事件
            } else if (press_duration >= debounce_interval) {
                key_state = KEY0_Msk;
				//printf("KEY0 Short Press\r\n"); // 短按事件
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line4); // 清楚中断标志
    }
}

void EXTI3_IRQHandler(void) // KEY1
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
        if (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 0) {
            if ((globalTick - press_start_time_key1) >= debounce_interval) {
                press_start_time_key1 = globalTick;
            }
        } else {
            uint32_t press_duration = globalTick - press_start_time_key1;
            if (press_duration >= long_press_duration) {
                key_state = KEY1_Long_Msk;
				//printf("KEY1 Long Press\r\n");
            } else if (press_duration >= debounce_interval) {
                key_state = KEY1_Msk;
				//printf("KEY1 Short Press\r\n");
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

void EXTI0_IRQHandler(void) // WK_UP
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        if (GPIO_ReadInputDataBit(WKUP_PORT, WKUP_PIN) == 1) {
            if ((globalTick - press_start_time_wkup) >= debounce_interval) {
                press_start_time_wkup = globalTick;
            }
        } else {
            uint32_t press_duration = globalTick - press_start_time_wkup;
            if (press_duration >= long_press_duration) {
                key_state = WKUP_Long_Msk;
				//printf("WK_UP Long Press\r\n");
            } else if (press_duration >= debounce_interval) {
                key_state = WKUP_Msk;
				//printf("WK_UP Short Press\r\n");
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

//主循环调用 获取之前中断取得的键值
//priority:
//short>long 
//wkup>key0>key1
u8 KEY_Scan(u8 mode){
	//Short
	if(key_state&WKUP_Msk){ 
		key_state = 0;
		return WKUP_PRES;
	}
	if(key_state&KEY0_Msk){
		key_state = 0;
		return KEY0_PRES;
	} 
	if(key_state&KEY1_Msk){
		key_state = 0;
		return KEY1_PRES;
	}
	//Long
	if(key_state&WKUP_Long_Msk){ 
		key_state = 0;
		return WKUP_LONGP;
	}
	if(key_state&KEY0_Long_Msk){
		key_state = 0;
		return KEY0_LONGP;
	} 
	if(key_state&KEY1_Long_Msk){
		key_state = 0;
		return KEY1_LONGP;
	} 
	return 0;
}
