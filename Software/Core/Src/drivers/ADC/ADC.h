#ifndef SRC_DRIVERS_ADC_ADC_H_
#define SRC_DRIVERS_ADC_ADC_H_

#include "stm32f4xx_hal.h"

#define VREF            3.3f
#define MAX_ADC         4095.0f
#define R1              3e5
#define R2              1e5
#define DIVIDER_RATIO   ((R1 + R2) / R2)

typedef struct{
    ADC_HandleTypeDef *hadc;
    uint16_t rawADCValue;
    float multiplier; 
    float voltage;
}ADC_Handle_t;


HAL_StatusTypeDef ADCInit(ADC_Handle_t *adc);
HAL_StatusTypeDef ADCReadRaw(ADC_Handle_t *adc);
HAL_StatusTypeDef ADCRawToData(ADC_Handle_t *adc);

#endif /* SRC_DRIVERS_ADC_ADC_H_ */
