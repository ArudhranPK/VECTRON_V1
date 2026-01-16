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

    //  from imu
    float accel_x;
    float accel_y;
    float accel_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;

    float temp_imu;

    //  from barometer
    float pressure;
    float altitude;
    float temp_baro;

    //  from magentometer
    float roll;
    float pitch;
    float yaw;
    
    //  from 
    uint8_t gps_sats;
    double gps_lat;
    double gps_lon;
    float gps_alt;

    //  from ADC
    float battery_voltage;

    //  from kalman filter
    float est_temperature;
    float est_altitude;
    float est_velocity_z;
    float est_pitch;
    float est_roll;
    float est_yaw;

    //  from servo angle
    uint8_t servoAngleX1;
    uint8_t servoAngleX2;
    uint8_t servoAngleY1;
    uint8_t servoAngleY2;

    uint8_t servoRecov1;
    uint8_t servoRecov2;


} FlightDataHandler_t;

#endif /* SRC_FLIGHT_H_ */
