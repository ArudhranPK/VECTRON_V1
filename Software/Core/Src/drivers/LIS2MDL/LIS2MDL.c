#include "LIS2MDL.h"


static HAL_StatusTypeDef readRegister(LIS2MDL_Handle_t *lis, uint16_t regAddr, uint8_t *dataPointer, uint16_t size)
{
	return HAL_I2C_Mem_Read(lis->hi2c, LIS2MDL_I2C_ADDR, regAddr, I2C_MEMADD_SIZE_8BIT, dataPointer, size, HAL_MAX_DELAY);
}


static HAL_StatusTypeDef writeRegister(LIS2MDL_Handle_t *lis, uint16_t regAddr, uint8_t *dataPointer, uint16_t size)
{
	return HAL_I2C_Mem_Write(lis->hi2c, LIS2MDL_I2C_ADDR, regAddr, I2C_MEMADD_SIZE_8BIT, dataPointer, size, HAL_MAX_DELAY);
}


HAL_StatusTypeDef LIS2MDLInit(LIS2MDL_Handle_t *lis)
{
	uint8_t tempBuff = 0;

	//	Verifying chip ID
	if(readRegister(lis, LIS2MDL_CHIP_ID_ADDR, &tempBuff, 1) != HAL_OK) return HAL_ERROR;
	if(tempBuff != LIS2MDL_CHIP_ID) return HAL_ERROR;	//	If Chip ID is not matching, then return HAL_ERROR


	// 	Reboot followed by Reset
	tempBuff = LIS2MDL_REBOOT;
	if(writeRegister(lis, LIS2MDL_REBOOT_ADDR, &tempBuff, 1) != HAL_OK) return HAL_ERROR;
	HAL_Delay(5);

	tempBuff = LIS2MDL_SOFT_RST;
	if(writeRegister(lis, LIS2MDL_SOFT_RST_ADDR, &tempBuff, 1) != HAL_OK) return HAL_ERROR;
	HAL_Delay(5);


	//	Writing configuration registers
	//	Load configuration register A
	tempBuff = LIS2MDL_CFG_REG_A;
	if(writeRegister(lis, LIS2MDL_CFG_REG_A_ADDR, &tempBuff, 1) != HAL_OK) return HAL_ERROR;

	//	Load configuration register B
	tempBuff = LIS2MDL_CFG_REG_B;
	if(writeRegister(lis, LIS2MDL_CFG_REG_B_ADDR, &tempBuff, 1) != HAL_OK) return HAL_ERROR;

	//	Load configuration register C
	tempBuff = LIS2MDL_CFG_REG_C;
	if(writeRegister(lis, LIS2MDL_CFG_REG_C_ADDR, &tempBuff, 1) != HAL_OK) return HAL_ERROR;


	//	Writing interrupt control registers : Turning interrupt off
	tempBuff = LIS2MDL_INT_CRTL_REG;
	if(writeRegister(lis, LIS2MDL_INT_CRTL_REG_ADDR, &tempBuff, 1) != HAL_OK) return HAL_ERROR;

	return HAL_OK;
}


HAL_StatusTypeDef LIS2MDLReadData(LIS2MDL_Handle_t *lis, float *mag_x, float *mag_y, float *mag_z, float *temperature)
{
	uint8_t rawData[8];

	if(readRegister(lis, LIS2MDL_OUT_START_ADDR, rawData, 8) != HAL_OK) return HAL_ERROR;

	int16_t raw_x = ((uint16_t)rawData[0] | (uint16_t)(rawData[1] << 8));
	int16_t raw_y = ((uint16_t)rawData[2] | (uint16_t)(rawData[3] << 8));
	int16_t raw_z = ((uint16_t)rawData[4] | (uint16_t)(rawData[5] << 8));
	int16_t raw_temp = ((uint16_t)rawData[6] | (uint16_t)(rawData[7] << 8));

	//	Sensitivity is 1.5 milliGauss per LSB
    *mag_x = (float)raw_x * 1.5e-3;
    *mag_y = (float)raw_y * 1.5e-3;
    *mag_z = (float)raw_z * 1.5e-3;

    *temperature = 25.0f + ((float)raw_temp) / 8.0f;

	return HAL_OK;
}
