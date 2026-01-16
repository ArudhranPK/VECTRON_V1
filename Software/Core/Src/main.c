/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "FLIGHT.h"

#include "drivers/ISM330DHCX/ISM330DHCX.h"
#include "drivers/BMP280/BMP280.h"
#include "drivers/LIS2MDL/LIS2MDL.h"
#include "drivers/NEO6M/NEO6M.h"
#include "drivers/ADC/ADC.h"

#include "drivers/SERVO/SERVO.h"
#include "drivers/BUZZER/BUZZER.h"
#include "drivers/WS2812/WS2812.h"

#include "drivers/W25Q128JV/W25Q128JV.h"
#include "drivers/MICROSD/MICROSD.h"

#include "drivers/DEBUGGING/DEBUGGING.h"

#include "drivers/KALMAN/KALMAN.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim8_ch3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
uint8_t estimationTimes = 0;
uint8_t estimationFlag = 0;

FlightDataHandler_t FlightData;

typedef enum
{
    STATE_READING_BMP,
    STATE_READING_LIS
} I2CSequenceState_t;

volatile I2CSequenceState_t I2CdmaState = STATE_READING_BMP;

ISM330DHCX_Handle_t ISM330DHCX_Handler;
BMP280_Handle_t BMP280_Handler;
LIS2MDL_Handle_t LIS2MDL_Handler;
NEO6M_Handle_t NEO6M_Handler;
ADC_Handle_t ADC_Handler;

Servo_Handle_t servoX1;
Servo_Handle_t servoX2;
Servo_Handle_t servoY1;
Servo_Handle_t servoY2;

Servo_Handle_t servoRecov1;
Servo_Handle_t servoRecov2;

Buzzer_Handle_t Buzzer_Handler;
WS2812_Handle_t WS2812_Handler;

W25Q128JV_Handle_t W25Q128JV_Handler;

KalmanFilterState_t g_kalman_state = {0};

char output[1024];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM8_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

HAL_StatusTypeDef flightComputerInit(FlightDataHandler_t *flight)
{
    // Initialize sensors
    ISM330DHCXInit(&ISM330DHCX_Handler);
    BMP280Init(&BMP280_Handler);
    LIS2MDLInit(&LIS2MDL_Handler);
    NEO6MInit(&NEO6M_Handler);
    ADCInit(&ADC_Handler);
    // Start timer for timestamp
    HAL_TIM_Base_Start_IT(&htim5);

    // Initialize servos
    Servo_Init_LUT();
    ServoInit(&servoX1);
    ServoInit(&servoX2);
    ServoInit(&servoY1);
    ServoInit(&servoY2);
    ServoInit(&servoRecov1);
    ServoInit(&servoRecov2);

    // Initialize other peripherals
    BuzzerInit(&Buzzer_Handler);
    WS2812_Init(&WS2812_Handler);

    //  Initialize data storage elements
    W25Q128JVInit(&W25Q128JV_Handler);
//    W25Q128JVCleanSlate(&W25Q128JV_Handler);

    // Initial sensor readings
    NEO6MReadRaw(&NEO6M_Handler);
    BMP280ReadRaw(&BMP280_Handler);
    ISM330DHCXReadRaw(&ISM330DHCX_Handler);

    // Initialize flight data structure
    flight->start_byte = 0xAA; // Example start byte
    flight->packet_count = 0;
    flight->system_state = 0;
    flight->flight_state = 0;
    flight->error_flags = 0;

    return HAL_OK;
}

HAL_StatusTypeDef flightComputerCopySensors(FlightDataHandler_t *flight)
{
    // Copy timestamp
    flight->timestamp_ms = HAL_GetTick();

    // Copy IMU data (accelerometer)
    flight->accel_x = ISM330DHCX_Handler.accel_g_x;
    flight->accel_y = ISM330DHCX_Handler.accel_g_y;
    flight->accel_z = ISM330DHCX_Handler.accel_g_z;

    // Copy IMU data (gyroscope)
    flight->gyro_x = ISM330DHCX_Handler.gyro_dps_x;
    flight->gyro_y = ISM330DHCX_Handler.gyro_dps_y;
    flight->gyro_z = ISM330DHCX_Handler.gyro_dps_z;

    // Copy IMU temperature
    flight->temp_imu = ISM330DHCX_Handler.temperature;

    // Copy barometer data
    flight->pressure = BMP280_Handler.pressure;
    flight->altitude = BMP280_Handler.altitude;
    flight->temp_baro = BMP280_Handler.temperature;

    // Copy magnetometer/attitude data
    flight->roll = LIS2MDL_Handler.mag_x;
    flight->pitch = LIS2MDL_Handler.mag_y;
    flight->yaw = LIS2MDL_Handler.mag_z;

    // Copy GPS data
    flight->gps_sats = NEO6M_Handler.SatellitesUsed;
    flight->gps_lat = NEO6M_Handler.Latitude;
    flight->gps_lon = NEO6M_Handler.Longitude;
    flight->gps_alt = NEO6M_Handler.Altitude;

    // Copy ADC voltage
    flight->battery_voltage = ADC_Handler.voltage;

    // Copy servo positions
    flight->servoAngleX1 = servoX1.angle;
    flight->servoAngleX2 = servoX2.angle;
    flight->servoAngleY1 = servoY1.angle;
    flight->servoAngleY2 = servoY2.angle;
    flight->servoRecov1 = servoRecov1.angle;
    flight->servoRecov2 = servoRecov2.angle;

    // Increment packet counter
    flight->packet_count++;

    return HAL_OK;
}

HAL_StatusTypeDef flightComputerEstimation(FlightDataHandler_t *flight)
{
    // Initialize Kalman filters on first call
    static bool first_call = true;
    if (first_call)
    {
        Kalman_Init(&g_kalman_state);
        first_call = false;
    }

    // Update altitude and velocity estimation
    Kalman_UpdateAltitude(&g_kalman_state.altitude_filter,
                          flight->altitude,
                          flight->gps_alt,
                          flight->gps_sats,
                          flight->accel_z,
                          flight->timestamp_ms);

    // Update attitude estimation
    Kalman_UpdateAttitude(&g_kalman_state.attitude_filter,
                          flight->roll,
                          flight->pitch,
                          flight->yaw,
                          flight->gyro_x,
                          flight->gyro_y,
                          flight->gyro_z,
                          flight->timestamp_ms);

    // Update temperature estimation
    flight->est_temperature = Kalman_UpdateTemperature(&g_kalman_state.temp_filter,
                                                       flight->temp_imu,
                                                       flight->temp_baro);

    // Copy estimated values to flight data structure
    flight->est_altitude = g_kalman_state.altitude_filter.state[0];
    flight->est_velocity_z = g_kalman_state.altitude_filter.state[1];
    flight->est_roll = g_kalman_state.attitude_filter.state[0];
    flight->est_pitch = g_kalman_state.attitude_filter.state[1];
    flight->est_yaw = g_kalman_state.attitude_filter.state[2];

    return HAL_OK;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */
    ISM330DHCX_Handler.hspi = &hspi1;
    ISM330DHCX_Handler.CSPort = IMU_CS_GPIO_Port;
    ISM330DHCX_Handler.CSPin = IMU_CS_Pin;

    BMP280_Handler.hi2c = &hi2c1;
    BMP280_Handler.address = 0x76;

    LIS2MDL_Handler.hi2c = &hi2c1;

    NEO6M_Handler.huart = &huart2;

    ADC_Handler.hadc = &hadc1;

    servoX1.htim = &htim2;
    servoX1.TIM_CHANNEL = TIM_CHANNEL_1;
    servoX2.htim = &htim2;
    servoX2.TIM_CHANNEL = TIM_CHANNEL_2;
    servoY1.htim = &htim2;
    servoY1.TIM_CHANNEL = TIM_CHANNEL_3;
    servoY2.htim = &htim2;
    servoY2.TIM_CHANNEL = TIM_CHANNEL_4;

    servoRecov1.htim = &htim3;
    servoRecov1.TIM_CHANNEL = TIM_CHANNEL_1;
    servoRecov2.htim = &htim3;
    servoRecov2.TIM_CHANNEL = TIM_CHANNEL_2;

    Buzzer_Handler.htim = &htim1;
    Buzzer_Handler.TIM_CHANNEL = TIM_CHANNEL_2;

    WS2812_Handler.htim = &htim8;
    WS2812_Handler.TIM_CHANNEL = TIM_CHANNEL_3;
    WS2812_Handler.brightness = 255;

    W25Q128JV_Handler.hspi = &hspi2;
    W25Q128JV_Handler.CsPort = FLASH_CS_GPIO_Port;
    W25Q128JV_Handler.CsPin = FLASH_CS_Pin;
    W25Q128JV_Handler.currentOffset = 0;

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_I2C1_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM5_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_SDIO_SD_Init();
    MX_SPI1_Init();
    MX_SPI2_Init();
    MX_FATFS_Init();
    MX_USB_DEVICE_Init();
    MX_TIM8_Init();
    /* USER CODE BEGIN 2 */

    ///////////////////////////////////////////////////////////////////
    // Init function
    flightComputerInit(&FlightData);

    BuzzerPlayNote(&Buzzer_Handler, NOTE_G5);
    HAL_Delay(100);
    BuzzerPlayNote(&Buzzer_Handler, NOTE_B5);
    HAL_Delay(100);
    BuzzerPlayNote(&Buzzer_Handler, NOTE_D6);
    HAL_Delay(100);
    BuzzerPlayNote(&Buzzer_Handler, NOTE_G6);
    HAL_Delay(100);
    BuzzerPlayNote(&Buzzer_Handler, NOTE_B6);
    HAL_Delay(100);
    BuzzerPlayNote(&Buzzer_Handler, NOTE_DS7);
    HAL_Delay(80);
    BuzzerPlayNote(&Buzzer_Handler, NOTE_E7);
    HAL_Delay(400);
    BuzzerPlayNote(&Buzzer_Handler, NOTE_REST);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        if (estimationFlag)
        {

            snprintf(output, sizeof(output),
                     "--- Packet: %lu ---\n"
                     "Time: %lu ms | Sys: %u | Flight: %u | Err: 0x%02X\n"
                     "IMU Acc: [%.2f, %.2f, %.2f]\n"
                     "IMU Gyr: [%.2f, %.2f, %.2f]\n"
                     "IMU Temp: %.2f\n"
                     "Baro: P:%.2f Alt:%.2f T:%.2f\n"
                     "Mag (RPY): %.2f, %.2f, %.2f\n"
                     "GPS: Sats:%u Lat:%.7f Lon:%.7f Alt:%.2f\n"
                     "KF Est: Alt:%.2f Vel:%.2f RPY:[%.2f, %.2f, %.2f] T:%.2f\n"
                     "Batt: %.2f V\n"
                     "Servos: X[%u, %u] Y[%u, %u] Rec[%u, %u]\n\n",

                     // Header
                     FlightData.packet_count,
                     FlightData.timestamp_ms,
                     FlightData.system_state,
                     FlightData.flight_state,
                     FlightData.error_flags,

                     // IMU
                     FlightData.accel_x, FlightData.accel_y, FlightData.accel_z,
                     FlightData.gyro_x, FlightData.gyro_y, FlightData.gyro_z,
                     FlightData.temp_imu,

                     // Barometer
                     FlightData.pressure,
                     FlightData.altitude,
                     FlightData.temp_baro,

                     // Magnetometer / Orientation
                     FlightData.roll, FlightData.pitch, FlightData.yaw,

                     // GPS (Notice %.7f for Lat/Lon)
                     FlightData.gps_sats,
                     FlightData.gps_lat,
                     FlightData.gps_lon,
                     FlightData.gps_alt,

                     // Kalman Filter Estimates
                     FlightData.est_altitude,
                     FlightData.est_velocity_z,
                     FlightData.est_roll, FlightData.est_pitch, FlightData.est_yaw,
                     FlightData.est_temperature,

                     // Battery
                     FlightData.battery_voltage,

                     // Servos
                     FlightData.servoAngleX1, FlightData.servoAngleX2,
                     FlightData.servoAngleY1, FlightData.servoAngleY2,
                     FlightData.servoRecov1, FlightData.servoRecov2);

            debugging(output);
            ///////////////////////////////////////////////////////////////////
            //    copying function of flight data form various sensors
            flightComputerCopySensors(&FlightData);

            ///////////////////////////////////////////////////////////////////
            //    estimation function
            flightComputerEstimation(&FlightData);

            switch (FlightData.flight_state)
            {
            case STATE_GROUND_IDLE:
                if (1) //////////////////////////////////////  LOGIC
                {
                    FlightData.flight_state = STATE_POWERED_FLIGHT;
                    break;
                }

                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case STATE_POWERED_FLIGHT:
                if (1) //////////////////////////////////////  LOGIC
                {
                    FlightData.flight_state = STATE_UNPOWERED_FLIGHT;
                    break;
                }

                break;

            ///////////////////////////////////////////////////////////////////////////////////////////////
            case STATE_UNPOWERED_FLIGHT:
                if (1) //////////////////////////////////////  LOGIC
                {
                    FlightData.flight_state = STATE_BALLISTIC_DESCENT;
                    break;
                }
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////////
            case STATE_BALLISTIC_DESCENT:
                if (1) //////////////////////////////////////  LOGIC
                {
                    FlightData.flight_state = STATE_CHUTE_DESCENT;
                    break;
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case STATE_CHUTE_DESCENT:
                if (1) //////////////////////////////////////  LOGIC
                {
                    FlightData.flight_state = STATE_LANDING_SAFE;
                    break;
                }

                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case STATE_LANDING_SAFE:

                break;
            }

            estimationTimes++;
            W25Q128JVWritePage(&W25Q128JV_Handler, &FlightData);
            if (estimationTimes == 2)
            {
                estimationTimes = 0;
            }

            estimationFlag = 0;
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
     */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
     */
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief SDIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_SDIO_SD_Init(void)
{

    /* USER CODE BEGIN SDIO_Init 0 */

    /* USER CODE END SDIO_Init 0 */

    /* USER CODE BEGIN SDIO_Init 1 */

    /* USER CODE END SDIO_Init 1 */
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = 8;
    /* USER CODE BEGIN SDIO_Init 2 */

    /* USER CODE END SDIO_Init 2 */
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI2_Init(void)
{

    /* USER CODE BEGIN SPI2_Init 0 */

    /* USER CODE END SPI2_Init 0 */

    /* USER CODE BEGIN SPI2_Init 1 */

    /* USER CODE END SPI2_Init 1 */
    /* SPI2 parameter configuration*/
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI2_Init 2 */

    /* USER CODE END SPI2_Init 2 */
}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{

    /* USER CODE BEGIN TIM1_Init 0 */

    /* USER CODE END TIM1_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    /* USER CODE BEGIN TIM1_Init 1 */

    /* USER CODE END TIM1_Init 1 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 83;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 65535;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_TIM_ENABLE_OCxPRELOAD(&htim1, TIM_CHANNEL_2);
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM1_Init 2 */

    /* USER CODE END TIM1_Init 2 */
    HAL_TIM_MspPostInit(&htim1);
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 83;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 19999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */
    HAL_TIM_MspPostInit(&htim2);
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

    /* USER CODE BEGIN TIM3_Init 0 */

    /* USER CODE END TIM3_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM3_Init 1 */

    /* USER CODE END TIM3_Init 1 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 83;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 19999;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM3_Init 2 */

    /* USER CODE END TIM3_Init 2 */
    HAL_TIM_MspPostInit(&htim3);
}

/**
 * @brief TIM5 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM5_Init(void)
{

    /* USER CODE BEGIN TIM5_Init 0 */

    /* USER CODE END TIM5_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM5_Init 1 */

    /* USER CODE END TIM5_Init 1 */
    htim5.Instance = TIM5;
    htim5.Init.Prescaler = 8399;
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 2499;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM5_Init 2 */

    /* USER CODE END TIM5_Init 2 */
}

/**
 * @brief TIM8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM8_Init(void)
{

    /* USER CODE BEGIN TIM8_Init 0 */

    /* USER CODE END TIM8_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    /* USER CODE BEGIN TIM8_Init 1 */

    /* USER CODE END TIM8_Init 1 */
    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 1;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = 104;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM8_Init 2 */

    /* USER CODE END TIM8_Init 2 */
    HAL_TIM_MspPostInit(&htim8);
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    /* DMA1_Stream5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    /* DMA1_Stream6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    /* DMA2_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    /* DMA2_Stream3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    /* DMA2_Stream4_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */

    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : FLASH_CS_Pin */
    GPIO_InitStruct.Pin = FLASH_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : IMU_CS_Pin */
    GPIO_InitStruct.Pin = IMU_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(IMU_CS_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : IMU_INT_1_Pin */
    GPIO_InitStruct.Pin = IMU_INT_1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(IMU_INT_1_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : IMU_INT_2_Pin */
    GPIO_InitStruct.Pin = IMU_INT_2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(IMU_INT_2_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : SDIO_DET_Pin */
    GPIO_InitStruct.Pin = SDIO_DET_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SDIO_DET_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : STATUS_LED_Pin */
    GPIO_InitStruct.Pin = STATUS_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    /* USER CODE BEGIN MX_GPIO_Init_2 */

    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == IMU_INT_1_Pin)
    {
        ISM330DHCXReadRaw(&ISM330DHCX_Handler);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {

        HAL_GPIO_WritePin(ISM330DHCX_Handler.CSPort, ISM330DHCX_Handler.CSPin, GPIO_PIN_SET);
        ISM330DHCXRawToData(&ISM330DHCX_Handler);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM5)
    {
        BMP280ReadRaw(&BMP280_Handler);
    }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        if (I2CdmaState == STATE_READING_BMP)
        {
            // BMP finished! Now start LIS2MDL immediately
            ADCReadRaw(&ADC_Handler);
            LIS2MDLReadRaw(&LIS2MDL_Handler);
            I2CdmaState = STATE_READING_LIS;

            BMP280RawToData(&BMP280_Handler);
        }
        else if (I2CdmaState == STATE_READING_LIS)
        {
            // LIS finished! The sequencing chain is done.
            I2CdmaState = STATE_READING_BMP;

            LIS2MDLRawToData(&LIS2MDL_Handler);

            estimationFlag = 1;
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if (NEO6M_Handler.tempByte == '$')
        {
            /* FOUND IT! Sync detected. */

            // 1. Manually place '$' at the start of the buffer
            NEO6M_Handler.rawBuffer[0] = '$';

            // 2. Switch to DMA immediately to catch the rest of the sentence.
            // Notice: We point to &rawbuffer[1] because index 0 is already filled.
            // We reduce size by 1.
            HAL_UARTEx_ReceiveToIdle_DMA(NEO6M_Handler.huart, &NEO6M_Handler.rawBuffer[1], 511);
        }
        else
        {
            /* Garbage data (noise or middle of sentence). Keep Hunting. */
            HAL_UART_Receive_IT(NEO6M_Handler.huart, &NEO6M_Handler.tempByte, 1);
        }
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        /* DMA has stopped receiving. We now have a full batch. */

        // The 'Size' argument is how many bytes DMA transferred.
        // Since we manually added '$' at index 0, total length is Size + 1.
        uint16_t TotalLen = Size + 1;

        // 1. Null terminate for safety
        NEO6M_Handler.rawBuffer[TotalLen] = '\0';

        // 2. Process the Clean Data

        NEO6MRawToData(&NEO6M_Handler);

        //        sprintf(output, "%s", NEO6M_Handler.rawBuffer);
        //        debugging(output);
        //
        //        sprintf(output, "Time: %d:%d:%d | Lat: %.6f | Lon: %.6f | altitude: %.2f | Sats: %d\r\n",
        //                NEO6M_Handler.Hour, // String (%s)
        //                NEO6M_Handler.Min,
        //                NEO6M_Handler.Sec,
        //                NEO6M_Handler.Latitude,      // Float with 6 decimals (%.6f)
        //                NEO6M_Handler.Longitude,     // Float with 6 decimals (%.6f)
        //                NEO6M_Handler.Altitude,      // Float with 2 decimals (%.2f)
        //                NEO6M_Handler.SatellitesUsed // Integer (%d)
        //        );
        //
        //        debugging(output);

        // 3. Reset logic: Go back to "Hunting" for the next '$'
        // We do NOT restart DMA here. We restart the single-byte Interrupt.
        HAL_UART_Receive_IT(NEO6M_Handler.huart, &NEO6M_Handler.tempByte, 1);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) 
    {
        ADCRawToData(&ADC_Handler);
    }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
