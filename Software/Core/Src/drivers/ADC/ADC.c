#include "ADC.h"

HAL_StatusTypeDef ADCInit(ADC_Handle_t *adc)
{
    if (adc == NULL) return HAL_ERROR;
    adc->multiplier = (VREF / MAX_ADC) * DIVIDER_RATIO;
    return HAL_OK;
}

HAL_StatusTypeDef ADCReadRaw(ADC_Handle_t *adc)
{
    HAL_ADC_Start_DMA(adc->hadc, (uint32_t*)adc->rawADCValue, 1);
    return HAL_OK;
}

HAL_StatusTypeDef ADCRawToData(ADC_Handle_t *adc)
{
    adc->voltage = (float)adc->rawADCValue * adc->multiplier;
    return HAL_OK;
}
