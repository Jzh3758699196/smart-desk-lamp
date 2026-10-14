#include "pwm.h"

/* PA1 = TIM2_CH2，PWM 频率 1kHz */
void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//时基结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;//输出比较结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//AFIO：复用功能，PA1的复用就是TIM2_CH2(PWM)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能挂在总线上的外设时钟

    // PA1 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 系统时钟为72MHz，APB1的分频是2，故为36MHz，APB分频≠1时，定时器时钟=APB时钟×2，所以 TIM2 的实际时钟是 72MHz。
    TIM_TimeBaseStructure.TIM_Period = 999;//计数1000次更新一次，即1ms更新一次
    TIM_TimeBaseStructure.TIM_Prescaler = 71;//分频为72MHz/（72+1）=1MHz，即一微秒记一次数
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;//CKD设置，pwm输出时设什么都行
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//计数方式选择向上计数，即溢出时产生更新事件并归零
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);//写入

    // TIM2_CH2 PWM1 模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置输出比较模式为 PWM 模式 1。
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//CH2引脚(PA1)会真正输出 PWM 波形。
    TIM_OCInitStructure.TIM_Pulse = 0;//占空比=TIM_Pulse/(TIM_Period+1)，设初始占空比为0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//设占空比为高电平比例
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能 CCR2 的预装载，影响占空比。预装载：新值先存在，更新事件时启用

    TIM_ARRPreloadConfig(TIM2, ENABLE);//使能 ARR（自动重装载值）的预装载。影响频率/周期
    TIM_Cmd(TIM2, ENABLE);//启动定时器，计数器开始跑。
}

/* duty = 0 ~ 100 ，设置规定数值与占空比的关系*/
void PWM_SetDuty(uint16_t duty)
{
    if (duty > 100) duty = 100;
    TIM_SetCompare2(TIM2, duty * 10);   // ARR=999，100% → 1000
}
