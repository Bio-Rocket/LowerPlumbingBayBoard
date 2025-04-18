/*
 * FlashDriver.cpp
 *
 *  Created on: Apr 17, 2025
 *      Author: jessegerbrandt
 */

#include "Flash.hpp"
#include "SystemDefines.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
HAL_StatusTypeDef Flash_Write(uint32_t* next_addr, uint32_t sector_end, uint16_t data) {

    if (*next_addr >= sector_end) {
        return HAL_ERROR;
    }

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, *next_addr, data);
    HAL_FLASH_Lock();

    if (status == HAL_OK) {
        *next_addr += 2;
    }

    return status;
}

HAL_StatusTypeDef Flash_Erase(uint32_t sector)
{
  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;

  EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector       = sector;
  EraseInitStruct.NbSectors    = 1;

  HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

  HAL_FLASH_Lock();
  return status;
}

void Flash_Erase_AllUserSectors(void)
{
  for (uint32_t sector = FLASH_SECTOR_5; sector <= FLASH_SECTOR_8; sector++) {
    Flash_Erase(sector);
  }
}
