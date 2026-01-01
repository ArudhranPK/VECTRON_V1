//	Imports
#include "BMP280.h"


static HAL_StatusTypeDef readCalibrationData(BMP280_Handle_t *bmp)
{
	//	initiating buffer for calibration data
	uint8_t calibrationData[24];
	if(HAL_I2C_Mem_Read(bmp->hi2c, bmp->address, BMP280_CALIBRATION_START_ADDR, I2C_MEMADD_SIZE_8BIT, calibrationData, 24, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;


	//	Temperature compensation data
	bmp->dig_T1 = (((uint16_t)calibrationData[1] << 8) | (uint16_t)(calibrationData[0]));
	bmp->dig_T2 = (((int16_t)calibrationData[3] << 8) | (int16_t)(calibrationData[2]));
	bmp->dig_T3 = (((int16_t)calibrationData[5] << 8) | (int16_t)(calibrationData[4]));

	//	Pressure compensation data
	bmp->dig_P1 = ((uint16_t)(calibrationData[7] << 8) | (uint16_t)(calibrationData[6]));
	bmp->dig_P2 = ((int16_t)(calibrationData[9] << 8) | (int16_t)(calibrationData[8]));
	bmp->dig_P3 = ((int16_t)(calibrationData[11] << 8) | (int16_t)(calibrationData[10]));
	bmp->dig_P4 = ((int16_t)(calibrationData[13] << 8) | (int16_t)(calibrationData[12]));
	bmp->dig_P5 = ((int16_t)(calibrationData[15] << 8) | (int16_t)(calibrationData[14]));
	bmp->dig_P6 = ((int16_t)(calibrationData[17] << 8) | (int16_t)(calibrationData[16]));
	bmp->dig_P7 = ((int16_t)(calibrationData[19] << 8) | (int16_t)(calibrationData[18]));
	bmp->dig_P8 = ((int16_t)(calibrationData[21] << 8) | (int16_t)(calibrationData[20]));
	bmp->dig_P9 = ((int16_t)(calibrationData[23] << 8) | (int16_t)(calibrationData[22]));

	return HAL_OK;
}


//  Global Function declarations
HAL_StatusTypeDef BMP280Init(BMP280_Handle_t *bmp)
{
	uint8_t tempBuff = 0;

	//	Turning the 8 bit address to 7 bit address
	bmp->address = (bmp->address << 1);

	//	Reading chip ID. and verifying it's actually BMP280
	if(HAL_I2C_Mem_Read(bmp->hi2c, bmp->address, BMP280_CHIP_ID_ADDR, I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
	if(tempBuff != BMP280_CHIP_ID) return HAL_ERROR;	//	If Chip ID is not matching, then return HAL_ERROR


	//	Reseting sensors register
	tempBuff = BMP280_RESET_VALUE;
    if(HAL_I2C_Mem_Write(bmp->hi2c, bmp->address, BMP280_RESET_VALUE_ADDR, I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
	HAL_Delay(5);	//	Wait for the resetting to happen. Try without it


	//	BMP280 configuration registers like polling, over sampling, filter coefficient, different modes as well
	//	configuration of these should be done in the header file's macros

	//	1. Configuration which includes standby time, filter coefficient, SPI three wire mode enable
	tempBuff = BMP280_CONFIG;
    if(HAL_I2C_Mem_Write(bmp->hi2c, bmp->address, BMP280_CONFIG_ADDR, I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;


	//	2. Control which includes over sampling for temperature and pressure and mode of operation
	tempBuff = BMP280_CONTROL_MEASUREMENT;
    if(HAL_I2C_Mem_Write(bmp->hi2c, bmp->address, BMP280_CONTROL_MEASUREMENT_ADDR, I2C_MEMADD_SIZE_8BIT, &tempBuff, 1, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;


	//	Read calibration data
	if(readCalibrationData(bmp) != HAL_OK) return HAL_ERROR;


	//	If everything went well, return HAL_OK
	return HAL_OK;
}

HAL_StatusTypeDef BMP280ReadRaw(BMP280_Handle_t *bmp)
{
	//	Read raw ACD temperature and pressure data
	if(HAL_I2C_Mem_Read_DMA(bmp->hi2c, bmp->address, BMP280_DATA_START_ADDR, I2C_MEMADD_SIZE_8BIT, bmp->rawData, 6) != HAL_OK) return HAL_ERROR;

	return HAL_OK;
}

void BMP280RawToData(BMP280_Handle_t *bmp)
{
	uint32_t rawPressure;
	uint32_t rawTemprature;

	float var1;
	float var2;
	float t_fine;

	float finalTemperature;
	float finalPressure;

    rawPressure = ((int32_t)(bmp->rawData[0] << 12) | (int32_t)(bmp->rawData[1] << 4) | (int32_t)(bmp->rawData[2] >> 4));
    rawTemprature = ((int32_t)(bmp->rawData[3] << 12) | (int32_t)(bmp->rawData[4] << 4) | (int32_t)(bmp->rawData[5] >> 4));


    //  Calculate the temperature and pressure using the compensation values
    var1 = ((rawTemprature / 16384.0f) - ((bmp->dig_T1) / 1024.0f)) * (bmp->dig_T2);
    var2 = (((rawTemprature / 131072.0f) - ((bmp->dig_T1) / 8192.0f)) *
            ((rawTemprature / 131072.0f) - ((bmp->dig_T1) / 8192.0f))) * (bmp->dig_T3);
    t_fine = var1 + var2;
    finalTemperature = t_fine / 5120.0f;

    var1 = (t_fine / 2.0f) - 64000.0f;
    var2 = var1 * var1 * (bmp->dig_P6) / 32768.0f;
    var2 = var2 + var1 * (bmp->dig_P5) * 2.0f;
    var2 = (var2 / 4.0f) + ((bmp->dig_P4) * 65536.0f);
    var1 = ((bmp->dig_P3) * var1 * var1 / 524288.0f + (bmp->dig_P2) * var1) / 524288.0f;
    var1 = (1.0f + var1 / 32768.0f) * (bmp->dig_P1);

    if (var1 == 0.0f) 
    {
        // Assign "Error Values" so the main loop sees something is wrong
        bmp->pressure = 0.0f;
        bmp->temperature = 0.0f;
        
        // Exit the function immediately (void return)
        return; 
    }

    finalPressure = 1048576.0f - rawPressure;
    finalPressure = ((finalPressure - (var2 / 4096.0f)) * 6250.0f) / var1;
    var1 = (bmp->dig_P9) * finalPressure * finalPressure / 2147483648.0f;
    var2 = finalPressure * (bmp->dig_P8) / 32768.0f;
    finalPressure = finalPressure + (var1 + var2 + (bmp->dig_P7)) / 16.0f;


    //  Return the values
    bmp->temperature = finalTemperature;
    bmp->pressure = finalPressure;
}
