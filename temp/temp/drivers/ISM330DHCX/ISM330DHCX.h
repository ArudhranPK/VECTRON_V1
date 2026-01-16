#ifndef SRC_DRIVERS_ISM330DHCX_ISM330DHCX_H_
#define SRC_DRIVERS_ISM330DHCX_ISM330DHCX_H_

//  Imports
#include "stm32f4xx_hal.h"

//  Constants

#define ISM330DHCX_WHO_AM_I_REG         0x6B
#define ISM330DHCX_WHO_AM_I_REG_ADDR    0x0F

#define ISM330DHCX_RESET_VALUE          0x01
#define ISM330DHCX_RESET_VALUE_ADDR     0x12

#define ISM330DHCX_ODR_XL               0b0111  //  833HZ
#define ISM330DHCX_FS_XL                0b01    //  +-16g
#define ISM330DHCX_LPF2_XL_EN           0b1
#define ISM330DHCX_CTRL1_XL             ((ISM330DHCX_ODR_XL << 4) | (ISM330DHCX_FS_XL << 2) | (ISM330DHCX_LPF2_XL_EN << 1))
#define ISM330DHCX_CTRL1_XL_ADDR        0x10

#define ISM330DHCX_ODR_G                0b0111  //  833Hz
#define ISM330DHCX_FS_G                 0b11    //  +-2000DPS
#define ISM330DHCX_FS_125               0b0
#define ISM330DHCX_FS_4000              0b0
#define ISM330DHCX_CTRL2_G              ((ISM330DHCX_ODR_G << 4) | (ISM330DHCX_FS_G << 2) | (ISM330DHCX_FS_125 << 1) | (ISM330DHCX_FS_4000))
#define ISM330DHCX_CTRL2_G_ADDR         0x11

#define ISM330DHCX_BOOT                 0b0
#define ISM330DHCX_BDU                  0b1
#define ISM330DHCX_H_LACTIVE            0b1
#define ISM330DHCX_PP_OD                0b1
#define ISM330DHCX_SIM                  0b0
#define ISM330DHCX_IF_INC               0b1
#define ISM330DHCX_SW_RESET             0b0
#define ISM330DHCX_CTRL3_C              ((ISM330DHCX_BOOT << 7) | (ISM330DHCX_BDU << 6) | (ISM330DHCX_H_LACTIVE << 5) | (ISM330DHCX_PP_OD << 4) | (ISM330DHCX_SIM << 3) | (ISM330DHCX_IF_INC << 2) | (ISM330DHCX_SW_RESET))
#define ISM330DHCX_CTRL3_C_ADDR         0x12

#define ISM330DHCX_SLEEP_G              0b0
#define ISM330DHCX_INT2_ON_INT1         0b0
#define ISM330DHCX_DRDY_MASK            0b1
#define ISM330DHCX_I2C_DISABLE          0b1
#define ISM330DHCX_LPF1_SEL_G           0b1
#define ISM330DHCX_CTRL4_C              ((ISM330DHCX_SLEEP_G << 6) | (ISM330DHCX_INT2_ON_INT1 << 5) | (ISM330DHCX_DRDY_MASK << 3) | (ISM330DHCX_I2C_DISABLE << 2) | (ISM330DHCX_LPF1_SEL_G << 1))
#define ISM330DHCX_CTRL4_C_ADDR         0X13

#define ISM330DHCX_TRIG_EN              0b0
#define ISM330DHCX_LVL1_EN              0b0
#define ISM330DHCX_LVL2_EN              0b0
#define ISM330DHCX_XL_HM_MODE           0b0
#define ISM330DHCX_USR_OFF_W            0b0
#define ISM330DHCX_FTYPE                0b000   //  222Hz
#define ISM330DHCX_CTRL6_C              ((ISM330DHCX_TRIG_EN << 7) | (ISM330DHCX_LVL1_EN << 6) | (ISM330DHCX_LVL2_EN << 5) | (ISM330DHCX_XL_HM_MODE << 4) | (ISM330DHCX_USR_OFF_W << 3) | (ISM330DHCX_FTYPE))
#define ISM330DHCX_CTRL6_C_ADDR         0x15

#define ISM330DHCX_HPCF_XL              0b010   // ODR/20 = 41Hz
#define ISM330DHCX_HP_REF_MODE_XL       0b0
#define ISM330DHCX_FASTSETTL_MODE_XL    0b0
#define ISM330DHCX_HP_SLOPE_XL_EN       0b0
#define ISM330DHCX_LOW_PASS_ON_6D       0b0
#define ISM330DHCX_CTRL8_XL             ((ISM330DHCX_HPCF_XL << 5) | (ISM330DHCX_HP_REF_MODE_XL << 4) | (ISM330DHCX_FASTSETTL_MODE_XL << 3) | (ISM330DHCX_HP_SLOPE_XL_EN << 2) | (ISM330DHCX_LOW_PASS_ON_6D))
#define ISM330DHCX_CTRL8_XL_ADDR        0x17

#define ISM330DHCX_DEN_DRDY_FLAG        0b0
#define ISM330DHCX_INT1_CNT_BDR         0b0
#define ISM330DHCX_INT1_FIFO_FULL       0b0
#define ISM330DHCX_INT1_FIFO_OVR        0b0
#define ISM330DHCX_INT1_FIFO_TH         0b0
#define ISM330DHCX_INT1_BOOT            0b0
#define ISM330DHCX_INT1_DRDY_G          0b0
#define ISM330DHCX_INT1_DRDY_XL         0b1
#define ISM330DHCX_INT1_CTRL            ((ISM330DHCX_DEN_DRDY_FLAG << 7) | (ISM330DHCX_INT1_CNT_BDR << 6) | (ISM330DHCX_INT1_FIFO_FULL << 5) | (ISM330DHCX_INT1_FIFO_OVR << 4) | (ISM330DHCX_INT1_FIFO_TH << 3) | (ISM330DHCX_INT1_BOOT << 2) | (ISM330DHCX_INT1_DRDY_G << 1) | (ISM330DHCX_INT1_DRDY_XL))
#define ISM330DHCX_INT1_CTRL_ADDR       0x0D

#define ISM330DHCX_INT2_CNT_BDR         0b0
#define ISM330DHCX_INT2_FIFO_FULL       0b0
#define ISM330DHCX_INT2_FIFO_OVR        0b0
#define ISM330DHCX_INT2_FIFO_TH         0b0
#define ISM330DHCX_INT2_DRDY_TEMP       0b0
#define ISM330DHCX_INT2_DRDY_G          0b0
#define ISM330DHCX_INT2_DRDY_XL         0b0
#define ISM330DHCX_INT2_CTRL            ((ISM330DHCX_INT2_DRDY_XL << 6) | (ISM330DHCX_INT2_DRDY_G << 5) | (ISM330DHCX_INT2_DRDY_TEMP << 4) | (ISM330DHCX_INT2_FIFO_TH << 3) | (ISM330DHCX_INT2_FIFO_OVR << 2) | (ISM330DHCX_INT2_FIFO_FULL << 1) | (ISM330DHCX_INT2_CNT_BDR))
#define ISM330DHCX_INT2_CTRL_ADDR       0x0E

#define ISM330DHCX_OUT_TEMP_L_ADDR      0x20
#define ISM330DHCX_OUTX_L_G_ADDR        0x22
#define ISM330DHCX_OUTX_L_A_ADDR        0x28

#define ISM330DHCX_OUT_START_ADDR       0x20


//  Type definition
typedef struct{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *CSPort;
    uint16_t CSPin;

    uint8_t txBuffer[15];
    uint8_t rxBuffer[15];

    float temperature;

    float accel_g_x;
    float accel_g_y;
    float accel_g_z;

    float gyro_dps_x;
    float gyro_dps_y;
    float gyro_dps_z;

}ISM330DHCX_Handle_t;

HAL_StatusTypeDef ISM330DHCXInit(ISM330DHCX_Handle_t *imu);
HAL_StatusTypeDef ISM330DHCXReadRaw(ISM330DHCX_Handle_t *imu);
HAL_StatusTypeDef ISM330DHCXRawToData(ISM330DHCX_Handle_t *imu);


#endif /* SRC_DRIVERS_ISM330DHCX_ISM330DHCX_H_ */
