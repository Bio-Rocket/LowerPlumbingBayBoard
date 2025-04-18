/*
 * Flash.hpp
 *
 *  Created on: Apr 17, 2025
 *      Author: jessegerbrandt
 */

#ifndef FLASH_HPP_
#define FLASH_HPP_

#include "stm32f4xx_hal.h"

#define PT18_SECTOR  FLASH_SECTOR_5
#define PT19_SECTOR  FLASH_SECTOR_6
#define TC11_SECTOR  FLASH_SECTOR_7
#define TC12_SECTOR  FLASH_SECTOR_8

#define PT18_ADDR    ADDR_FLASH_SECTOR_5
#define PT19_ADDR    ADDR_FLASH_SECTOR_6
#define TC11_ADDR    ADDR_FLASH_SECTOR_7
#define TC12_ADDR    ADDR_FLASH_SECTOR_8

#define PT18_END_ADDR    END_ADDR_FLASH_SECTOR_5
#define PT19_END_ADDR    END_ADDR_FLASH_SECTOR_6
#define TC11_END_ADDR    END_ADDR_FLASH_SECTOR_7
#define TC12_END_ADDR    END_ADDR_FLASH_SECTOR_8

#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000)
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000)
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000)
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000)

#define END_ADDR_FLASH_SECTOR_5     ((uint32_t)0x0803FFFF)
#define END_ADDR_FLASH_SECTOR_6     ((uint32_t)0x0805FFFF)
#define END_ADDR_FLASH_SECTOR_7     ((uint32_t)0x0807FFFF)
#define END_ADDR_FLASH_SECTOR_8     ((uint32_t)0x0809FFFF)

HAL_StatusTypeDef Flash_Write(uint32_t* next_addr, uint32_t sector_end, uint16_t data);
HAL_StatusTypeDef Flash_Erase(uint32_t sector);
void Flash_Erase_AllUserSectors(void);

#endif /* FLASH_HPP_ */
