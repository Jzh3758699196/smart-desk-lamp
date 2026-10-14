#include "bh1750.h"

/* 7位地址0x23，左移一位变0x46 */
#define BH1750_ADDR    (0x23 << 1)

/* 写一个字节到 BH1750 */
static void BH1750_WriteByte(uint8_t cmd)
{
    // 发送 START
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//检查事件是否发生（进入主机模式）

    I2C_Send7bitAddress(I2C1, BH1750_ADDR, I2C_Direction_Transmitter);//发送地址
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // 发送命令字节
    I2C_SendData(I2C1, cmd);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//等待发完

    // 发送 STOP
    I2C_GenerateSTOP(I2C1, ENABLE);
}

/* 读取 2 个字节，返回合并后的原始值 */
static uint16_t BH1750_ReadRaw(void)
{
    uint8_t dataH, dataL;

    I2C_GenerateSTART(I2C1, ENABLE);// 发送 START
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, BH1750_ADDR, I2C_Direction_Receiver);   // 发送设备地址 + 读方向
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));    // 读高字节（应答）
    dataH = I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, DISABLE);    // 读低字节前，先关 ACK，表示最后一个字节

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));    // 读低字节
    dataL = I2C_ReceiveData(I2C1);

    I2C_GenerateSTOP(I2C1, ENABLE);//发stop

    // 恢复 ACK，准备下一次通信
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return ((uint16_t)dataH << 8) | dataL;
}

/* 初始化：上电 + 连续高分辨率模式 */
void BH1750_Init(void)
{
    BH1750_WriteByte(0x01);    // 1. 上电

    BH1750_WriteByte(0x10);    // 2. 连续高分辨率模式，1lux 精度，约 120ms 采样一次
}

float BH1750_ReadLux(void)
{
    uint16_t raw = BH1750_ReadRaw();
    return raw / 1.2f;//读光照强度，单位 lux
}