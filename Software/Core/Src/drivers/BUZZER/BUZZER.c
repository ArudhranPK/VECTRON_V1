#include "BUZZER.h"

HAL_StatusTypeDef BuzzerInit(Buzzer_Handle_t *buzzer)
{
    __HAL_TIM_SET_AUTORELOAD(buzzer->htim, NOTE_REST);
    HAL_TIMEx_OCN_Start(buzzer->htim, buzzer->TIM_CHANNEL);
}

HAL_StatusTypeDef BuzzerBeep(Buzzer_Handle_t *buzzer)
{
    __HAL_TIM_SET_AUTORELOAD(buzzer->htim, BEEP);
    HAL_Delay(100);
    __HAL_TIM_SET_AUTORELOAD(buzzer->htim, NOTE_REST);
}

HAL_StatusTypeDef BuzzerPlayNote(Buzzer_Handle_t *buzzer, uint16_t note)
{
    __HAL_TIM_SET_AUTORELOAD(buzzer->htim, note);
}
