/*
 * Flash.hpp
 *
 *  Created on: Apr 17, 2025
 *      Author: jessegerbrandt
 */

#ifndef FLASH_HPP
#define FLASH_HPP

#include <cstdint>
#include "stm32f4xx_hal.h"

class Flash
{
public:
    virtual ~Flash() = default;
    virtual bool Erase(uint32_t offset) = 0;
    virtual bool Write(uint32_t offset, uint8_t* data, uint32_t len) = 0;
    virtual bool Read(uint32_t offset, uint8_t* data, uint32_t len) = 0;
    virtual bool EraseChip() = 0;

    virtual uint32_t GetSectorSize() const = 0;
    virtual uint32_t GetNumSectors() const = 0;
    virtual uint32_t GetBaseAddr() const = 0;
};

#endif // FLASH_HPP
