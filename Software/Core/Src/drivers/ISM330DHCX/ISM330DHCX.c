#include "ISM330DHCX.h"

HAL_StatusTypeDef ISM330DHCXInit(ISM330DHCX_Handle_t *imu)
{
    uint8_t txTemp[2];
    uint8_t rxTemp[2];

    //cheking who am i

    txTemp[0] = ISM330DHCX_WHO_AM_I_REG_ADDR | 0x80;    //  0x80 for read mode
    txTemp[1] = 0;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_TransmitReceive(imu->hspi, txTemp, rxTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);

    if(rxTemp[1] != ISM330DHCX_WHO_AM_I_REG) return HAL_ERROR;


    // reseting the imu

    txTemp[0] = ISM330DHCX_RESET_VALUE_ADDR;
    txTemp[1] = ISM330DHCX_RESET_VALUE;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);

    HAL_Delay(100);

    // configuring registers

    txTemp[0] = ISM330DHCX_CTRL1_XL_ADDR;
    txTemp[1] = ISM330DHCX_CTRL1_XL;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);


    txTemp[0] = ISM330DHCX_CTRL2_G_ADDR;
    txTemp[1] = ISM330DHCX_CTRL2_G;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);


    txTemp[0] = ISM330DHCX_CTRL3_C_ADDR;
    txTemp[1] = ISM330DHCX_CTRL3_C;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);


    txTemp[0] = ISM330DHCX_CTRL4_C_ADDR;
    txTemp[1] = ISM330DHCX_CTRL4_C;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);


    txTemp[0] = ISM330DHCX_CTRL6_C_ADDR;
    txTemp[1] = ISM330DHCX_CTRL6_C;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);


    txTemp[0] = ISM330DHCX_CTRL8_XL_ADDR;
    txTemp[1] = ISM330DHCX_CTRL8_XL;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);


    txTemp[0] = ISM330DHCX_INT1_CTRL_ADDR;
    txTemp[1] = ISM330DHCX_INT1_CTRL;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);


    txTemp[0] = ISM330DHCX_INT2_CTRL_ADDR;
    txTemp[1] = ISM330DHCX_INT2_CTRL;

    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    if(HAL_SPI_Transmit(imu->hspi, txTemp, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_SET);

    imu->txBuffer[0] = ISM330DHCX_OUT_START_ADDR | 0x80;
    for(int i=1; i<15; i++) imu->txBuffer[i] = 0x00;

    return HAL_OK;
}

HAL_StatusTypeDef ISM330DHCXReadRaw(ISM330DHCX_Handle_t *imu)
{
    HAL_GPIO_WritePin(imu->CSPort, imu->CSPin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive_DMA(imu->hspi, imu->txBuffer, imu->rxBuffer, 15);

    return HAL_OK;
}

HAL_StatusTypeDef ISM330DHCXRawToData(ISM330DHCX_Handle_t *imu)
{
    int16_t temp_raw = (int16_t)(imu->rxBuffer[1] | (imu->rxBuffer[2] << 8));
    imu->temperature = (temp_raw / 256.0f) + 25.0f;

    float gyro_factor = 0.07f;
    imu->gyro_dps_x = (int16_t)(imu->rxBuffer[3] | (imu->rxBuffer[4] << 8)) * gyro_factor;
    imu->gyro_dps_y = (int16_t)(imu->rxBuffer[5] | (imu->rxBuffer[6] << 8)) * gyro_factor;
    imu->gyro_dps_z = (int16_t)(imu->rxBuffer[7] | (imu->rxBuffer[8] << 8)) * gyro_factor;

    float accel_factor = 0.000488f;
    imu->accel_g_x = (int16_t)(imu->rxBuffer[9]  | (imu->rxBuffer[10] << 8)) * accel_factor;
    imu->accel_g_y = (int16_t)(imu->rxBuffer[11] | (imu->rxBuffer[12] << 8)) * accel_factor;
    imu->accel_g_z = (int16_t)(imu->rxBuffer[13] | (imu->rxBuffer[14] << 8)) * accel_factor;

    return HAL_OK;
}
