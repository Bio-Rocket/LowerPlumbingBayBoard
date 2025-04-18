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
#define PT18_SECTOR  FLASH_SECTOR_5
#define PT19_SECTOR  FLASH_SECTOR_6
#define TC11_SECTOR  FLASH_SECTOR_7
#define TC12_SECTOR  FLASH_SECTOR_8

#define PT18_ADDR    ADDR_FLASH_SECTOR_5
#define PT19_ADDR    ADDR_FLASH_SECTOR_6
#define TC11_ADDR    ADDR_FLASH_SECTOR_7
#define TC12_ADDR    ADDR_FLASH_SECTOR_8

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
HAL_StatusTypeDef Flash_Write(uint32_t address, uint32_t* data, uint32_t length)
{
  HAL_FLASH_Unlock();

  for (uint32_t i = 0; i < length; i++) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + (i * 4), data[i]) != HAL_OK) {
      HAL_FLASH_Lock();
      return HAL_ERROR;
    }
  }

  HAL_FLASH_Lock();
  return HAL_OK;
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
