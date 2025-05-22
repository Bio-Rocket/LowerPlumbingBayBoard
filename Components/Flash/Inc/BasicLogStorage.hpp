#ifndef BASIC_LOG_STORAGE_HPP
#define BASIC_LOG_STORAGE_HPP

#include "FlashDriver.hpp"
#include <cstdint>
#include <cstring>

template <typename T>
class BasicLogStorage
{
public:
    BasicLogStorage(Flash* fls, uint32_t startAddr, uint32_t numSectors)
        : fls_(fls),
          startAddr_(startAddr),
          nSectors_(numSectors),
          maxLogs_((fls->GetSectorSize() * numSectors) / sizeof(T)),
          writeIdx_(0)
    {
        // scan for first unwritten log (0xFF) on init 
        T temp;
        for (uint32_t i = 0; i < maxLogs_; ++i) {
            if (!ReadRaw(i, temp)) {
                writeIdx_ = i;
                break;
            }
        }
    }

    // Write a log entry, returns true on success
    bool Write(const T& data)
    {
        if (writeIdx_ >= maxLogs_)
            return false; // Out of space

        uint32_t writeAddr = GetWriteAddr(writeIdx_);
        bool writeOk = fls_->Write(writeAddr, reinterpret_cast<uint8_t*>(const_cast<T*>(&data)), sizeof(T));
        if (writeOk)
            ++writeIdx_;
        return writeOk;
    }

    // Read a log entry by index, returns true if valid data
    bool Read(uint32_t idx, T& data)
    {
        if (idx >= maxLogs_)
            return false; // Out of bounds

        if (!ReadRaw(idx, data))
            return false;

        // Check if the data is valid (not erased)
        const uint8_t* dataBytes = reinterpret_cast<const uint8_t*>(&data);
        for (size_t i = 0; i < sizeof(T); ++i) {
            if (dataBytes[i] != 0xFF)
                return true; // Data is valid
        }
        return false; // Data is erased/invalid
    }

    // Returns the maximum number of logs that can be stored
    uint32_t GetMaxLogs() const { return maxLogs_; }

    // Erase all sectors used by this log storage
    bool Erase()
    {
        uint32_t successes = 0;
        for (uint32_t i = 0; i < nSectors_; ++i) {
            uint32_t sectorAddr = startAddr_ + (i * fls_->GetSectorSize());
            if (fls_->Erase(sectorAddr))
                ++successes;
        }
        writeIdx_ = 0;
        return successes == nSectors_;
    }

    // Returns the current write index (next log to be written)
    uint32_t GetWriteIndex() const { return writeIdx_; }

private:
    // Returns the flash address for a given log index
    uint32_t GetWriteAddr(uint32_t idx) const
    {
        return startAddr_ + (idx * sizeof(T));
    }

    // Raw read, does not check for erased/valid data
    bool ReadRaw(uint32_t idx, T& data)
    {
        if (idx >= maxLogs_)
            return false;
        uint32_t readAddr = GetWriteAddr(idx);
        return fls_->Read(readAddr, reinterpret_cast<uint8_t*>(&data), sizeof(T));
    }

    Flash* fls_;
    uint32_t startAddr_;
    uint32_t nSectors_;
    uint32_t maxLogs_;
    uint32_t writeIdx_;
};

#endif // BASIC_LOG_STORAGE_HPP