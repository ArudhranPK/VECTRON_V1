#include "WS2812.h"

// Variable to track if DMA is currently sending data
static volatile uint8_t isSending = 0;

HAL_StatusTypeDef WS2812_Init(WS2812_Handle_t *led)
{
    // 1. Set the 24 bits of color data to "0" (Duty Cycle 33)
    // sending 0,0,0 color turns the LED off.
    for (int i = 0; i < 24; i++)
    {
        led->pwmData[i] = WS2812_DUTY_0;
    }

    // 2. Set the 50 reset bits to absolute 0 (0% Duty Cycle)
    // This holds the line LOW to reset the strip
    for (int i = 24; i < (24 + 50); i++)
    {
        led->pwmData[i] = 0;
    }

    return HAL_OK;
}

void WS2812_SetColor(WS2812_Handle_t *led, uint32_t color)
{
    // 1. Extract RGB from uint32_t (Format: 0x00RRGGBB)
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    // 3. Start index for the first (and only) LED
    uint32_t pos = 0;

    // 4. Fill Buffer: WS2812 expects data in GRB order (Green-Red-Blue)

    // GREEN (8 bits)
    for (int i = 7; i >= 0; i--)
    {
        led->pwmData[pos++] = (g & (1 << i)) ? WS2812_DUTY_1 : WS2812_DUTY_0;
    }

    // RED (8 bits)
    for (int i = 7; i >= 0; i--)
    {
        led->pwmData[pos++] = (r & (1 << i)) ? WS2812_DUTY_1 : WS2812_DUTY_0;
    }

    // BLUE (8 bits)
    for (int i = 7; i >= 0; i--)
    {
        led->pwmData[pos++] = (b & (1 << i)) ? WS2812_DUTY_1 : WS2812_DUTY_0;
    }
}

HAL_StatusTypeDef WS2812_Send(WS2812_Handle_t *led)
{
    if (isSending)
        return HAL_BUSY;

    isSending = 1; // Lock to prevent overlapping sends

    // Total length: 24 bits for color + 50 bits for reset
    uint32_t totalLen = 24 + 50;

    // 1. Start DMA. Note: We cast pwmData to uint32_t* as per HAL requirement
    if (HAL_TIM_PWM_Start_DMA(led->htim, led->TIM_CHANNEL, (uint32_t *)led->pwmData, totalLen) != HAL_OK)
    {
        isSending = 0;
        return HAL_ERROR;
    }

    // 2. IMPORTANT: Enable the N-Channel (CH3N) output
    // Standard HAL_TIM_PWM_Start only enables the regular CH3
    HAL_TIMEx_PWMN_Start(led->htim, led->TIM_CHANNEL);

    return HAL_OK;
}

// Global Callback: Stops the timer when DMA is done
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    // Check if this is the timer we are using (TIM1)
    if (htim->Instance == TIM8)
    {
        // Stop DMA and PWM generation
        HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_3);

        // Stop the N-Channel explicitly
        HAL_TIMEx_PWMN_Stop(htim, TIM_CHANNEL_3);

        isSending = 0; // Release lock
    }
}
