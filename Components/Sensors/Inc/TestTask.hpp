#pragma once

#include "Task.hpp"
#include "FlashDriver.hpp"
#include "BasicLogStorage.hpp"
#include "FlashSectorManager.hpp"

enum TEST_SENSOR_COMMANDS {
    TEST_SENSOR_CMD_NONE = 0,
    TEST_SENSOR_CMD_ERASE,
    TEST_SENSOR_CMD_WRITE,
    TEST_SENSOR_CMD_DUMP,
    TEST_SENSOR_CMD_WRITE_BURST,
};

using TestSensorData = uint16_t;

class TestSensorTask : public Task
{
public:
    static TestSensorTask& Inst() {
        static TestSensorTask inst;
        return inst;
    }

    void InitTask();

private:
    TestSensorTask();
    TestSensorTask(const TestSensorTask&) = delete;
    TestSensorTask& operator=(const TestSensorTask&) = delete;

    // Placement new buffer and pointer
    alignas(BasicLogStorage<TestSensorData>) uint8_t log_buf_[sizeof(BasicLogStorage<TestSensorData>)];
    BasicLogStorage<TestSensorData>* log_;

    bool burstActive_ = false;
    uint32_t burstCount_ = 0;
    static constexpr uint32_t BURST_TOTAL = 1000; // 1kHz for 1s

protected:
    static void RunTask(void* pvParams) { TestSensorTask::Inst().Run(pvParams); }
    void Run(void* pvParams);
    void HandleCommand(Command& cm);
};