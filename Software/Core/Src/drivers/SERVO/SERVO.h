#ifndef SRC_SERVO_H_
#define SRC_SERVO_H_

//	Imports
#include <stm32f4xx_hal.h>


//	Constants
#define SERVO_MIN_PULSE 500
#define SERVO_MAX_PULSE 2500


typedef struct {
	TIM_HandleTypeDef *htim; 	// pointer to Timer handle
    uint32_t TIM_CHANNEL;  		// Channel of the timer to which PWM is configured
} Servo_Handle_t;


//Functions initialization
void Servo_Init_LUT();
HAL_StatusTypeDef ServoInit(Servo_Handle_t *servo);
HAL_StatusTypeDef ServoSetAngle(Servo_Handle_t *servo, int16_t angle);

#endif /* SRC_SERVO_H_ */
