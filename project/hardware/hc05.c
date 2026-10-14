#include "hc05.h"
#include <string.h>   // strcmp, memset
#include "bh1750.h"
#include "pwm.h"
#define RX_BUFFER_SIZE 16

static uint8_t rx_data;
static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;//索引
static uint8_t rx_complete_flag = 0;//完成标志

//int target_lux = 0; 

void HC05_SendString(char *str)
{
    while (*str != '\0') {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *str++);
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

/* 串口1初始化：38400, 8N1, 开启接收中断 */
void HC05_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//结构体
    USART_InitTypeDef USART_InitStructure;//结构体
    NVIC_InitTypeDef NVIC_InitStructure;//结构体

    // 开时钟：GPIOA + USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // PA9: USART1_TX，复用推挽
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//赋予

    // PA10: USART1_RX，浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//配置

    // USART1 参数
    USART_InitStructure.USART_BaudRate = 38400;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字节
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//控制流
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//模式
    USART_Init(USART1, &USART_InitStructure);//配置

    // 开启接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//常开，与hal库不同

    // NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//指定要配置的中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//打开
    NVIC_Init(&NVIC_InitStructure);

    // 使能 USART1
    USART_Cmd(USART1, ENABLE);
}
int target_lux = 0;   // 0 = 手动模式，>0 = 闭环模式
void HC05_Process(void)
{
	
     int val = 0, i = 0, is_number = 1;

    if (rx_complete_flag != 1) return;
	//判断是否为数字
  while (rx_buffer[i] != '\0') {
        if (rx_buffer[i] < '0' || rx_buffer[i] > '9') {
            is_number = 0;
            break;
        }
        val = val * 10 + (rx_buffer[i] - '0');//字符转数字
        i++;
    }
  
    if (is_number && i > 0 && val <= 100) {
		target_lux = 0;
        PWM_SetDuty((uint16_t)val);      // 数字 → 设亮度
    }
    else if (strcmp((char*)rx_buffer, "ON") == 0) {
		target_lux = 0;
        PWM_SetDuty(100);
    }
    else if (strcmp((char*)rx_buffer, "OFF") == 0) {
		target_lux = 0;
        PWM_SetDuty(0);  // 熄灭
    }
	 else if (rx_buffer[0] == 'L' && rx_buffer[1] >= '0' && rx_buffer[1] <= '9') {
        i = 1;
        val = 0;
        while (rx_buffer[i] >= '0' && rx_buffer[i] <= '9') {
            val = val * 10 + (rx_buffer[i] - '0');
            i++;
        }
        if (val > 0 && val <= 20000) {
            target_lux = val;            // 进入闭环
        }
    }
    memset(rx_buffer, 0, RX_BUFFER_SIZE);//清空
    rx_index = 0;
    rx_complete_flag = 0;
}

/* 串口1中断服务函数 */
void USART1_IRQHandler(void)//中断后触发
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//检测是否是USART1导致的中断
    {
        rx_data = USART_ReceiveData(USART1);//读取
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清空

        if (rx_data == '@') {
            rx_buffer[rx_index] = '\0';
            rx_complete_flag = 1;
        }
        else {
            if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = rx_data;
            }
        }
    }
}


