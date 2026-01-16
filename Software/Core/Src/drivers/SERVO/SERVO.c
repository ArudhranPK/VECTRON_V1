#include "SERVO.h"

static uint16_t servo_angle_values[181];

void Servo_Init_LUT(void) {
    for (int i = 0; i <= 180; i++) {
        servo_angle_values[i] = SERVO_MIN_PULSE + ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * i) / 180;
    }
}

HAL_StatusTypeDef ServoInit(Servo_Handle_t *servo)
{
	if(HAL_TIM_PWM_Start(servo->htim, servo->TIM_CHANNEL) != HAL_OK) return HAL_ERROR;
	ServoSetAngle(servo, 0);
	return HAL_OK;
}

HAL_StatusTypeDef ServoSetAngle(Servo_Handle_t *servo, int16_t angle)
{
	if (angle > 180) angle = 180;
	servo->angle = angle;
	__HAL_TIM_SET_COMPARE(servo->htim, servo->TIM_CHANNEL, servo_angle_values[angle]);
	return HAL_OK;
}
