/*
 * Flash.hpp
 *
 *  Created on: Apr 17, 2025
 *      Author: jessegerbrandt
 */

#ifndef FLASH_HPP_
#define FLASH_HPP_

#include "stm32f4xx_hal.h"

#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000)
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000)
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000)
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000)

HAL_StatusTypeDef Flash_Write(uint32_t address, uint32_t* data, uint32_t length);
HAL_StatusTypeDef Flash_Erase(uint32_t sector);
void Flash_Erase_AllUserSectors(void);

#endif /* FLASH_HPP_ */
