// Import
#include "NEO6M.h"

static double NMEA_To_Decimal(double nmea_coord) {
    uint16_t degrees = (uint16_t)(nmea_coord / 100);
    double minutes = nmea_coord - (degrees * 100);
    return degrees + (minutes / 60.0);
}

/* Helper 2: Safe Float conversion that handles empty fields */
static float Get_Float(char *p) {
    if (p == NULL || *p == ',') return 0.0f;
    return strtof(p, NULL);
}

/* Helper 3: Safe Double conversion */
static double Get_Double(char *p) {
    if (p == NULL || *p == ',') return 0.0;
    return strtod(p, NULL);
}

HAL_StatusTypeDef NEO6MInit(NEO6M_Handle_t *neo)
{
    //  Change the baud rate from 9600 to 115200

    uint8_t setBaud115200[] = {
            0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00,
            0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00,
            0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7E,
          };

    HAL_UART_Transmit(neo->huart, setBaud115200, sizeof(setBaud115200), HAL_MAX_DELAY);
    HAL_Delay(100);

    neo->huart->Init.BaudRate = 115200;
    HAL_UART_Init(neo->huart);

	return HAL_OK;
}




HAL_StatusTypeDef NEO6MReadRaw(NEO6M_Handle_t *neo)
{
    if(HAL_UART_Receive_IT(neo->huart, &neo->tempByte, 1) != HAL_OK) return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef NEO6MRawToData(NEO6M_Handle_t *neo)
{
    if (neo == NULL) return HAL_ERROR;

    // Use the raw buffer where DMA stored the data
    char *buffer = (char *)neo->rawBuffer;

    // =========================================================
    // 1. Parse GPRMC (Best for Location, Speed, Date)
    // =========================================================
    char *rmc = strstr(buffer, "$GPRMC");

    if (rmc != NULL)
    {
        char *p = rmc;

        // Use a loop/macro approach or simple checks to move safely
        // 1. Time
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        float time = Get_Float(p);

        neo->Hour = (int)(time / 10000);
        neo->Min  = (int)((time - (neo->Hour * 10000)) / 100);
        neo->Sec  = (int)(time - (neo->Hour * 10000) - (neo->Min * 100));

        // 2. Status
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        if (*p == 'A') neo->FixStatus = 1;
        else neo->FixStatus = 0;

        // 3. Latitude
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        double lat_raw = Get_Double(p);

        // 4. N/S
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        neo->LatSide = *p;

        // 5. Longitude
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        double lon_raw = Get_Double(p);

        // 6. E/W
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        neo->LonSide = *p;

        // --- MATH ---
        neo->Latitude = NMEA_To_Decimal(lat_raw);
        if (neo->LatSide == 'S') neo->Latitude *= -1;

        neo->Longitude = NMEA_To_Decimal(lon_raw);
        if (neo->LonSide == 'W') neo->Longitude *= -1;

        // 7. Speed
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        neo->SpeedKnots = Get_Float(p);
        neo->SpeedKm = neo->SpeedKnots * 1.852f;

        // 8. Course
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        neo->Course = Get_Float(p);

        // 9. Date
        p = strchr(p, ','); if (p) p++; else return HAL_ERROR;
        int date = atoi(p);
        neo->Day = date / 10000;
        neo->Month = (date % 10000) / 100;
        neo->Year = date % 100;
    }

    // =========================================================
    // 2. Parse GPGGA (Best for Altitude & Satellites)
    // =========================================================
    char *gga = strstr(buffer, "$GPGGA");

    if (gga != NULL)
    {
        char *p = gga;

        // Skip first 6 commas safely
        for(int i=0; i<6; i++) {
            p = strchr(p, ',');
            if (p) p++;
            else break;
        }

        if (p != NULL)
        {
            // Fix Quality
            int quality = atoi(p);
            if(quality > 0) neo->FixStatus = quality;

            // Satellites
            p = strchr(p, ','); if (p) p++;
            neo->SatellitesUsed = atoi(p);

            // HDOP
            p = strchr(p, ','); if (p) p++;
            neo->HDOP = Get_Float(p);

            // Altitude
            p = strchr(p, ','); if (p) p++;
            neo->Altitude = Get_Float(p);
        }
    }

    return HAL_OK;
}


