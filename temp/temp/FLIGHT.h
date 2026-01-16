#ifndef SRC_FLIGHT_H_
#define SRC_FLIGHT_H_

#include "stm32f4xx_hal.h"

typedef enum
{

    STATE_GROUND_IDLE = 0,
    STATE_POWERED_FLIGHT,
    STATE_UNPOWERED_FLIGHT,
    STATE_BALLISTIC_DESCENT,
    STATE_CHUTE_DESCENT,
    STATE_LANDING_SAFE

} FlightState_t;

typedef struct __attribute__((packed))
{

    uint8_t start_byte;
    uint32_t timestamp_ms;

    uint32_t packet_count;
    uint8_t system_state;
    uint8_t flight_state;
    uint8_t error_flags;

    float accel_x;
    float accel_y;
    float accel_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;

    float pressure;

    float est_temperature;
    float est_altitude;
    float est_velocity_z;
    float est_pitch;
    float est_roll;
    float est_yaw;

    float battery_voltage;

    float servoAngleX1;
    float servoAngleX2;
    float servoAngleY1;
    float servoAngleY2;

    float servoRecov1;
    float servoRecov2;

    uint8_t gps_sats;
    int32_t gps_lat;
    int32_t gps_lon;
    int32_t gps_alt;

} FlightDataHandler_t;

HAL_StatusTypeDef flightComputerInit(FlightDataHandler_t *flight);
HAL_StatusTypeDef flightComputerCopySensors(FlightDataHandler_t *flight);
HAL_StatusTypeDef flightComputerEstimation(FlightDataHandler_t *flight);
#endif /* SRC_FLIGHT_H_ */
