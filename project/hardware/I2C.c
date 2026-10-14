#include"i2c.h"

void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//结构体
    I2C_InitTypeDef  I2C_InitStructure;//结构体

    // 1. 开时钟：GPIOB + I2C1 + AFIO（重映射要用到）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // 2. PB6(SCL) 和 PB7(SDA) 配成复用开漏
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;    // 复用开漏，I2C必须用开漏
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 3. I2C1 参数
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//标准i2c模式
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//占空比
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;          // 主机地址随便填
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//接收数据时自动回ACK(应答)（需手动关闭ACK）
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//BH1750 就是 7 位地址（0x23）
    I2C_InitStructure.I2C_ClockSpeed = 100000;         // 100kHz
    I2C_Init(I2C1, &I2C_InitStructure);

    // 4. 使能 I2C1
    I2C_Cmd(I2C1, ENABLE);
}