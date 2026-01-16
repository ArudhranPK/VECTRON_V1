#include "LIS2MDL.h"

HAL_StatusTypeDef LIS2MDLInit(LIS2MDL_Handle_t *lis)
{
    uint8_t tempBuff = 0;

    //	Verifying chip ID
    if (HAL_I2C_Mem_Read(lis->hi2c, LIS2MDL_I2C_ADDR, LIS2MDL_CHIP_ID_ADDR,
            I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;
    if (tempBuff != LIS2MDL_CHIP_ID)
        return HAL_ERROR;//	If Chip ID is not matching, then return HAL_ERROR

    // 	Reboot followed by Reset
    tempBuff = LIS2MDL_REBOOT;
    if (HAL_I2C_Mem_Write(lis->hi2c, LIS2MDL_I2C_ADDR, LIS2MDL_REBOOT_ADDR,
            I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(5);

    tempBuff = LIS2MDL_SOFT_RST;
    if (HAL_I2C_Mem_Write(lis->hi2c, LIS2MDL_I2C_ADDR, LIS2MDL_SOFT_RST_ADDR,
            I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(5);

    //	Writing configuration registers
    //	Load configuration register A
    tempBuff = LIS2MDL_CFG_REG_A;
    if (HAL_I2C_Mem_Write(lis->hi2c, LIS2MDL_I2C_ADDR, LIS2MDL_CFG_REG_A_ADDR,
            I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    //	Load configuration register B
    tempBuff = LIS2MDL_CFG_REG_B;
    if (HAL_I2C_Mem_Write(lis->hi2c, LIS2MDL_I2C_ADDR, LIS2MDL_CFG_REG_B_ADDR,
            I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    //	Load configuration register C
    tempBuff = LIS2MDL_CFG_REG_C;
    if (HAL_I2C_Mem_Write(lis->hi2c, LIS2MDL_I2C_ADDR, LIS2MDL_CFG_REG_C_ADDR,
            I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    //	Writing interrupt control registers : Turning interrupt off
    tempBuff = LIS2MDL_INT_CRTL_REG;
    if (HAL_I2C_Mem_Write(lis->hi2c, LIS2MDL_I2C_ADDR,
            LIS2MDL_INT_CRTL_REG_ADDR, I2C_MEMADD_SIZE_8BIT, &tempBuff, 1,
            HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef LIS2MDLReadRaw(LIS2MDL_Handle_t *lis)
{
    if (HAL_I2C_Mem_Read_DMA(lis->hi2c, LIS2MDL_I2C_ADDR,
            LIS2MDL_OUT_START_ADDR, I2C_MEMADD_SIZE_8BIT, lis->rawData, 6)
            != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

void LIS2MDLRawToData(LIS2MDL_Handle_t *lis)
{

    int16_t raw_x = ((uint16_t) lis->rawData[0]
            | (uint16_t) (lis->rawData[1] << 8));
    int16_t raw_y = ((uint16_t) lis->rawData[2]
            | (uint16_t) (lis->rawData[3] << 8));
    int16_t raw_z = ((uint16_t) lis->rawData[4]
            | (uint16_t) (lis->rawData[5] << 8));

    //  Sensitivity is 1.5 milliGauss per LSB
    lis->mag_x = (float) raw_x * 1.5e-3;
    lis->mag_y = (float) raw_y * 1.5e-3;
    lis->mag_z = (float) raw_z * 1.5e-3;
}

