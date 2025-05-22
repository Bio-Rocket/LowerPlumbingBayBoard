#include "TestTask.hpp"
#include "FlashDriver.hpp"
#include "BasicLogStorage.hpp"
#include "FlashSectorManager.hpp"
#include "main.h"
#include "Task.hpp"
#include <cstdlib>
#include <ctime>
#include "cmsis_os.h"
#include "DebugTask.hpp"
#include "core_cm4.h"
#include "main_avionics.hpp"

TestSensorTask::TestSensorTask()
    : Task(TASK_TEST_QUEUE_DEPTH_OBJS),
      log_(nullptr)
{
    std::srand(HAL_GetTick()); // Seed the rng
    int sectorIdx = gFlashManager->Allocate(1);
    SOAR_ASSERT(sectorIdx >= 0, "Failed to allocate flash sector for TestSensorTask");
    log_ = new (log_buf_) BasicLogStorage<TestSensorData>(
        gFlashDriver,
        gFlashManager->GetSectorStartAddr(sectorIdx),
        1
    );
}

void TestSensorTask::InitTask()
{
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize TestSensorTask twice");

    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)TestSensorTask::RunTask,
            (const char*)"TestSensorTask",
            (uint16_t)TASK_TEST_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)TASK_TEST_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "TestSensorTask::InitTask() - xTaskCreate() failed");
}

void TestSensorTask::Run(void* pvParams)
{
    TestSensorTask* self = static_cast<TestSensorTask*>(pvParams);

    Command cm;
    TickType_t lastWakeTime = xTaskGetTickCount();

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    while (1) {
        if (self->qEvtQueue->Receive(cm)) {
            self->HandleCommand(cm);
        }

        // If burst is active, do 1kHz writes for 1 second
        static uint32_t start_cycles = 0;
        if (self->burstActive_ && self->burstCount_ < BURST_TOTAL) {
            if (self->burstCount_ == 0) {
                start_cycles = DWT->CYCCNT;
                lastWakeTime = xTaskGetTickCount();
                SOAR_PRINT("Burst start cycles: %lu\n", start_cycles);
            }

            TestSensorData data = static_cast<uint16_t>(std::rand() & 0xFFFF);
            self->log_->Write(data);

            // Timing
            self->burstCount_++;
            if (self->burstCount_ == BURST_TOTAL) {
                uint32_t end_cycles = DWT->CYCCNT;
                SOAR_PRINT("Burst end cycles: %lu\n", end_cycles);

                uint32_t cpu_hz = HAL_RCC_GetHCLKFreq();
                uint64_t elapsed_ns = ((uint64_t)(end_cycles - start_cycles) * 1000000000ULL) / cpu_hz;
                SOAR_PRINT("Elapsed time: %lu ns\n", (uint32_t)elapsed_ns);
            }

            vTaskDelayUntil(&lastWakeTime, 1);
        } else {
            self->burstActive_ = false;
            osDelay(10);
        }
    }
}

void TestSensorTask::HandleCommand(Command& cm)
{
    switch (cm.GetTaskCommand()) {
    case TEST_SENSOR_CMD_ERASE:
        log_->Erase();
        SOAR_PRINT("TestSensorTask: Flash erased\n");
        break;
    case TEST_SENSOR_CMD_WRITE: {
        TestSensorData data = static_cast<uint16_t>(std::rand() & 0xFFFF);
        log_->Write(data);
        SOAR_PRINT("TestSensorTask: uint16_t data written: %u\n", data);
        break;
    }
    case TEST_SENSOR_CMD_WRITE_BURST: {
        burstActive_ = true;
        burstCount_ = 0;
        SOAR_PRINT("Starting 1kHz burst for 1s\n");
        break;
    }
    case TEST_SENSOR_CMD_DUMP: {
        SOAR_PRINT("TestSensorTask: Dumping flash contents:\n");
        TestSensorData data;
        const uint32_t maxLogs = log_->GetMaxLogs();
        uint32_t emptyCount = 0;
        for (uint32_t i = 0; i < maxLogs; ++i) {
            if (!log_->Read(i, data)) {
                // Could not read
                emptyCount++;
                if (emptyCount >= 6) {
                    SOAR_PRINT("EOF\n");
                    break;
                }
            } else {
                // if the data is empty (I dont think it actually ever gets here)
                if (data == 0xFFFF) {
                    emptyCount++;
                    if (emptyCount >= 6) {
                        SOAR_PRINT("EOF\n");
                        break;
                    }
                } else {
                    emptyCount = 0;
                    SOAR_PRINT("  [%lu] %u\n", i, data);
                }
            }
            osDelay(50);
        }
        break;
    }
    default:
        break;
    }
    cm.Reset();
}
