#pragma once
#include <vector>
#include <cstdint>
#include "FlashDriver.hpp"

class FlashSectorManager {
public:
    FlashSectorManager(FlashDriver* driver)
        : driver_(driver), numSectors_(driver->GetNumSectors()), used_(driver->GetNumSectors(), false) {}

    // Allocates numSectors and returns the first sector index, or -1 if not enough free
    int Allocate(uint32_t numSectors) {
        for (uint32_t i = 0; i <= numSectors_ - numSectors; ++i) {
            bool free = true;
            for (uint32_t j = 0; j < numSectors; ++j) {
                if (used_[i + j]) { free = false; break; }
            }
            if (free) {
                for (uint32_t j = 0; j < numSectors; ++j) used_[i + j] = true;
                return i;
            }
        }
        return -1;
    }

    // Returns the start address for a given sector index
    uint32_t GetSectorStartAddr(int sectorIdx) const {
        return driver_->GetBaseAddr() + driver_->GetSectorSize() * sectorIdx;
    }

    // free sectors (called after erase)
    void Free(int sectorIdx, uint32_t numSectors) {
        for (uint32_t i = 0; i < numSectors; ++i) {
            used_[sectorIdx + i] = false;
        }
    }

    uint32_t GetSectorSize() const { return driver_->GetSectorSize(); }
    uint32_t GetNumSectors() const { return numSectors_; }

private:
    FlashDriver* driver_;
    uint32_t numSectors_;
    std::vector<bool> used_;
};