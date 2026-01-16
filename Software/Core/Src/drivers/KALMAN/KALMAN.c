#include "KALMAN.h"
#include <math.h>
#include <string.h>

// Helper function to limit dt to reasonable values
#define MAX_DT 1.0f
#define MIN_DT 0.001f

static float constrain_dt(float dt)
{
    if (dt > MAX_DT) return MAX_DT;
    if (dt < MIN_DT) return MIN_DT;
    return dt;
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================

void Kalman_Init(KalmanFilterState_t *kf)
{
    Kalman_InitAltitudeFilter(&kf->altitude_filter);
    Kalman_InitAttitudeFilter(&kf->attitude_filter);
    Kalman_InitTemperatureFilter(&kf->temp_filter);
}

void Kalman_InitAltitudeFilter(KalmanAltitudeFilter_t *filter)
{
    // Initialize state to zero
    filter->state[0] = 0.0f;  // altitude
    filter->state[1] = 0.0f;  // velocity_z
    
    // Initialize error covariance (high initial uncertainty)
    filter->P[0][0] = 100.0f;  // altitude variance
    filter->P[0][1] = 0.0f;
    filter->P[1][0] = 0.0f;
    filter->P[1][1] = 10.0f;   // velocity variance
    
    // Process noise covariance (tunable parameters)
    filter->Q[0][0] = 0.1f;    // altitude process noise
    filter->Q[0][1] = 0.0f;
    filter->Q[1][0] = 0.0f;
    filter->Q[1][1] = 0.5f;    // velocity process noise
    
    // Measurement noise
    filter->R_baro = 2.5f;     // barometer noise (meters)
    filter->R_gps = 5.0f;      // GPS altitude noise (meters)
    
    filter->last_timestamp_ms = 0;
    filter->initialized = false;
}

void Kalman_InitAttitudeFilter(KalmanAttitudeFilter_t *filter)
{
    // Initialize state to zero
    memset(filter->state, 0, sizeof(filter->state));
    memset(filter->gyro_bias, 0, sizeof(filter->gyro_bias));
    
    // Initialize error covariance (high initial uncertainty)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            filter->P[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    // Process noise covariance (tunable)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            filter->Q[i][j] = (i == j) ? 0.001f : 0.0f;
        }
    }
    
    // Measurement noise covariance (from magnetometer/accelerometer)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            filter->R[i][j] = (i == j) ? 0.1f : 0.0f;
        }
    }
    
    filter->last_timestamp_ms = 0;
    filter->initialized = false;
}

void Kalman_InitTemperatureFilter(TemperatureFilter_t *filter)
{
    filter->filtered_value = 0.0f;
    filter->alpha = 0.1f;  // Smoothing factor (lower = more smoothing)
    filter->initialized = false;
}

// ============================================================================
// ALTITUDE KALMAN FILTER
// ============================================================================

void Kalman_UpdateAltitude(KalmanAltitudeFilter_t *filter, 
                           float baro_altitude, 
                           float gps_altitude,
                           uint8_t gps_sats,
                           float accel_z,
                           uint32_t timestamp_ms)
{
    // First time initialization
    if (!filter->initialized)
    {
        filter->state[0] = baro_altitude;
        filter->state[1] = 0.0f;
        filter->last_timestamp_ms = timestamp_ms;
        filter->initialized = true;
        return;
    }
    
    // Calculate time delta
    float dt = (timestamp_ms - filter->last_timestamp_ms) / 1000.0f;
    dt = constrain_dt(dt);
    filter->last_timestamp_ms = timestamp_ms;
    
    // ========== PREDICTION STEP ==========
    
    // State transition matrix F
    float F[2][2] = {
        {1.0f, dt},
        {0.0f, 1.0f}
    };
    
    // Control input matrix B (for acceleration)
    float B[2] = {0.5f * dt * dt, dt};
    
    // Predict state: x = F*x + B*u (u = accel_z)
    float new_state[2];
    new_state[0] = F[0][0] * filter->state[0] + F[0][1] * filter->state[1] + B[0] * accel_z;
    new_state[1] = F[1][0] * filter->state[0] + F[1][1] * filter->state[1] + B[1] * accel_z;
    
    // Predict covariance: P = F*P*F' + Q
    float P_temp[2][2];
    
    // P_temp = F * P
    P_temp[0][0] = F[0][0] * filter->P[0][0] + F[0][1] * filter->P[1][0];
    P_temp[0][1] = F[0][0] * filter->P[0][1] + F[0][1] * filter->P[1][1];
    P_temp[1][0] = F[1][0] * filter->P[0][0] + F[1][1] * filter->P[1][0];
    P_temp[1][1] = F[1][0] * filter->P[0][1] + F[1][1] * filter->P[1][1];
    
    // P = P_temp * F' + Q
    filter->P[0][0] = P_temp[0][0] * F[0][0] + P_temp[0][1] * F[0][1] + filter->Q[0][0];
    filter->P[0][1] = P_temp[0][0] * F[1][0] + P_temp[0][1] * F[1][1] + filter->Q[0][1];
    filter->P[1][0] = P_temp[1][0] * F[0][0] + P_temp[1][1] * F[0][1] + filter->Q[1][0];
    filter->P[1][1] = P_temp[1][0] * F[1][0] + P_temp[1][1] * F[1][1] + filter->Q[1][1];
    
    // ========== UPDATE STEP (Barometer) ==========
    
    // Measurement model: H = [1, 0] (we measure altitude directly)
    float H[2] = {1.0f, 0.0f};
    
    // Innovation (measurement residual): y = z - H*x
    float innovation = baro_altitude - (H[0] * new_state[0] + H[1] * new_state[1]);
    
    // Innovation covariance: S = H*P*H' + R
    float S = H[0] * (filter->P[0][0] * H[0] + filter->P[0][1] * H[1]) +
              H[1] * (filter->P[1][0] * H[0] + filter->P[1][1] * H[1]) +
              filter->R_baro;
    
    // Kalman gain: K = P*H' / S
    float K[2];
    K[0] = (filter->P[0][0] * H[0] + filter->P[0][1] * H[1]) / S;
    K[1] = (filter->P[1][0] * H[0] + filter->P[1][1] * H[1]) / S;
    
    // Update state: x = x + K*y
    new_state[0] += K[0] * innovation;
    new_state[1] += K[1] * innovation;
    
    // Update covariance: P = (I - K*H) * P
    float KH[2][2];
    KH[0][0] = K[0] * H[0];
    KH[0][1] = K[0] * H[1];
    KH[1][0] = K[1] * H[0];
    KH[1][1] = K[1] * H[1];
    
    float I_KH[2][2];
    I_KH[0][0] = 1.0f - KH[0][0];
    I_KH[0][1] = -KH[0][1];
    I_KH[1][0] = -KH[1][0];
    I_KH[1][1] = 1.0f - KH[1][1];
    
    float P_new[2][2];
    P_new[0][0] = I_KH[0][0] * filter->P[0][0] + I_KH[0][1] * filter->P[1][0];
    P_new[0][1] = I_KH[0][0] * filter->P[0][1] + I_KH[0][1] * filter->P[1][1];
    P_new[1][0] = I_KH[1][0] * filter->P[0][0] + I_KH[1][1] * filter->P[1][0];
    P_new[1][1] = I_KH[1][0] * filter->P[0][1] + I_KH[1][1] * filter->P[1][1];
    
    memcpy(filter->P, P_new, sizeof(filter->P));
    
    // ========== GPS FUSION (if available) ==========
    
    // Use GPS altitude if we have good satellite lock (>= 5 satellites)
    if (gps_sats >= 5)
    {
        // Innovation for GPS
        float gps_innovation = gps_altitude - new_state[0];
        
        // Innovation covariance with GPS noise
        float S_gps = filter->P[0][0] + filter->R_gps;
        
        // GPS Kalman gain (only affects altitude state)
        float K_gps = filter->P[0][0] / S_gps;
        
        // Update altitude with GPS
        new_state[0] += K_gps * gps_innovation;
        
        // Update covariance
        filter->P[0][0] = (1.0f - K_gps) * filter->P[0][0];
    }
    
    // Store updated state
    filter->state[0] = new_state[0];
    filter->state[1] = new_state[1];
}

// ============================================================================
// ATTITUDE KALMAN FILTER (Complementary Filter Approach)
// ============================================================================

void Kalman_UpdateAttitude(KalmanAttitudeFilter_t *filter,
                           float measured_roll,
                           float measured_pitch,
                           float measured_yaw,
                           float gyro_x,
                           float gyro_y,
                           float gyro_z,
                           uint32_t timestamp_ms)
{
    // First time initialization
    if (!filter->initialized)
    {
        filter->state[0] = measured_roll;
        filter->state[1] = measured_pitch;
        filter->state[2] = measured_yaw;
        filter->last_timestamp_ms = timestamp_ms;
        filter->initialized = true;
        return;
    }
    
    // Calculate time delta
    float dt = (timestamp_ms - filter->last_timestamp_ms) / 1000.0f;
    dt = constrain_dt(dt);
    filter->last_timestamp_ms = timestamp_ms;
    
    // Convert gyro from deg/s to rad/s
    float gyro_rad[3] = {
        gyro_x * (M_PI / 180.0f),
        gyro_y * (M_PI / 180.0f),
        gyro_z * (M_PI / 180.0f)
    };
    
    // Subtract gyro bias
    gyro_rad[0] -= filter->gyro_bias[0];
    gyro_rad[1] -= filter->gyro_bias[1];
    gyro_rad[2] -= filter->gyro_bias[2];
    
    // ========== PREDICTION STEP ==========
    
    // Predict state using gyroscope (integration)
    float predicted[3];
    predicted[0] = filter->state[0] + gyro_rad[0] * dt;
    predicted[1] = filter->state[1] + gyro_rad[1] * dt;
    predicted[2] = filter->state[2] + gyro_rad[2] * dt;
    
    // Wrap angles to [-pi, pi]
    for (int i = 0; i < 3; i++)
    {
        while (predicted[i] > M_PI) predicted[i] -= 2.0f * M_PI;
        while (predicted[i] < -M_PI) predicted[i] += 2.0f * M_PI;
    }
    
    // Predict covariance: P = P + Q
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            filter->P[i][j] += filter->Q[i][j];
        }
    }
    
    // ========== UPDATE STEP ==========
    
    // Convert measured angles to radians
    float measured[3] = {
        measured_roll * (M_PI / 180.0f),
        measured_pitch * (M_PI / 180.0f),
        measured_yaw * (M_PI / 180.0f)
    };
    
    // For each axis (roll, pitch, yaw)
    for (int i = 0; i < 3; i++)
    {
        // Innovation (handle angle wrapping)
        float innovation = measured[i] - predicted[i];
        while (innovation > M_PI) innovation -= 2.0f * M_PI;
        while (innovation < -M_PI) innovation += 2.0f * M_PI;
        
        // Innovation covariance: S = P[i][i] + R[i][i]
        float S = filter->P[i][i] + filter->R[i][i];
        
        // Kalman gain: K = P[i][i] / S
        float K = filter->P[i][i] / S;
        
        // Update state
        predicted[i] += K * innovation;
        
        // Update covariance
        filter->P[i][i] = (1.0f - K) * filter->P[i][i];
        
        // Wrap updated angle
        while (predicted[i] > M_PI) predicted[i] -= 2.0f * M_PI;
        while (predicted[i] < -M_PI) predicted[i] += 2.0f * M_PI;
    }
    
    // Store updated state (convert back to degrees)
    filter->state[0] = predicted[0] * (180.0f / M_PI);
    filter->state[1] = predicted[1] * (180.0f / M_PI);
    filter->state[2] = predicted[2] * (180.0f / M_PI);
}

// ============================================================================
// TEMPERATURE FILTER (Simple Low-Pass)
// ============================================================================

float Kalman_UpdateTemperature(TemperatureFilter_t *filter,
                               float imu_temp,
                               float baro_temp)
{
    // Average the two temperature readings
    float avg_temp = (imu_temp + baro_temp) / 2.0f;
    
    if (!filter->initialized)
    {
        filter->filtered_value = avg_temp;
        filter->initialized = true;
        return filter->filtered_value;
    }
    
    // Low-pass filter: y = alpha * x + (1 - alpha) * y_prev
    filter->filtered_value = filter->alpha * avg_temp + 
                             (1.0f - filter->alpha) * filter->filtered_value;
    
    return filter->filtered_value;
}