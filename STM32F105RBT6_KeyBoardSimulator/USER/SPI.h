#ifndef SPI_H
#define SPI_H

#include "stm32f10x.h"

#define KEYBOARD_SPI_CS      GPIO_Pin_14
#define KEYBOARD_SPI_SCK     GPIO_Pin_13
#define KEYBOARD_SPI_MOSI    GPIO_Pin_11
#define KEYBOARD_SPI_MISO    GPIO_Pin_12


void SPI_FLASH_Init(void);
void sendSPIMsg(uint8_t msg);

#endif //SPI_H
