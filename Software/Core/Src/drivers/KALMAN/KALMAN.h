#ifndef SRC_DRIVERS_KALMAN_KALMAN_H_
#define SRC_DRIVERS_KALMAN_KALMAN_H_

#include <stdint.h>
#include <stdbool.h>

// Kalman filter state for altitude and vertical velocity
typedef struct
{
    // State vector [altitude, velocity_z]
    float state[2];

    // Error covariance matrix (2x2)
    float P[2][2];

    // Process noise covariance
    float Q[2][2];

    // Measurement noise covariance
    float R_baro;
    float R_gps;

    // Time tracking
    uint32_t last_timestamp_ms;
    bool initialized;

} KalmanAltitudeFilter_t;

// Kalman filter state for attitude (roll, pitch, yaw)
typedef struct
{
    // State vector [roll, pitch, yaw]
    float state[3];

    // Error covariance matrix (3x3)
    float P[3][3];

    // Process noise covariance (3x3)
    float Q[3][3];

    // Measurement noise covariance (3x3)
    float R[3][3];

    // Gyro bias estimates
    float gyro_bias[3];

    // Time tracking
    uint32_t last_timestamp_ms;
    bool initialized;

} KalmanAttitudeFilter_t;

// Simple low-pass filter for temperature
typedef struct
{
    float filtered_value;
    float alpha;  // Smoothing factor (0 to 1)
    bool initialized;

} TemperatureFilter_t;

// Main Kalman filter structure
typedef struct
{
    KalmanAltitudeFilter_t altitude_filter;
    KalmanAttitudeFilter_t attitude_filter;
    TemperatureFilter_t temp_filter;

} KalmanFilterState_t;

// Function prototypes
void Kalman_Init(KalmanFilterState_t *kf);
void Kalman_InitAltitudeFilter(KalmanAltitudeFilter_t *filter);
void Kalman_InitAttitudeFilter(KalmanAttitudeFilter_t *filter);
void Kalman_InitTemperatureFilter(TemperatureFilter_t *filter);

void Kalman_UpdateAltitude(KalmanAltitudeFilter_t *filter,
                           float baro_altitude,
                           float gps_altitude,
                           uint8_t gps_sats,
                           float accel_z,
                           uint32_t timestamp_ms);

void Kalman_UpdateAttitude(KalmanAttitudeFilter_t *filter,
                           float measured_roll,
                           float measured_pitch,
                           float measured_yaw,
                           float gyro_x,
                           float gyro_y,
                           float gyro_z,
                           uint32_t timestamp_ms);

float Kalman_UpdateTemperature(TemperatureFilter_t *filter,
                               float imu_temp,
                               float baro_temp);



// Global Kalman filter instance (declare in your main file)
//extern KalmanFilterState_t g_kalman_state;

#endif /* SRC_DRIVERS_KALMAN_KALMAN_H_ */
