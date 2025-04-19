/**
 * @file    Flash.hpp
 * @brief   Super rough example for a log storage class
 *
 */
#pragma once
#include "SystemDefines.hpp"
#include "Flash.hpp"

template < typename T >
class BasicLogStorage
{
    BasicLogStorage(Flash* fls, const int startAddr, const int numSectors)
        : 
        fls_(fls), 
        startAddr_(startAddr), 
        maxLogs_((fls->GetSectorSize() * numSectors)/sizeof(T)),
        nSectors_(numSectors),
        writeIdx_(0)
    {
    }

    bool Write(const T& data); // Write data to flash and advance the writeIndex, ret True if ok, False if error or out of space
    bool Read(int idx, T& data); // How do you know if read data is valid / at end? Assume 0xFF is invalid

    int GetMaxLogs() const { return maxLogs_; }
    bool Erase(); // Erase all sectors associated with this log storage

private:
    void GetWriteAddr() { writeAddr_ = startAddr_ + (writeIdx_ * sizeof(T)); }

    Flash* fls_;
    int startAddr_;
    int maxLogs_;
    int nSectors_;
    int writeIdx_;
}

template < typename T >
bool BasicLogStorage<T>::Write(const T& data)
{
    if (writeIdx_ >= maxLogs_)
    {
        return false; // Out of space
    }

    bool writeOk = fls_->Write(GetWriteAddr(), (uint8_t*)&data, sizeof(T));
    writeIdx_ += 1;
    return writeOk;
}

template < typename T >
bool BasicLogStorage<T>::Read(int idx, T& data)
{
    if (idx >= maxLogs_)
    {
        return false; // Out of bounds
    }

    bool readOk = fls_->Read(startAddr_ + (idx * sizeof(T)), (uint8_t*)&data, sizeof(T));
    return readOk;
}

template < typename T >
bool BasicLogStorage<T>::Erase()
{
    int successes = 0;
    for(int i = 0; i < nSectors_; i++)
    {
        if(fls_->Erase(startAddr_ + (i * fls_->GetSectorSize())))
        {
            successes += 1;
        }
    }
    return successes == nSectors_;
}