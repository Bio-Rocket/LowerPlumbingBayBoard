/*
 * FlashDriver.cpp
 *
 *  Created on: Apr 17, 2025
 *      Author: jessegerbrandt
 */

#include "Flash.hpp"
#include "SystemDefines.hpp"
#include <cstring>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
HAL_StatusTypeDef Flash_Write(uint32_t next_addr, uint32_t sector_end, uint16_t data) {

    // If address is within sector
    if (next_addr >= sector_end) {
        return HAL_ERROR;
    }

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_addr, data);
    HAL_FLASH_Lock();

    // Increment the address by 2 bytes (halfword)
    if (status == HAL_OK) {
        next_addr += 2;
    }

    return status;
}

HAL_StatusTypeDef Flash_Erase(uint32_t sector) {

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

// Erases all sensor sectors
void Flash_Erase_All_User_Sectors(void) {
    for (uint32_t sector = FLASH_SECTOR_5; sector <= FLASH_SECTOR_8; sector++) {
        Flash_Erase(sector);
    }
}

void Flash_Offload(uint32_t sector_start, uint32_t sector_end) {
    // Iterate through the flash sector reading 2 bytes at a time
    for (uint32_t addr = sector_start; addr < sector_end; addr += sizeof(uint16_t)) {
        // Read the sensor value
        uint16_t value = *reinterpret_cast<const uint16_t*>(addr);

        // Stop printing if there is no more data.
        // maybe check for multiple in a row
        if (value == 0xFFFF) {
            break;
        }

        // Print the address and the 16-bit value
        SOAR_PRINT("Address: 0x%08X, Data: 0x%04X\n", addr, value);
    }
}
