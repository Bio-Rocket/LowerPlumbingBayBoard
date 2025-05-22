#pragma once

#include "Flash.hpp"
#include <cstdint>

class FlashDriver : public Flash
{
public:
    FlashDriver(uint32_t sectorSize);

    bool Erase(uint32_t offset) override;
    bool Write(uint32_t offset, uint8_t* data, uint32_t len) override;
    bool Read(uint32_t offset, uint8_t* data, uint32_t len) override;
    bool EraseChip() override;
    uint32_t GetSectorNumber(uint32_t address) const;
    
    uint32_t GetSectorSize() const override;
    uint32_t GetNumSectors() const override;
    uint32_t GetBaseAddr() const override;

private:
    uint32_t sectorSize_;
    uint32_t currentSectorStartAddr_;
    uint32_t currentWriteAddr_;
};