#ifndef SRC_DRIVERS_NEO6M_NEO6M_H_
#define SRC_DRIVERS_NEO6M_NEO6M_H_


//	Imports
#include "stm32f4xx_hal.h"


//	Constants


//	Type definitions
typedef struct {
	UART_HandleTypeDef huart;
}NEO6M_Handle_t;


//	Functions
HAL_StatusTypeDef NEO6MInit(NEO6M_Handle_t *neo);
HAL_StatusTypeDef NEO6MGetData(NEO6M_Handle_t *neo, float *latitude, float *longitude, float *altitude, int8_t sateliteNos);	//	Yet to add UTS time

#endif /* SRC_DRIVERS_NEO6M_NEO6M_H_ */
