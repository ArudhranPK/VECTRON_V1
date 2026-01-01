#ifndef SRC_DRIVERS_SPIFLASH_SPIFLASH_H_
#define SRC_DRIVERS_SPIFLASH_SPIFLASH_H_

//  Imports
#include "stm32f4xx_hal.h"


//  Costants
#define W25Q_PAGE_SIZE       256
#define W25Q_SECTOR_SIZE     4096
#define W25Q_PAGE_COUNT      65536

// W25Q128JV Commands
#define CMD_CHECK_ID         0x9F
#define CMD_WRITE_ENABLE     0x06
#define CMD_PAGE_PROGRAM     0x02
#define CMD_READ_DATA        0x03
#define CMD_READ_STATUS_1    0x05
#define CMD_CHIP_ERASE       0xC7
#define CMD_ENABLE_RESET     0x66
#define CMD_RESET_DEVICE     0x99

//  Type definition
typedef struct{
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *CsPort;
    uint16_t CsPin;

    uint32_t currentPage;
    uint8_t currentOffset;

} W25Q128JV_Handle_t;

//  Function declaration
HAL_StatusTypeDef W25Q128JVInit(W25Q128JV_Handle_t *flash);
HAL_StatusTypeDef W25Q128JVWritePage(W25Q128JV_Handle_t *flash);
HAL_StatusTypeDef W25Q128JVCleanSlate(W25Q128JV_Handle_t *flash);

#endif /* SRC_DRIVERS_SPIFLASH_SPIFLASH_H_ */
