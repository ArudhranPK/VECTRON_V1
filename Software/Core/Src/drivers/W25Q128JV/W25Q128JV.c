#include "W25Q128JV.h"

static void W25Q128JVWaitForReady(W25Q128JV_Handle_t *flash) {
    uint8_t status_reg;
    uint8_t cmd = CMD_READ_STATUS_1;

    do {
        HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(flash->hspi, &cmd, 1, 100);
        HAL_SPI_Receive(flash->hspi, &status_reg, 1, 100);
        HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

    } while ((status_reg & 0x01));
}

HAL_StatusTypeDef W25Q128JVInit(W25Q128JV_Handle_t *flash)
{
    uint8_t tempTxBuffer[5] = {};
    uint8_t tempRxBuffer[5] = {};
    if (flash->hspi == NULL || flash->CsPort == NULL)
    {
        return HAL_ERROR;
    }

    // Ensure CS is high before starting
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);
    HAL_Delay(10);

    //  Resetting the device
    W25Q128JVWaitForReady(flash);

    tempTxBuffer[0] = CMD_ENABLE_RESET;
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(flash->hspi,tempTxBuffer, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

    tempTxBuffer[0] = CMD_RESET_DEVICE;
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(flash->hspi, tempTxBuffer, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);
    HAL_Delay(10);

    //  Checking device ID
    W25Q128JVWaitForReady(flash);

    tempTxBuffer[0] = CMD_CHECK_ID;
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(flash->hspi, tempTxBuffer, tempRxBuffer, 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);


    if (!((tempRxBuffer[1] == 0xEF) && ((tempRxBuffer[2] << 8) | (tempRxBuffer[3])) == 0x4018)) return HAL_ERROR;

    //  Global Unlock
//    tempTxBuffer[0] = CMD_WRITE_ENABLE;
//    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
//    HAL_SPI_Transmit(flash->hspi, tempTxBuffer, 1, 100);
//    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);
//
//
//    tempTxBuffer[0] = 0x01; // Write Status Register Command
//    tempTxBuffer[1] = 0x00; // Value to write (0x00 = No Protection)
//
//    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
//    HAL_SPI_Transmit(flash->hspi, tempTxBuffer, 2, 100);
//    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

    //Scan for free space
    tempTxBuffer[0] = CMD_READ_DATA;
    for(uint32_t pageNo = 0; pageNo < W25Q_PAGE_COUNT; pageNo++){
        tempTxBuffer[1] = pageNo >> 8;
        tempTxBuffer[2] = pageNo;
        tempTxBuffer[3] = 0;

        HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(flash->hspi, tempTxBuffer, tempRxBuffer, 5, HAL_MAX_DELAY);
        HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

        if(tempRxBuffer[4] == 0xFF)
        {
            if ((pageNo + 1) >= W25Q_PAGE_COUNT)
            {
                flash->currentPage = pageNo;
                break;
            }
            pageNo++;
            tempTxBuffer[1] = (pageNo >> 8);
            tempTxBuffer[2] = (pageNo);
            tempTxBuffer[3] = 0;

            HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(flash->hspi, tempTxBuffer, tempRxBuffer, 5, HAL_MAX_DELAY);
            HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

            if(tempRxBuffer[4] == 0xFF)
            {
                flash->currentPage = pageNo;
                break;
            }
        }
    }

    return HAL_OK;
}


HAL_StatusTypeDef W25Q128JVWritePage(W25Q128JV_Handle_t *flash, FlightDataHandler_t *data)
{
    uint8_t tempTxBuffer[4] = {};

    tempTxBuffer[0] = CMD_WRITE_ENABLE;
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(flash->hspi, tempTxBuffer, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

    tempTxBuffer[0] = CMD_PAGE_PROGRAM;
    tempTxBuffer[1] = flash->currentPage >> 8;
    tempTxBuffer[2] = flash->currentPage;
    tempTxBuffer[3] = flash->currentOffset;

    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(flash->hspi, tempTxBuffer, 4, HAL_MAX_DELAY);
    HAL_SPI_Transmit(flash->hspi, (uint8_t*)data, 119, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

    flash->currentOffset += 128;
    if(!flash->currentOffset) flash->currentPage += 1;

    W25Q128JVWaitForReady(flash);

    return HAL_OK;
}

HAL_StatusTypeDef W25Q128JVReadPage(W25Q128JV_Handle_t *flash, FlightDataHandler_t *data)
{
    // 1. Prepare Command (0x03 = Read Data) and Address
    uint8_t tempTxBuffer[4] = {};

    tempTxBuffer[0] = CMD_READ_DATA;
    tempTxBuffer[1] = flash->currentPage >> 8;
    tempTxBuffer[2] = flash->currentPage;
    tempTxBuffer[3] = flash->currentOffset;

    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(flash->hspi, tempTxBuffer, 4, HAL_MAX_DELAY);
    HAL_SPI_Receive(flash->hspi, (uint8_t*)data, 119, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

    flash->currentOffset += 128;
    if(!flash->currentOffset) flash->currentPage += 1;

    return HAL_OK;
}

HAL_StatusTypeDef W25Q128JVCleanSlate(W25Q128JV_Handle_t *flash)
{

    uint8_t tempTxBuffer = CMD_WRITE_ENABLE;
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(flash->hspi, &tempTxBuffer, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

    tempTxBuffer = CMD_CHIP_ERASE;
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(flash->hspi, &tempTxBuffer, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(flash->CsPort, flash->CsPin, GPIO_PIN_SET);

    W25Q128JVWaitForReady(flash);

    flash->currentPage = 1;
    flash->currentOffset = 0;

    return HAL_OK;
}
