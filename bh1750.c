#include "bh1750.h"
void delay_ms(uint32_t ms) {
    SysTick_Config(SystemCoreClock / 1000);
    while(ms--) {
        while(!((SysTick->CTRL) & (1 << 16)));
    }
    SysTick->CTRL = 0;
}


static void BH1750_WriteCmd(uint8_t cmd) {
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, BH1750_ADDR, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, cmd);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

void BH1750_Init(void) {
    BH1750_WriteCmd(BH1750_POWER_ON);
    BH1750_WriteCmd(BH1750_RESET);
    BH1750_WriteCmd(BH1750_POWER_DOWN); 
}

float BH1750_ReadLux_OneTime(void) {
    uint16_t val = 0;

    // b?t ngu?n và g?i l?nh do 1 l?n
    BH1750_WriteCmd(BH1750_POWER_ON);
    BH1750_WriteCmd(ONE_TIME_HIGH_RES_MODE);

    // d?i c?m bi?n do (~180ms)
    delay_ms(180);

    // b?t d?u d?c d? li?u
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, BH1750_ADDR, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    val = I2C_ReceiveData(I2C1) << 8;

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    val |= I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    BH1750_WriteCmd(BH1750_POWER_DOWN);

    return (float)val / 1.2f;
}
