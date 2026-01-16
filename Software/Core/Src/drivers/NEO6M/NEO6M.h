#ifndef SRC_DRIVERS_NEO6M_NEO6M_H_
#define SRC_DRIVERS_NEO6M_NEO6M_H_

//	Imports
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <string.h>

typedef struct
{
    /* --- Hardware Resources --- */
    UART_HandleTypeDef *huart;    // Pointer to the UART handler (e.g., &huart2)

    /* --- Raw Buffers --- */
    uint8_t rawBuffer[512];
    uint8_t tempByte;
    uint8_t IsDataAvailable;

    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;
    // uint8_t Day;
    // uint8_t Month;
    // uint8_t Year;

    double Latitude;
    double Longitude;
    // char LatSide;
    // char LonSide;
    float Altitude;
    // float GeoidSep;

    // float SpeedKnots;
    // float SpeedKm;
    // float Course;

    uint8_t FixStatus;
    uint8_t SatellitesUsed;
    uint8_t FixMode;

    float HDOP;
    float PDOP;
    float VDOP;

} NEO6M_Handle_t;

//	Functions
HAL_StatusTypeDef NEO6MInit(NEO6M_Handle_t *neo);
HAL_StatusTypeDef NEO6MReadRaw(NEO6M_Handle_t *neo);
HAL_StatusTypeDef NEO6MRawToData(NEO6M_Handle_t *neo);

#endif /* SRC_DRIVERS_NEO6M_NEO6M_H_ */
