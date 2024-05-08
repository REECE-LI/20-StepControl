#include "mt6816_stm32.h"
#include "spi.h"

void MT6816::SpiInit()
{
//    MX_SPI1_Init();
}

uint16_t MT6816::SpiTransmitAndRead16Bits(uint16_t _dataTx)
{
    uint16_t dataRx;

    GPIOB->BRR = GPIO_PIN_5; // Chip select
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t*) &_dataTx, (uint8_t*) &dataRx, 1, HAL_MAX_DELAY);
    GPIOB->BSRR = GPIO_PIN_5;

    return dataRx;
}
