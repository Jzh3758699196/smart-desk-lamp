#include "stm32f10x.h"
#include "hc05.h"
#include "i2c.h"
#include "bh1750.h"
#include "pwm.h"

extern int target_lux;

int main(void)
{
	uint16_t pwm_duty = 0;
    uint32_t loop_count = 0;
	I2C1_Init();       // PB6/PB7 给 BH1750
    BH1750_Init();     // 传感器初始化
    HC05_Init();       // 蓝牙
    //LED_Init();
	PWM_Init();
    while (1)
    {
     HC05_Process();
		loop_count++;
        if (loop_count > 200000)
        {
            loop_count = 0;

            if (target_lux > 0)
            {
                float current_lux = BH1750_ReadLux();

                if (current_lux >= 0)
                {
                    if (current_lux < target_lux - 20) {
                        if (pwm_duty <= 99) pwm_duty += 1;
                        else pwm_duty = 100;
                        PWM_SetDuty(pwm_duty);
                    }
                    else if (current_lux > target_lux + 20) {
                        if (pwm_duty >= 1) pwm_duty -= 1;
                        else pwm_duty = 0;
                        PWM_SetDuty(pwm_duty);
                    }
                }
            }
        }
    }
}

