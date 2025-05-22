#include "FlashDriver.hpp"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"
#include <cstring>
#include <cstdint>

constexpr uint32_t USER_FLASH_BASE = 0x08040000UL; // Base addr for sector 6
constexpr uint32_t SECTOR_SIZE = 128 * 1024;       // 128KB per sector
constexpr uint32_t NUM_SECTORS = 6;                // 6 sectors available

FlashDriver::FlashDriver(uint32_t sectorSize)
    : sectorSize_(sectorSize),
      currentSectorStartAddr_(0),
      currentWriteAddr_(0)
{
}

uint32_t FlashDriver::GetSectorSize() const { return SECTOR_SIZE; }
uint32_t FlashDriver::GetNumSectors() const { return NUM_SECTORS; }
uint32_t FlashDriver::GetBaseAddr() const { return USER_FLASH_BASE; }

bool FlashDriver::Erase(uint32_t offset)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = GetSectorNumber(offset);
    EraseInitStruct.NbSectors = 1;

    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

    HAL_FLASH_Lock();
    return status == HAL_OK;
}

// TODO: Create a full WORD version
bool FlashDriver::Write(uint32_t offset, uint8_t* data, uint32_t len)
{
    HAL_FLASH_Unlock();

    for (uint32_t i = 0; i < len; i += 2)
    {
        uint16_t halfword = *reinterpret_cast<uint16_t*>(data + i);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, offset + i, halfword) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return false;
        }
    }

    currentWriteAddr_ = offset + len;
    HAL_FLASH_Lock();
    return true;
}

bool FlashDriver::Read(uint32_t offset, uint8_t* data, uint32_t len)
{
    std::memcpy(data, reinterpret_cast<const void*>(offset), len);
    return true;
}

// Scary. should be modified to only erase user sectors
bool FlashDriver::EraseChip()
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_MASSERASE;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

    HAL_FLASH_Lock();
    return status == HAL_OK;
}

uint32_t FlashDriver::GetSectorNumber(uint32_t address) const {
    if (address < 0x08004000) return 0;
    if (address < 0x08008000) return 1;
    if (address < 0x0800C000) return 2;
    if (address < 0x08010000) return 3;
    if (address < 0x08020000) return 4;
    if (address < 0x08040000) return 5;
    if (address < 0x08060000) return 6;
    if (address < 0x08080000) return 7;
    if (address < 0x080A0000) return 8;
    if (address < 0x080C0000) return 9;
    if (address < 0x080E0000) return 10;
    if (address < 0x08100000) return 11;
    
    return 0xFFFFFFFF; // Invalid
}