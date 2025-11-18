#ifndef SRC_DRIVERS_BMP280_BMP280_H_
#define SRC_DRIVERS_BMP280_BMP280_H_


//	Imports
#include "stm32f4xx_hal.h"


// Constants
//#define BMP280_I2C_ADDR 		(0x76 << 1)		//	Can be changeable

#define BMP280_CHIP_ID 			0x58
#define BMP280_CHIP_ID_ADDR 	0xD0

#define BMP280_RESET_VALUE		0xB6
#define BMP280_RESET_VALUE_ADDR 0xE0

#define BMP280_TEMPRATURE_OVER_SAMPLING_1	0b001
#define BMP280_TEMPRATURE_OVER_SAMPLING_2	0b010
#define BMP280_TEMPRATURE_OVER_SAMPLING_4	0b011
#define BMP280_TEMPRATURE_OVER_SAMPLING_8	0b100
#define BMP280_TEMPRATURE_OVER_SAMPLING_16	0b101

#define BMP280_PRESSURE_OVER_SAMPLING_1		0b001
#define BMP280_PRESSURE_OVER_SAMPLING_2 	0b010
#define BMP280_PRESSURE_OVER_SAMPLING_4		0b011
#define BMP280_PRESSURE_OVER_SAMPLING_8		0b100
#define BMP280_PRESSURE_OVER_SAMPLING_16	0b101

#define BMP280_MODE_SLEEP		0b00
#define BMP280_MODE_FORCED		0b01
#define BMP280_MODE_NORMAL		0b11

#define BMP280_CONTROL_MEASUREMENT 			((BMP280_TEMPRATURE_OVER_SAMPLING_8 << 5) | (BMP280_PRESSURE_OVER_SAMPLING_8 << 2) | (BMP280_MODE_NORMAL))
#define BMP280_CONTROL_MEASUREMENT_ADDR		0xF4

#define BMP280_STANDBY_TIME_0S5		0b000
#define BMP280_STANDBY_TIME_62S5	0b001
#define BMP280_STANDBY_TIME_125		0b010
#define BMP280_STANDBY_TIME_250		0b011
#define BMP280_STANDBY_TIME_500		0b100
#define BMP280_STANDBY_TIME_1000	0b101
#define BMP280_STANDBY_TIME_2000	0b110
#define BMP280_STANDBY_TIME_4000	0b111

#define BMP280_FILTER_COEF_0		0b000
#define BMP280_FILTER_COEF_2		0b001
#define BMP280_FILTER_COEF_4		0b010
#define BMP280_FILTER_COEF_8		0b011
#define BMP280_FILTER_COEF_16		0b100

#define BMP280_SPI_THREE_WIRE_EN 	0

#define BMP280_CONFIG			((BMP280_STANDBY_TIME_0S5 << 5) | (BMP280_FILTER_COEF_4 << 2) | (BMP280_SPI_THREE_WIRE_EN))
#define BMP280_CONFIG_ADDR		0xF5

#define BMP280_DATA_START_ADDR			0xF7
#define BMP280_PRESSURE_START_ADDR		0xF7
#define BMP280_TEMPRATURE_START_ADDR	0xFA

#define BMP280_CALIBRATION_START_ADDR	0x88


//	Type definition
typedef struct {
    I2C_HandleTypeDef *hi2c; // pointer to I2C handle
    uint8_t address;         // sensor 8 bit I2C address. !!!Left shift is done at initialization

    //	Calibration data
    uint16_t dig_T1;
    int16_t  dig_T2;
	int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

} BMP280_Handle_t;


//	Functions
HAL_StatusTypeDef BMP280Init(BMP280_Handle_t *bmp);
HAL_StatusTypeDef BMP280ReadData(BMP280_Handle_t *bmp, float *pressure, float *temperature);


#endif /* SRC_DRIVERS_BMP280_BMP280_H_ */
